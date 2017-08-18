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
 

#ifndef __ELORIENTEDPOINT_HPP
#define __ELORIENTEDPOINT_HPP

#if !defined (__ELVECTOR_HPP)
#	include "elVector.h"
#endif

namespace EL
{

//------------------------------------------------------------------------
    
class OrientedPoint
{
    
public:
    
    OrientedPoint (void);
    OrientedPoint (const OrientedPoint& s);
    ~OrientedPoint (void);
    
    const OrientedPoint& operator= (const OrientedPoint& s);
    
    EL_FORCE_INLINE const Vector3& getPosition (void) const
    { return m_position; }
    
    EL_FORCE_INLINE const Matrix3& getOrientation (void) const
    { return m_orientation; }
    
    void setPosition (const Vector3& position)
    { m_position = position; }
    
    void setOrientation (const Matrix3& orientation)
    { m_orientation = orientation; }
    
    void translate (const Vector3& translation)
    { m_position += translation; }
    
    void rotate (const Matrix3& rotation) { m_orientation *= rotation; }
    
    const std::string& getName (void) const { return m_name; }
    void setName (std::string name) { m_name = name; }
    
    Vector3 getDirection (void) const { return m_orientation.getColumn(2); }
    Vector3 getUp (void) const { return m_orientation.getColumn(1); }
    Vector3 getRight (void) const { return m_orientation.getColumn(0); }
    
    //void							render			(const Vector3& color) const;
    
    
private:
    
    Vector3 m_position;
    Matrix3 m_orientation;
    std::string m_name;
    
};
    
    //------------------------------------------------------------------------
} // namespace EL

#endif // __ELORIENTEDPOINT_HPP
