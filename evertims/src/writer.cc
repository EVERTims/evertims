/*************************************************************************
 *
 * This file is part of the EVERT Library / EVERTims program for room 
 * acoustics simulation.
 *
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 2 of the License, or any later version.
 *
 * THIS PROGRAM IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL; BUT WITHOUT 
 * ANY WARRANTY; WITHIOUT EVEN THE IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, see https://www.gnu.org/licenses/gpl-2.0.html or write 
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301, USA.
 *
 * Copyright
 *
 * (C) 2004-2005 Samuli Laine
 * Helsinki University of Technology
 *
 * (C) 2008-2017 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ************************************************************************/
 
 
#define SPEED_OF_SOUND 340.0
#define MAX_RESPONSE_TIME 2.0
#define SAMPLE_RATE 22050

#include <iostream>

#include "elSource.h"
#include "elPolygon.h"
#include "elListener.h"

#include "writer.h"
#include "OSC-client.h"
// #include "reverbEstimate.h"
#include "utils.h"

using namespace std;

void Writer::parseOrder(char *s)
{
    char *maxStart = 0;
    char *amountStart = 0;
    
    maxStart = strchr(s, '-');
    if( maxStart ){ m_maxOrder = atoi(maxStart+1); }
    if( maxStart != s ){ m_minOrder = atoi(s); }
    if( !m_maxOrder ){ m_maxOrder = m_minOrder + (1024*1024); }
    
    amountStart = strchr(s, ',');
    if( amountStart ){ m_maxAmount = atoi(amountStart+1); }
    
}

Writer::Writer (char *addr):
m_minOrder(0),
m_maxOrder(1024 * 1024),
m_maxAmount(1024 * 1024)
{
    char *s = strchr(addr, '/');
    
    if( s )
    {
        m_pattern = addr;
        *s = '\0';
        m_host = s+1;
        s = strchr(addr, '(');
        if (s)
        {
            s = s + 1;
            *(s-1) = '\0';
            parseOrder(s);
        }
    }
    else
    {
        m_pattern = ".*";
        m_host = addr;
    }
    
    char errbuf[1024];
    int err = regcomp ( &m_preq, m_pattern, REG_EXTENDED | REG_NOSUB );
    
    if( err != 0 )
    {
        regerror ( err, &m_preq, errbuf, 1024 );
        cout << "Regcomp error: " << errbuf << endl;
    }
    
    cout << "Initializing new writer. Path pattern = " << m_pattern << ". Order: " << m_minOrder << " - " << m_maxOrder;
    cout << ". Amount: " << m_maxAmount << ". Host = " << m_host << endl;
}

bool Writer::match( const char* id )
{
    int result = regexec ( &m_preq, id, 0, 0, 0);
    
    cout << "Match of " << id << " resulted " << result << " for " << m_pattern << endl;
    
    return (result == 0);
}

void Writer::connect ()
{
    m_socket = new Socket(m_host);
    
    if( m_socket ){ std::cout << "New writer socket opened." << std::endl; }
}

void Writer::disconnect () { delete m_socket; }

AuralizationWriter::AuralizationWriter (char *addr):
Writer(addr)
{
    OSC_initBuffer(&m_oscbuf, BUF_SIZE, m_writeBuf);
    OSC_resetBuffer(&m_oscbuf);
}

std::string AuralizationWriter::getPathName(const EL::PathSolution::Path& p)
{
    std::string pathName;
    
    if (p.m_order > 0)
    {
        for (int i=0; i < p.m_order ; i++)
        {
            pathName.append( p.m_polygons[i]->getName() );
        }
    }
    else{ pathName.append("dir"); }
    
    return pathName;
}  

int AuralizationWriter::getPathIDandState(const EL::PathSolution::Path& p, enum PathState& state)
{
    std::string pathName = getPathName(p);
    
    //  std::cout << "Path name = " << pathName << std::endl;
    
    state = m_pathMap[pathName].m_state;
    
    return m_pathMap[pathName].m_id;
}

#define OSC_SAFE(OPERATION) if (OSC_freeSpaceInBuffer(&m_oscbuf) <= 4) cout << "OSC buffer full. Please increase!" << endl; else OPERATION

void AuralizationWriter::createSourceMessage(const EL::Source& source)
{
    const EL::Vector3& p = source.getPosition();
    const EL::Matrix3& mRot = source.getOrientation();
    const EL::Vector3& eul = source.getOrientation().toEuler();
    COUT << "source pos: (" << p[0] << ", " << p[1] << ", " << p[2] << ") rot: (" << eul[0] << ", " << eul[1] << ", " << eul[2] << ")" << "\n";
    const std::string& name = source.getName();
    char *cptr = strdup(name.c_str());
    
    // add header to OSC msg
    OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/source", ",sffffffffffff");)
    
    // add position information to OSC msg
    OSC_SAFE(OSC_writeStringArg(&m_oscbuf, cptr);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[0]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[1]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[2]);)
    
    // add orientation information to OSC msg
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // WARNING: trying to access mRot[i][j] will not return the full matrix here
            OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, mRot.getRow(i)[j]);)
        }
    }
    
    // free alloc
    free(cptr);
}

void AuralizationWriter::createListenerMessage(const EL::Listener& listener)
{
    const EL::Vector3& p = listener.getPosition();
    const EL::Matrix3& mRot = listener.getOrientation();
    const EL::Vector3& eul = listener.getOrientation().toEuler();
    COUT << "listener pos: (" << p[0] << ", " << p[1] << ", " << p[2] << ") rot: (" << eul[0] << ", " << eul[1] << ", " << eul[2] << ")" << "\n";
    const std::string& name = listener.getName();
    char *cptr = strdup(name.c_str());
    
    // add header to OSC msg
    OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/listener", ",sffffffffffff");)
    OSC_SAFE(OSC_writeStringArg(&m_oscbuf, cptr);)
    
    // add position information to OSC msg
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[0]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[1]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[2]);)
    
    // add orientation information to OSC msg
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // WARNING: trying to access mRot[i][j] will not return the full matrix here
            OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, mRot.getRow(i)[j]);)
        }
    }
    
    // free alloc
    free(cptr);
}

void AuralizationWriter::createReflectionMessage(int pathID,
                                                 enum PathState state,
                                                 const EL::Vector3& p0,
                                                 const EL::Vector3& pN,
                                                 float len,
                                                 int order,
                                                 float *reflectance)

{
    if ( state == FADE_IN )
    {
        OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/in" , ",iifffffffffffffffff");)
    }
    else
    {
        OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/upd", ",iifffffffffffffffff");)
    }
    
    OSC_SAFE(OSC_writeIntArg(&m_oscbuf, pathID);)
    
    OSC_SAFE(OSC_writeIntArg(&m_oscbuf, order);)
    
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p0[0]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p0[1]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p0[2]);)
    
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, pN[0]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, pN[1]);)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, pN[2]);)
    
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, len);)
    
    for (int i=0;i<10;i++)
    {
        OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, 1-reflectance[i]);)
    }
}

void AuralizationWriter::createInvisMessage( int pathID )
{
    OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/out", ",i");)
    OSC_SAFE(OSC_writeIntArg(&m_oscbuf, pathID);)
}

int AuralizationWriter::getNewID()
{
    static int s_next = 0;
    int id;
    
    if ( !m_releaved.empty () )
    {
        id = m_releaved.back ();
        m_releaved.pop_back ();
    }
    else
    {
        id = s_next;
        s_next++;
    }
    return id;
}

void  AuralizationWriter::markExistingPaths (EL::PathSolution *solution)
{
    string pathName;
    
    for (map<std::string, struct PathNode>::iterator it = m_pathMap.begin();
         it != m_pathMap.end(); it++)
    {
        it->second.m_state = FADE_OUT;
    }
    
    int pathCount = 0;
    for (int i=0; i < solution->numPaths(); i++)
    {
        const EL::PathSolution::Path& path = solution->getPath(i);
        
        if( pathCount >= m_maxAmount ){ break; }
        if( (path.m_order < m_minOrder) || (path.m_order > m_maxOrder)){ continue; }
        
        pathCount++;
        
        pathName = getPathName ( path );
        
        map<std::string, struct PathNode>::iterator it = m_pathMap.find ( pathName );
        if ( it == m_pathMap.end() )
        {
            m_pathMap[pathName].m_state = FADE_IN;
            m_pathMap[pathName].m_id = getNewID ();
        }
        else{ m_pathMap[pathName].m_state = UPDATE; }
    }
}

void AuralizationWriter::releaveLeftOverPaths()
{
    for (map<std::string, struct PathNode>::iterator it = m_pathMap.begin();
         it != m_pathMap.end(); it++)
    {
        if ( it->second.m_state == FADE_OUT )
        {
            createInvisMessage ( it->second.m_id );
            m_releaved.push_back ( it->second.m_id );
            m_pathMap.erase ( it );
        }
    }
}

void AuralizationWriter::writeMajor(EL::PathSolution *solution)
{
    int pathID;
    float len;
    enum PathState state;
    float reflectance[10];
    const EL::Source& source = solution->getSource();
    const EL::Listener& listener = solution->getListener();
    OSCTimeTag tt;
    int error;
    
    error = OSC_openBundle(&m_oscbuf, tt);
    if( error ){ printf("OSC error: %s\n", OSC_errorMessage); }
    
    createSourceMessage ( source );
    createListenerMessage ( listener );
    
    markExistingPaths ( solution );
    releaveLeftOverPaths ();
    
    error = OSC_closeBundle(&m_oscbuf);
    if( error ){ printf("OSC error: %s\n", OSC_errorMessage); }
    
    m_socket->write(OSC_packetSize(&m_oscbuf), OSC_getPacket(&m_oscbuf));
    OSC_resetBuffer(&m_oscbuf);
    
    int pathCount = 0;
    for (int i=0; i < solution->numPaths (); i++)
    {
        const EL::PathSolution::Path& path = solution->getPath(i);
        
        if( pathCount >= m_maxAmount ){ break; }
        if( (path.m_order < m_minOrder) || (path.m_order > m_maxOrder) ){ continue; }
        pathCount++;
        
        pathID = getPathIDandState ( path, state );
        len = solution->getLength (path);
        
        const EL::Vector3& p0 = path.m_points[1];
        const EL::Vector3& pN = path.m_points[path.m_points.size () - 2];
        
        for( int k = 0; k < 10; k++ ){ reflectance[k] = 1.0; }
        
        for( int j = 0; j < path.m_order; j++ )
        {
            const EL::Polygon* p = path.m_polygons[j];
            const Material& m = p->getMaterial ();
            for( int k = 0; k < 10; k++ ){ reflectance[k] *= ( 1 - m.absorption[k] ); }
        }
        
        createReflectionMessage(pathID, state, p0, pN, len, path.m_order, reflectance);
        m_socket->write(OSC_packetSize(&m_oscbuf), OSC_getPacket(&m_oscbuf));
        OSC_resetBuffer(&m_oscbuf);
    }
    
    
// DISCARDED: RT60 sent by Blender add-on for now
//
//    // SEND R60
//    OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/r60" , ",ffffffffff");)
//    
//    ReverbEstimator r(SAMPLE_RATE, solution, SPEED_OF_SOUND, MAX_RESPONSE_TIME);
//    
//    solution->print( m_minOrder, m_maxOrder, m_maxAmount );
//    
//    double minTime_ampl, minTime_time;
//    double maxTime_ampl, maxTime_time;
//    
//    for( int i=0; i<10; i++ )
//    {
//        // get schroeder integrate min / max values
//        r.getMaxMin(i, minTime_ampl, minTime_time, maxTime_ampl, maxTime_time);
//        double startR60 = minTime_ampl * 1.1;
//        double endR60   = maxTime_ampl * 0.9;
//        
//        // compute RT60
//        double R60 = r.getEstimateR60(i, startR60, endR60);
//        printf( "%d. band: first %fdB at %fs, last %fdB at %fs. R60 = %f\n", i, minTime_ampl, minTime_time, maxTime_ampl, maxTime_time, R60 );
//        OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, (float)R60);)
//    }
//    
//    m_socket->write(OSC_packetSize(&m_oscbuf), OSC_getPacket(&m_oscbuf));
//    OSC_resetBuffer(&m_oscbuf);
}

void AuralizationWriter::writeMinor(EL::PathSolution *solution, int listSrcOrBoth)
{
    const EL::Listener& listener = solution->getListener();
    const EL::Source& source = solution->getSource();
    OSCTimeTag tt;
    int error;
    
    // open OSC bundle
    error = OSC_openBundle(&m_oscbuf, tt);
    if( error ){ printf("OSC error: %s\n", OSC_errorMessage); }
    
    // send specific message based on who (source or listener) needs the update
    if( listSrcOrBoth == 0 ){ createListenerMessage ( listener ); }
    else if( listSrcOrBoth == 1 ){ createSourceMessage ( source ); }
    else
    {
        createListenerMessage ( listener );
        createSourceMessage ( source );
    }
    
    // close OSC bundle
    error = OSC_closeBundle(&m_oscbuf);
    if( error ){ printf("OSC error: %s\n", OSC_errorMessage); }
    
    m_socket->write(OSC_packetSize(&m_oscbuf), OSC_getPacket(&m_oscbuf));
    OSC_resetBuffer(&m_oscbuf);
}

#define ABS(x) ((x)>0 ? (x) : (-(x)))

void VisualizationWriter::writeMajor(EL::PathSolution *solution)
{
    int numLines = 1;
    bool interesting;
    int pathCount = 0;
    
    for( int i=0; i < solution->numPaths(); i++ )
    {
        const EL::PathSolution::Path& path = solution->getPath(i);
        
        if( pathCount >= m_maxAmount ){ break; }
        if( (path.m_order < m_minOrder) || (path.m_order > m_maxOrder) ){ continue; }
        pathCount++;
        
        interesting = true;
        
        if( interesting )
        {
            for( int j=0; j < path.m_points.size()-1; j++ )
            {
                const EL::Vector3& p0 = path.m_points[j];
                const EL::Vector3& p1 = path.m_points[j+1];
                sprintf(m_writeBuf, "/line_on %d %f %f %f %f %f %f", numLines++, p0[0], p0[1], p0[2],
                        p1[0], p1[1], p1[2]);
                m_socket->write(strlen(m_writeBuf), m_writeBuf);
            }
        }
    }
    
    for( int i=numLines; i < m_numLines ; i++ )
    {
        sprintf(m_writeBuf, "/line_off %d", i);
        m_socket->write(strlen(m_writeBuf), m_writeBuf);
    }
    
    m_numLines = numLines;
}

void VisualizationWriter::writeMinor(EL::PathSolution *solution, int listSrcOrBoth) {}

void PrintWriter::writeMajor(EL::PathSolution *solution)
{
//  int numLines = 0;
//  ReverbEstimator r(SAMPLE_RATE, solution, SPEED_OF_SOUND, MAX_RESPONSE_TIME);
//
//  solution->print(m_minOrder, m_maxOrder, m_maxAmount);
//
//  double minValue, minTime;
//  double maxValue, maxTime;
//
//  for (int i=0; i<10; i++)
//    {
//      r.getMaxMin(i, minValue, minTime, maxValue, maxTime);
//      double startR60 = minValue * 1.1;
//      double endR60   = maxValue * 0.9;
//      double R60      = r.getEstimateR60(i, startR60, endR60);
//      printf("%d. band: first %f at %f, last %f at %f. R60 = %f\n", i, maxValue, maxTime, minValue, minTime, R60 );
//    }

}

void PrintWriter::writeMinor(EL::PathSolution *solution, int listSrcOrBoth) {}
