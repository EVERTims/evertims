#ifndef __ELPATHSOLUTION_HPP
#define __ELPATHSOLUTION_HPP
/*************************************************************************
 *
 * This file is part of the EVERT beam-tracing Library
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
 * (C) 2004-2005 Samuli Laine
 * Helsinki University of Technology  
 *
 ************************************************************************/

#if !defined (__ELVECTOR_HPP)
#	include "elVector.h"
#endif

//#define SUPER_VECTOR

#ifdef SUPER_VECTOR
#include "../superVector.h"
#endif

namespace EL
{

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
		int							m_order;
		std::vector<Vector3>		m_points;
		std::vector<const Polygon*>	m_polygons;
	};


				PathSolution	(const Room& room,
						 const Source& source,
						 const Listener& listener,
						 int maximumOrder,
						 bool changed);
				~PathSolution	(void);
				
	void			solve	   (void);
	void			update	   (void);

	int	       		numPaths   (void) const			{ return m_paths.size(); }
	const Path&		getPath	   (int i) const		{ EL_ASSERT(i >= 0 && i < numPaths()); return m_paths[i]; }
	const Listener &        getListener(void)                       { return m_listener; }
	const Source &          getSource  (void)                         { return m_source; }
	int                     getOrder   (void)                         { return m_maximumOrder; }
	void			renderPath (const Path& path) const;
	bool			save	   (char *filename, 
					    char *modelname);
	void                    print      (void);
	inline bool             getChanged (void)                       { return m_changed; }
 private:
	PathSolution	(const PathSolution&);	// prohibit
	const PathSolution&	operator=		(const PathSolution&);	// prohibit

	struct SolutionNode;

	void			solveRecursive	(const Vector3& source,
						 const Vector3& target,
						 const Beam& beam,
						 int order,
						 int parentIndex);

	void			validatePath	(const Vector3& source,
						 const Vector3& target,
						 int nodeIndex,
						 Vector4& failPlane);

	static Vector4		getFailPlane	(const Beam& beam,
						 const Vector3& target);

	void			clearCache	(void);

	const Room&		        m_room;
	const Source&		        m_source;
	const Listener&		        m_listener;
	int			        m_maximumOrder;
	bool                            m_changed;

	std::vector<const Polygon*>	m_polygonCache;
	std::vector<Vector3>		m_validateCache;
	std::multimap<float, int>	m_pathFirstSet;

#ifndef SUPER_VECTOR
	std::vector<SolutionNode>	m_solutionNodes;
	std::vector<Vector4>		m_failPlanes;
#else
	SuperVector<SolutionNode>       m_solutionNodes;
	SuperVector<Vector4>            m_failPlanes;
#endif
	
	std::vector<Vector4>		m_distanceSkipCache;
	Vector3				m_cachedSource;

	std::vector<Path>		m_paths;
};

//------------------------------------------------------------------------
} // namespace EL

#endif // __ELPATHSOLUTION_HPP
