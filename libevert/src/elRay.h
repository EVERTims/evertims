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
 

#ifndef __ELRAY_HPP
#define __ELRAY_HPP

#if !defined (__ELPOLYGON_HPP)
#	include "elPolygon.h"
#endif
#if !defined (__ELVECTOR_HPP)
#	include "elVector.h"
#endif

namespace EL
{
    
class Ray
{
    
public:
    
    Vector3 m_a;
    Vector3 m_b;
    
    EL_FORCE_INLINE Ray (void) {}
    EL_FORCE_INLINE Ray (const Vector3& a, const Vector3& b) : m_a(a), m_b(b) {}
    EL_FORCE_INLINE Ray	(const Ray& ray): m_a(ray.m_a), m_b(ray.m_b) {}
    EL_FORCE_INLINE ~Ray(void) {}
    
    EL_FORCE_INLINE const Ray& operator= (const Ray& ray) { m_a = ray.m_a; m_b = ray.m_b; return *this; }
    
    //void						render		(const Vector3& color) const;
    
    EL_FORCE_INLINE bool intersect (const Polygon& polygon) const
    {
        float s0 = dot(m_a, polygon.getPleq());
        float s1 = dot(m_b, polygon.getPleq());
        
        if( s0*s1 >= 0.f ){ return false; }
        
        int n = polygon.numPoints();
        
        Vector3 dir = m_b - m_a;
        Vector3 eb  = polygon[n-1] - m_a;
        float sign = 0.f;
        for( int i=0; i < n; i++ )
        {
            Vector3 ea = eb;
            eb = polygon[i] - m_a;
            
            float det = dot(dir, cross(ea, eb));
            
            if( sign == 0.f ){ sign = det; }
            else if( det * sign < 0.f ){ return false; }
        }
        
        return (sign != 0.f);
    }
    
    EL_FORCE_INLINE bool intersectExt(const Polygon& polygon) const
    {
        int n = polygon.numPoints();
        
        Vector3 dir = m_b - m_a;
        Vector3 eb  = polygon[n-1] - m_a;
        float sign = 0.f;
        for( int i=0; i < n; i++ )
        {
            Vector3 ea = eb;
            eb = polygon[i] - m_a;
            
            float det = dot(dir, cross(ea, eb));
            
            if( sign == 0.f ){ sign = det; }
            else if( det * sign < 0.f ){ return false; }
        }
        
        return (sign != 0.f);
    }
};

EL_FORCE_INLINE Vector3 intersect(const Ray& ray, const Vector4& pleq)
{
    float s0 = dot(ray.m_a, pleq);
    float s1 = dot(ray.m_b, pleq);
    
    return ray.m_a + (s0/(s0-s1)) * (ray.m_b - ray.m_a);
}

} // namespace EL

#endif // __ELRAY_HPP
