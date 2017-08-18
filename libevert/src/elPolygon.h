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
 

#ifndef __ELPOLYGON_HPP
#define __ELPOLYGON_HPP

#if !defined (__ELAABB_HPP)
#	include "elAABB.h"
#endif
#if !defined (__ELVECTOR_HPP)
#	include "elVector.h"
#endif

#include "material.h"


#include <iostream>

namespace EL
{

class AABB;
class Beam;

class Polygon
{
    
public:
    
    Polygon (void);
    Polygon (Material material);
    Polygon (const Polygon& p);
    Polygon (const Vector3* points, int numPoints);
    Polygon (const Vector3* points, int numPoints, Material material, const std::string& name);
    Polygon (const Vector3* points, int numPoints, const Vector4& pleq, const std::string& name);
    Polygon (const Vector3* points, int numPoints, const Vector4& pleq, Material material);
    Polygon (const Vector3* points, int numPoints, const Vector4& pleq, Material material, unsigned long id, std::string name);
    Polygon (const std::vector<Vector3>& points);
    Polygon (const std::vector<Vector3>& points, Material material);
    
    ~Polygon (void);
    
    const Polygon& operator= (const Polygon& p);
    
    int numPoints (void) const { return m_points.size(); }
    
    EL_FORCE_INLINE Vector3& operator[] (int i)
    {
        EL_ASSERT(i >= 0 && i < (int)m_points.size()); return m_points[i];
    }
    
    EL_FORCE_INLINE const Vector3& operator[] (int i) const
    {
        EL_ASSERT(i >= 0 && i < (int)m_points.size()); return m_points[i];
    }
    
    const Vector4& getPleq (void) const { return m_pleq;   }
    const Vector3& getNormal (void) const { return *((const Vector3*)&m_pleq); }
    
    bool isConvex (void) const;
    float getNonPlanarity (void) const;
    float getArea (void) const;
    AABB getAABB (void) const;
    
    void triangulate (std::vector<Polygon>& triangles);
    void splitConvex (std::vector<Polygon>& polygons);
    
    enum ClipResult
    {
        CLIP_CLIPPED,
        CLIP_ORIGINAL,
        CLIP_VANISHED
    };
    
    ClipResult clip (const Vector4& pleq);
    ClipResult clip (const AABB& aabb);
    ClipResult clip (const Beam& beam);
    
    //void					render		(const Vector3& color) const;
    
    EL_FORCE_INLINE void setMaterial (Material material) { m_material = material; }
    EL_FORCE_INLINE Material getMaterial (void) const { return m_material; }
    
    EL_FORCE_INLINE void setID (unsigned long id) { m_id = id; std::cout << id << std::endl; }
    EL_FORCE_INLINE unsigned long getID (void) const { return m_id; }
    
    EL_FORCE_INLINE std::string getName (void) const { return m_name; }
    
    void expand (float eps);
    void print () const;
    
    
private:
    
    void calculatePleq(void);
    static bool mergePartials( std::vector<std::vector<int> >& partials, const std::vector<Vector3>& vertices );
    
    static ClipResult clipInner	(const Vector3* inPoints, int numInPoints, Vector3* outPoints, int& numOutPoints, const Vector4& pleq);
    
    static std::vector<Vector3> s_clipBuffer[2];
    std::vector<Vector3> m_points;
    Vector4 m_pleq;
    Material m_material;
    unsigned long m_id;
    static unsigned long s_polygonCounter;
    std::string m_name;
};
    
} // namespace EL

#endif // __ELPOLYGON_HPP
