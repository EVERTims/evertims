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
 * The Reader class for EVERTims
 *
 * (C) 2007 Lauri Savioja
 * Helsinki University of Technology
 *
 * (C) 2007-2015 Markus Noisternig
 * IRCAM-CNRS-UPMC UMR9912 STMS
 *
 ***********************************************************************/

#ifndef _READER_H
#define _READER_H

#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <cstring>

#include "socket.h"
#include "elRoom.h"

class Solver;

class Reader
{
public:
  Reader (const char *material_file, int input_socket, float threshold );

  void start ();

  void updateElements(const std::string& id, 
		      const EL::Room::Element& element);
  void createElementList();

  void initializeMembers(EL::Room& room);
  void getRoom(EL::Room& room);

  EL_FORCE_INLINE MaterialFile& getMaterials() { return m_materials; };

  EL_FORCE_INLINE void attachSolver (Solver *solver) { m_solver = solver; }
  void signalSolver ();

  EL_FORCE_INLINE bool geometryInitialized() { return m_geometry_initialized; };

  Socket *initializeInputSocket() { return new Socket(m_input_port); };

  void parseSource ( std::string& msg );
  void parseListener ( std::string& msg );

  void printSourcesAndListeners();

private:
  int m_input_port;
  float m_threshold;

  MaterialFile m_materials;
  std::map<std::string, EL::Room::Element> emap;

  std::vector<EL::Room::Element> m_elements;
  std::map<std::string, EL::Listener> m_listeners;
  std::map<std::string, EL::Source> m_sources;

  bool m_geometry_initialized;

  Solver *m_solver;
};

#endif
