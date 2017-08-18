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


#ifndef __ELBSP_HPP
#define __ELBSP_HPP

#if !defined (__ELAABB_HPP)
    #include "elAABB.h"
#endif
#if !defined (__ELVECTOR_HPP)
    #include "elVector.h"
#endif

namespace EL
{
    
    //------------------------------------------------------------------------
    
    class Beam;
    class Polygon;
    class Ray;
    
    class BSP
    {
        
    public:
        
        BSP (void);
        ~BSP (void);
        
        void constructHierarchy (const Polygon** polygons, int numPolygons);
        
        void beamCast (const Beam& beam, std::vector<const Polygon*>& result) const;
        const Polygon* rayCast (const Ray& ray) const;
        bool rayCastAny (const Ray& ray) const;
        static Vector3 getIntersectionPoint (void);
        
        class TempNode;
        
        
    private:
        
        TempNode* m_hierarchy;
        uintptr_t* m_list;
        AABB m_aabb;
    };
    
    //------------------------------------------------------------------------
} // namespace EL

#endif // __ELBSP_HPP
