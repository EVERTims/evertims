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
 

#ifndef _SOLVER_H
#define _SOLVER_H

#include <pthread.h>

#include "elAABB.h"
#include "elBeam.h"
#include "elBSP.h"
#include "elPathSolution.h"
#include "elPolygon.h"
#include "elRay.h"
#include "elRoom.h"
#include "elVector.h"

#include "reader.h"
#include "writer.h"

#define MAX_NUM_SOLUTIONS 64

class Solver
{
    
public:
    
    enum Status
    {
        CHANGED,
        IN_PROCESS,
        UPDATED
    };
    
    struct SolutionNode
    {
        enum Status          m_listener_status_major;
        enum Status          m_listener_status_minor;
        enum Status          m_source_status_minor;
        enum Status          m_geom_or_source_status;
        bool                 m_to_send;
        // double buffering for the source and listener
        EL::Source           m_source[2];
        EL::Listener         m_listener[2];
        int m_current;
        
        EL::Vector3          m_new_source_position;
        EL::Matrix3          m_new_source_orientation;
        EL::Vector3          m_new_listener_position;
        EL::Matrix3          m_new_listener_orientation;
        EL::PathSolution     *m_solution;
        std::vector<Writer *> m_writers;
    };
    
    Solver (int mindepth, int maxdepth, bool graphics);
    ~Solver ();
    
    inline void calculateNextSolution ()
    {
        m_next_solution->solve ();
        printf ( "Solved!\n" );
        //    m_next_solution->update ();
        //    printf ( "Updated!\n" );
        //    m_next_solution->update ();
        pthread_mutex_lock (&next_solution_mutex);
        m_next_solution_flag = true;
        pthread_mutex_unlock (&next_solution_mutex);
    }
    
    inline void attachReader ( Reader *re ) { m_reader = re; }
    inline void addWriter ( Writer *wr ) { m_writers.push_back(wr); }
    
    void readRoomDescription (const char* filename, MaterialFile& materials);
    
    void update ();
    void draw ();
    bool readyToDraw () { return m_ready_to_draw; };
    void cleanDrawFlag () { m_ready_to_draw = false; };
    
    void markGeometryChanged  ();
    void createNewSolutionNode ( const EL::Source& source, const EL::Listener& listener );
    void markSourceMovementMajor   ( const EL::Source& source, const EL::Listener& listener );
    void markSourceMovementMinor   ( const EL::Source& source, const EL::Listener& listener );
    void markListenerMovementMajor ( const EL::Source& source, const EL::Listener& listener );
    void markListenerMovementMinor ( const EL::Source& source, const EL::Listener& listener );
    
    
private:
    
    void createNewSolution    ( int depth, const EL::Source &source, const EL::Listener& listener );
    void interruptCalculation ();
    
    void mapAvailableSolutionNodes ();
    
    int  m_min_depth;
    int  m_max_depth;
    bool m_next_solution_flag;
    bool m_request_for_stop;
    bool m_start_new;
    bool m_graphics;
    bool m_ready_to_draw;
    
    int m_current_room;
    bool isLoadingNewRoom;
    
    // "Doublebuffering" for the data structures
    EL::Room m_room[20];
    EL::PathSolution *m_next_solution;
    
    pthread_t graphics_thread;
    
    pthread_t path_solver_thread;
    pthread_mutex_t next_solution_mutex;
    
    struct SolutionNode m_solutionNodes[MAX_NUM_SOLUTIONS];
    int m_lastMappedSolutionNode;
    int m_lastAvailableSolutionNode;
    bool m_newSolutionNodesAvailable;
    
    typedef std::map<std::string, struct SolutionNode *> t_solutionNodeMap;
    typedef t_solutionNodeMap::iterator t_solutionNodeIterator;
    
    t_solutionNodeMap m_solutionNodeMap;
    
    std::vector<Writer *> m_writers;
    Reader *m_reader;
};

void *path_solver_function (void *data);
void *graphics_function (void *data);

#endif
