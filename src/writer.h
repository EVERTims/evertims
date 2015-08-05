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
 * (C) 2007-2015 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ***********************************************************************/

#ifndef _WRITER_H
#define _WRITER_H

#include "elPathSolution.h"
#include "OSC-client.h"
#include "socket.h"

#include <map>
#include <cstring>
#include <regex.h>

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
  virtual void write (EL::PathSolution *solution) { return; };

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
  void write (EL::PathSolution *solution);

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
  void write (EL::PathSolution *solution);

 private:
  int m_numLines;
};


class PrintWriter : public Writer
{
 public:
  PrintWriter (char *addr): Writer(addr) {};

  virtual const char* getType() { return "Print"; };
  void write (EL::PathSolution *solution);
};

#endif
