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
 

#ifndef _READER_H
#define _READER_H

#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <cstring>

#include "socket.h"
#include "elRoom.h"
#include "utils.h"

class Solver;

class Reader
{
    
public:
    
    Reader (const char *material_file, int input_socket, float threshold_loc, float threshold_rot );
    
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
    float m_threshold_loc;
    float m_threshold_rot;
    
    MaterialFile m_materials;
    std::map<std::string, EL::Room::Element> emap;
    
    std::vector<EL::Room::Element> m_elements;
    std::map<std::string, EL::Listener> m_listeners;
    std::map<std::string, EL::Source> m_sources;
    
    bool m_geometry_initialized;
    
    Solver *m_solver;
};

#endif
