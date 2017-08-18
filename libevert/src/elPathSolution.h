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
 

#ifndef __ELPATHSOLUTION_HPP
#define __ELPATHSOLUTION_HPP

#if !defined (__ELVECTOR_HPP)
#	include "elVector.h"
#endif

//#define SUPER_VECTOR

#ifdef SUPER_VECTOR
#include "../superVector.h"
#endif

namespace EL
{

static bool stop_signal;
inline void setStopSignalValue( bool value ){ stop_signal = value; };
inline bool getStopSignalValue( ){ return stop_signal; };
    
//------------------------------------------------------------------------

class Beam;
class Listener;
class Polygon;
class Room;
class Source;

class PathSolution
{
    
public:
    
    struct Path
    {
        int m_order;
        std::vector<Vector3> m_points;
        std::vector<const Polygon*>	m_polygons;
    };
    
    PathSolution (const Room& room, const Source& source, const Listener& listener, int maximumOrder, bool changed);
    
    ~PathSolution (void);
    
    void solve (void);
    void update (void);
    
    int numPaths (void) const { return m_paths.size(); }
    
    const Path& getPath (int i) const
    { EL_ASSERT(i >= 0 && i < numPaths()); return m_paths[i]; }
    
    const Listener & getListener (void) { return m_listener; }
    const Source & getSource (void) { return m_source; }
    
    int getOrder (void) { return m_maximumOrder; }
    void renderPath (const Path& path) const;
    bool save (char *filename, char *modelname);
    
    void print (int minOrder, int maxOrder, int maxAmount);
    float getLength (const Path& path);
    inline bool getChanged (void) { return m_changed; }
    
    
private:
    
    PathSolution (const PathSolution&);	// prohibit
    const PathSolution&	operator= (const PathSolution&);	// prohibit
    
    struct SolutionNode;
    
    void solveRecursive	(const Vector3& source, const Vector3& target, const Beam& beam, int order, int parentIndex);
    
    void validatePath (const Vector3& source, const Vector3& target, int nodeIndex, Vector4& failPlane);
    
    static Vector4 getFailPlane	(const Beam& beam, const Vector3& target);
    
    void clearCache	(void);
    
    const Room& m_room;
    const Source& m_source;
    const Listener& m_listener;
    int m_maximumOrder;
    bool m_changed;
    
    std::vector<const Polygon*> m_polygonCache;
    std::vector<Vector3> m_validateCache;
    std::multimap<float, int> m_pathFirstSet;
    
#ifndef SUPER_VECTOR
    std::vector<SolutionNode> m_solutionNodes;
    std::vector<Vector4> m_failPlanes;
#else
    SuperVector<SolutionNode> m_solutionNodes;
    SuperVector<Vector4> m_failPlanes;
#endif
    
    std::vector<Vector4> m_distanceSkipCache;
    Vector3 m_cachedSource;
    
    std::vector<Path> m_paths;
};
    
} // namespace EL

#endif // __ELPATHSOLUTION_HPP
