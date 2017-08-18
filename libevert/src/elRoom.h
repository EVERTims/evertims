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
 

#ifndef __ELROOM_HPP
#define __ELROOM_HPP

#if !defined (__ELLISTENER_HPP)
#	include "elListener.h"
#endif
#if !defined (__ELSOURCE_HPP)
#	include "elSource.h"
#endif
#if !defined (__ELPOLYGON_HPP)
#	include "elPolygon.h"
#endif
#if !defined (__ELVECTOR_HPP)
#	include "elVector.h"
#endif

#include "material.h"

namespace EL
{
    
class BSP;

class Room
{
    
public:
    
    struct Element
    {
        Polygon	m_polygon;
        Vector3	m_color;
    };
    
    Room (void);
    ~Room (void);
    
    bool import (const char* filename, MaterialFile& materials);
    bool exportStuff (const char* filename) const;
    
    bool setElements (std::vector<Element> &elements);
    bool addListener (Listener listener);
    bool addSource (Source source);
    
    int numElements (void) const { return m_elements.size(); }
    const Element& getElement (int i) const
    {
        EL_ASSERT(i >= 0 && i < numElements()); return m_elements[i];
    }
    
    Element& getElement (int i)
    {
        EL_ASSERT(i >= 0 && i < numElements()); return m_elements[i];
    }
    
    const std::vector<Element>&	getElements (void) const { return m_elements; }
    
    int	numConvexElements (void) const { return m_convexElements.size(); }
    
    const Element& getConvexElement (int i) const
    {
        EL_ASSERT(i >= 0 && i < numConvexElements()); return m_convexElements[i];
    }
    
    Element& getConvexElement (int i)
    {
        EL_ASSERT(i >= 0 && i < numConvexElements()); return m_convexElements[i];
    }
    
    int numSources (void) const { return m_sources.size(); }
    const Source& getSource (int i) const
    {
        EL_ASSERT(i >= 0 && i < numSources()); return m_sources[i];
    }
    
    Source& getSource (int i) { EL_ASSERT(i >= 0 && i < numSources()); return m_sources[i]; }
    
    int numListeners (void) const { return m_listeners.size(); }
    const Listener&	getListener (int i) const
    {
        EL_ASSERT(i >= 0 && i < numListeners()); return m_listeners[i];
    }
    Listener& getListener (int i) { EL_ASSERT(i >= 0 && i < numListeners()); return m_listeners[i]; }
    
    void getBoundingBox (Vector3& mn, Vector3& mx) const;
    float getMaxLength (void) const;
    Vector3 getCenter (void) const;
    
    const BSP& getBSP (void) const { return *m_bsp; }
    void render (void) const;
    
    
private:
    
    std::vector<Element> m_elements;
    std::vector<Element> m_convexElements;
    std::vector<Source> m_sources;
    std::vector<Listener> m_listeners;
    BSP* m_bsp;
};

} // namespace EL

#endif // __ELROOM_HPP
