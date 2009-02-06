/***********************************************************************
 *
 * This file is part of the EVERTims room acoustic modeling software.
 * It is released under the MIT License. You should have received a
 * copy of the MIT License along with EVERTims.  If not, see
 * http://www.opensource.org/licenses/mit-license.php
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * For details, see the LICENSE file
 *
 * The Writer class for EVERTims
 *
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology 
 *
 ***********************************************************************/

#include "writer.h"
#include "OSC-client.h"

#include "elSource.h"
#include "elPolygon.h"
#include "elListener.h"

#include <iostream>

using namespace std;

Writer::Writer (char *addr)
{
  char *s = strchr(addr, '/');

  if (s)
    {
      m_pattern = addr;
      *s = '\0';
      m_host = s+1;
    }
  else
    {
      m_pattern = ".*";
      m_host = addr;
    }

  char errbuf[1024];
  int err = regcomp ( &m_preq, m_pattern, REG_EXTENDED | REG_NOSUB );

  if ( err != 0 )
    {
      regerror ( err, &m_preq, errbuf, 1024 );
      cout << "Regcomp error: " << errbuf << endl;
    }

  cout << "Initializing new writer. Path pattern = " << m_pattern << ". Host = " << m_host << endl;
}

bool Writer::match ( const char* id )
{
  int result = regexec ( &m_preq, id, 0, 0, 0);

  cout << "Match of " << id << " resulted " << result << " for " << m_pattern << endl;

  return (result == 0);
}

void Writer::connect ()
{
  m_socket = new Socket(m_host);
  
  if (m_socket)
    std::cout << "New writer socket opened." << std::endl;
}

void Writer::disconnect ()
{
  delete m_socket;
}


MarkusWriter::MarkusWriter (char *addr): 
  Writer(addr)
{
  OSC_initBuffer(&m_oscbuf, BUF_SIZE, m_writeBuf);
  OSC_resetBuffer(&m_oscbuf);
}

std::string MarkusWriter::getPathName(const EL::PathSolution::Path& p)
{
  std::string pathName;

  if (p.m_order > 0)
    for (int i=0; i < p.m_order ; i++)
    {
      pathName.append( p.m_polygons[i]->getName() );
    }
  else
    pathName.append("dir");

  return pathName;
}  

int MarkusWriter::getPathIDandState(const EL::PathSolution::Path& p, enum PathState& state)
{
  std::string pathName = getPathName(p);

  //  std::cout << "Path name = " << pathName << std::endl;

  state = m_pathMap[pathName].m_state;

  return m_pathMap[pathName].m_id;
}

float MarkusWriter::getPathLength(const EL::PathSolution::Path& path)
{
  float len = 0;

  for (int j=0; j < path.m_points.size()-1; j++)
  {
    const EL::Vector3& p0 = path.m_points[j];
    const EL::Vector3& p1 = path.m_points[j+1];

    len += (p1-p0).length();
  }

  return len;
}

#define OSC_SAFE(OPERATION) if (OSC_freeSpaceInBuffer(&m_oscbuf) <= 4) cout << "OSC buffer full. Please increase!" << endl; else OPERATION

void MarkusWriter::createSourceMessage(const EL::Source& source)
{
  const EL::Vector3& p = source.getPosition();
  const std::string& name = source.getName();
  char *cptr = strdup(name.c_str());

  OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/source", ",sfff");)
  OSC_SAFE(OSC_writeStringArg(&m_oscbuf, cptr);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[0]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[1]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[2]);)

  free(cptr);
}

void MarkusWriter::createListenerMessage(const EL::Listener& listener)
{
  const EL::Vector3& p = listener.getPosition();
  const std::string& name = listener.getName();
  char *cptr = strdup(name.c_str());

  OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/listener", ",sfff");)
  OSC_SAFE(OSC_writeStringArg(&m_oscbuf, cptr);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[0]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[1]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p[2]);)

  free(cptr);
}

void MarkusWriter::createReflectionMessage(int pathID, 
					   enum PathState state,
					   const EL::Vector3& p0, 
					   const EL::Vector3& pN, 
					   float len, 
					   int order, 
					   float *reflectance)

{
  if ( state == FADE_IN )
    OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/in", ",iiffffffffffffffff");)
  else
    OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/upd", ",iiffffffffffffffff");)

  OSC_SAFE(OSC_writeIntArg(&m_oscbuf, pathID);)

  OSC_SAFE(OSC_writeIntArg(&m_oscbuf, order);)

  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p0[0]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p0[1]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, p0[2]);)

  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, pN[0]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, pN[1]);)
  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, pN[2]);)

  OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, len);)

  for (int i=0;i<9;i++)
    OSC_SAFE(OSC_writeFloatArg(&m_oscbuf, 1-reflectance[i]);)
    
}

void MarkusWriter::createInvisMessage( int pathID )
{
  OSC_SAFE(OSC_writeAddressAndTypes(&m_oscbuf, "/out", ",i");)
  OSC_SAFE(OSC_writeIntArg(&m_oscbuf, pathID);)
}

int MarkusWriter::getNewID()
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

void  MarkusWriter::markExistingPaths (EL::PathSolution *solution)
{
  string pathName;

  for (map<std::string, struct PathNode>::iterator it = m_pathMap.begin();
       it != m_pathMap.end(); it++)
    it->second.m_state = FADE_OUT;

  for (int i=0; i < solution->numPaths(); i++)
  {
    const EL::PathSolution::Path& path = solution->getPath(i);
    pathName = getPathName ( path );
    
    map<std::string, struct PathNode>::iterator it = m_pathMap.find ( pathName );
    if ( it == m_pathMap.end() )
    {
      m_pathMap[pathName].m_state = FADE_IN;
      m_pathMap[pathName].m_id = getNewID ();
    }
    else
      m_pathMap[pathName].m_state = UPDATE;
  }
}

void MarkusWriter::releaveLeftOverPaths()
{
  for (map<std::string, struct PathNode>::iterator it = m_pathMap.begin();
       it != m_pathMap.end(); it++)
    if ( it->second.m_state == FADE_OUT )
    {
      createInvisMessage ( it->second.m_id );
      m_releaved.push_back ( it->second.m_id );
      m_pathMap.erase ( it );
    }
}

void MarkusWriter::write(EL::PathSolution *solution)
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
  if (error)
    printf("OSC error: %s\n", OSC_errorMessage);

  createSourceMessage ( source );
  createListenerMessage ( listener );

  markExistingPaths ( solution );
  releaveLeftOverPaths ();

  error = OSC_closeBundle(&m_oscbuf);
  if (error)
    printf("OSC error: %s\n", OSC_errorMessage);

  m_socket->write(OSC_packetSize(&m_oscbuf), OSC_getPacket(&m_oscbuf));
  OSC_resetBuffer(&m_oscbuf);

  for (int i=0; i < solution->numPaths (); i++)
  {
    const EL::PathSolution::Path& path = solution->getPath(i);
    pathID = getPathIDandState ( path, state );
    len = getPathLength ( path );
    
    const EL::Vector3& p0 = path.m_points[1];
    const EL::Vector3& pN = path.m_points[path.m_points.size () - 2];

    for (int k = 0; k < 10; k++) reflectance[k] = 1.0;

    for (int j = 0; j < path.m_order; j++)
    {
      const EL::Polygon* p = path.m_polygons[j];
      const Material& m = p->getMaterial ();
      for (int k = 0; k < 10; k++) reflectance[k] *= ( 1 - m.absorption[k] );
    }

    createReflectionMessage(pathID, state, p0, pN, len, path.m_order, reflectance);
    m_socket->write(OSC_packetSize(&m_oscbuf), OSC_getPacket(&m_oscbuf));
    OSC_resetBuffer(&m_oscbuf);
  }

  printf ( "Sent!\n" );
}


#define ABS(x) ((x)>0 ? (x) : (-(x)))

void VirchorWriter::write(EL::PathSolution *solution)
{
  int numLines = 1;
  bool interesting;

  for (int i=0; i < solution->numPaths(); i++)
  {
    const EL::PathSolution::Path& path = solution->getPath(i);

    interesting = true;

    if (interesting)
      for (int j=0; j < path.m_points.size()-1; j++)
	{
	  const EL::Vector3& p0 = path.m_points[j];
	  const EL::Vector3& p1 = path.m_points[j+1];
	  sprintf(m_writeBuf, "/line_on %d %f %f %f %f %f %f", numLines++, p0[0], p0[1], p0[2], 
		  p1[0], p1[1], p1[2]);
	  m_socket->write(strlen(m_writeBuf), m_writeBuf);
	}
  }

  for (int i=numLines; i < m_numLines ; i++)
  {
    sprintf(m_writeBuf, "/line_off %d", i);
    m_socket->write(strlen(m_writeBuf), m_writeBuf);
  }

  m_numLines = numLines;
}

void PrintWriter::write(EL::PathSolution *solution)
{
  int numLines = 0;

  solution->print();
}

