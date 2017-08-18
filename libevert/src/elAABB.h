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


#ifndef __ELAABB_HPP
#define __ELAABB_HPP

#if !defined (__ELVECTOR_HPP)
    #include "elVector.h"
#endif

namespace EL
{
    
    //------------------------------------------------------------------------
    
    class AABB
    {
    public:
        Vector3						m_mn;
        Vector3						m_mx;
        
        EL_FORCE_INLINE AABB ( void ) : m_mn(0.f, 0.f, 0.f), m_mx(0.f, 0.f, 0.f)	{}
        EL_FORCE_INLINE AABB ( const Vector3& mn, const Vector3& mx ) : m_mn(mn), m_mx(mx) {}
        EL_FORCE_INLINE AABB ( const AABB& aabb ) : m_mn(aabb.m_mn), m_mx(aabb.m_mx) {}
        EL_FORCE_INLINE ~AABB ( void ) {}
        EL_FORCE_INLINE const AABB&	operator= ( const AABB& aabb ) { m_mn = aabb.m_mn; m_mx = aabb.m_mx; }
        
        EL_FORCE_INLINE void grow ( const Vector3& p )
        {
            for( int j=0; j < 3; j++ )
            {
                if (p[j] < m_mn[j]) m_mn[j] = p[j];
                if (p[j] > m_mx[j]) m_mx[j] = p[j];
            }
        }
        EL_FORCE_INLINE bool overlaps ( const AABB& o ) const
        {
            return (m_mn.x < o.m_mx.x && m_mx.x > o.m_mn.x &&
                    m_mn.y < o.m_mx.y && m_mx.y > o.m_mn.y &&
                    m_mn.z < o.m_mx.z && m_mx.z > o.m_mn.z);
        }
        EL_FORCE_INLINE bool contains ( const Vector3& p ) const
        {
            return (p.x > m_mn.x && p.x < m_mx.x &&
                    p.y > m_mn.y && p.y < m_mx.y &&
                    p.z > m_mn.z && p.z < m_mx.z);
        }
    };
    
    //------------------------------------------------------------------------
} // namespace EL

#endif // __ELAABB_HPP
