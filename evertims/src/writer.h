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
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology
 *
 * (C) 2008-2017 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ************************************************************************/
 
 
#ifndef _WRITER_H
#define _WRITER_H

#include <map>
#include <cstring>
#include <regex.h>

#include "elPathSolution.h"
#include "OSC-client.h"
#include "socket.h"

#define BUF_SIZE 16384

class Writer
{
    
public:
    
    Writer (char *addr);
    ~Writer () { disconnect(); };
    
    void parseOrder(char *s);
    bool match ( const char* id );
    
    void connect ();
    void disconnect ();
    
    virtual const char* getType() { return "Base"; };
    virtual void writeMajor (EL::PathSolution *solution) { return; };
    virtual void writeMinor (EL::PathSolution *solution, int listSrcOrBoth) { return; };
    
    
protected:
    
    Socket *m_socket;
    char m_writeBuf[BUF_SIZE];
    
    int  m_minOrder;
    int  m_maxOrder;
    int  m_maxAmount;
    
    
private:
    
    char *m_host;
    char *m_pattern;
    regex_t m_preq;
};

class AuralizationWriter : public Writer
{
    
public:
    
    AuralizationWriter (char *host);
    
    virtual const char* getType() { return "Auralization"; };
    void writeMajor (EL::PathSolution *solution);
    void writeMinor (EL::PathSolution *solution, int listSrcOrBoth);
    
    
private:
    
    enum PathState
    {
        FADE_IN,
        UPDATE,
        FADE_OUT
    };
    
    std::string    getPathName              ( const EL::PathSolution::Path& p );
    int            getPathIDandState        ( const EL::PathSolution::Path& p, enum PathState& state );
    float          getPathLength            ( const EL::PathSolution::Path& p );
    int            getNewID                 ( );
    
    void           createSourceMessage      ( const EL::Source& source );
    void           createListenerMessage    ( const EL::Listener& listener );
    void           createReflectionMessage  ( int pathID,
                                             enum PathState state,
                                             const EL::Vector3& p0,
                                             const EL::Vector3& pN,
                                             float len,
                                             int order,
                                             float *reflectance );
    void           createInvisMessage       ( int pathID );
    
    void           markExistingPaths        ( EL::PathSolution *solution );
    void           releaveLeftOverPaths     ( );
    
    OSCbuf m_oscbuf;
    
    struct PathNode
    {
        PathState    m_state;
        int          m_id;
    };
    
    std::map<std::string, struct PathNode> m_pathMap;
    std::vector<int> m_releaved;
};

class VisualizationWriter : public Writer
{
    
public:
    
    VisualizationWriter (char *host): Writer(host), m_numLines(0) {};
    
    virtual const char* getType() { return "VirChor"; };
    void writeMajor (EL::PathSolution *solution);
    void writeMinor (EL::PathSolution *solution, int listSrcOrBoth);
    
    
private:
    
    int m_numLines;
};


class PrintWriter : public Writer
{
    
public:
    
    PrintWriter (char *addr): Writer(addr) {};
    
    virtual const char* getType() { return "Print"; };
    void writeMajor (EL::PathSolution *solution);
    void writeMinor (EL::PathSolution *solution, int listSrcOrBoth);
};

#endif
