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
 

#include "elBSP.h"
#include "elRoom.h"
#include <cstdio>

#ifdef __Darwin
    #include <OpenGL/glu.h>
    #include <OpenGL/gl.h>
#else
    #include <GL/glu.h>
    #include <GL/gl.h>
#endif

using namespace EL;


Room::Room(void): m_bsp(0) {}

Room::~Room(void)
{
    delete m_bsp;
}

bool Room::import(const char* filename, MaterialFile& materials)
{
    FILE* f = fopen(filename, "r");
    if( !f ){ return false; }
    
    float maxNonPlanarity = 0.f;
    while( !feof(f) )
    {
        int itemType = 0;
        fscanf(f, "%d", &itemType);
        
        if( itemType == 4 )
        {
            // source
            Source s;
            Vector3 pos;
            fscanf(f, "%*f %*f %*f %*f %*f %f %f %f", &pos.x, &pos.y, &pos.z);
            s.setPosition( pos*0.001 );
            s.setOrientation(Matrix3(0,0,1,1,0,0,0,1,0));
            
            char buf[32];
            sprintf(buf, "Src%d", m_sources.size()+1);
            
            s.setName(buf);
            m_sources.push_back(s);
        }
        
        if( itemType == 2 )
        {
            // listener
            Listener l;
            Vector3 pos;
            fscanf(f, "%*f %*f %*f %*f %*f %f %f %f", &pos.x, &pos.y, &pos.z);
            l.setPosition( pos*0.001 );
            l.setOrientation(Matrix3(0,0,-1,-1,0,0,0,1,0));
            
            char buf[32];
            sprintf(buf, "Lst%d", m_listeners.size()+1);
            
            l.setName(buf);
            m_listeners.push_back(l);
        }
        
        if( itemType != 1 )
        {
            // skip the rest of the line
            char line[1024];
            fgets(line, 1024, f);
            continue;
        }
        
        // read element color, skip unknown number
        Vector3 color;
        fscanf(f, "%*d %f %f %f", &color.x, &color.y, &color.z);
        
        // number of points
        int numPoints;
        fscanf(f, "%d", &numPoints);
        
        // read points
        std::vector<Vector3> points(numPoints);
        for( int j=0; j < numPoints; j++ )
        {
            fscanf(f, "%f %f %f", &points[j].x, &points[j].y, &points[j].z);
            points[j] = points[j] * 0.001;
        }
        
        // read material name
        char material_name[256];
        fscanf(f, "%s", &material_name);
        Material& material = materials.find(material_name);
        
        // create polygon and split into convex parts
        Polygon poly(points, material);
        maxNonPlanarity = max2(maxNonPlanarity, poly.getNonPlanarity());
        std::vector<Polygon> parts;
        poly.splitConvex(parts);
        
        // add original element
        Element elem;
        elem.m_color   = color;
        elem.m_polygon = poly;
        m_elements.push_back(elem);
        
        // add an element for each convex part
        for( int i=0; i < (int)parts.size(); i++ )
        {
            Element elem;
            elem.m_color   = color;
            elem.m_polygon = parts[i];
            m_convexElements.push_back(elem);
        }
    }
    fclose(f);
    
    printf("room '%s' imported\n", filename);
    printf("  original elements: %d\n", m_elements.size());
    printf("  convex elements:   %d\n", m_convexElements.size());
    printf("  max. planar error: %.2g mm\n", maxNonPlanarity);
    
    // construct BSP
    std::vector<const Polygon*> polygons;
    for( int i=0; i < numConvexElements(); i++ )
    {
        polygons.push_back(&getConvexElement(i).m_polygon);
    }
    
    m_bsp = new BSP();
    m_bsp->constructHierarchy(&polygons[0], polygons.size());
    
    return true;
}


bool Room::setElements(std::vector<Element> &elements)
{
    // Clear old data
    m_elements.clear ();
    m_convexElements.clear ();
    m_sources.clear ();
    m_listeners.clear ();
    if( m_bsp )
    {
        delete m_bsp;
        m_bsp = 0;
    }
    
    if( elements.size () == 0 ){ return true; }
    
    for( std::vector<Element>::iterator element = elements.begin(); element != elements.end(); element++ )
    {
        m_elements.push_back(*element);
        
        //    element->m_polygon.print();
        //    std::cout << std::endl;
        
        std::vector<Polygon> parts;
        element->m_polygon.splitConvex(parts);
        for( std::vector<Polygon>::iterator convexPoly = parts.begin(); convexPoly != parts.end(); convexPoly++ )
        {
            Element elem;
            elem.m_polygon = *convexPoly;
            elem.m_color = element->m_color;
            m_convexElements.push_back(elem);
        }
    }
    
    printf("  original elements: %d\n", m_elements.size());
    printf("  convex elements:   %d\n", m_convexElements.size());
    std::vector<const Polygon*> polygons;
    for( int i = 0; i < numConvexElements(); i++ )
    {
        polygons.push_back(&getConvexElement(i).m_polygon);
    }
    
    m_bsp = new BSP();
    m_bsp->constructHierarchy(&polygons[0], polygons.size());
    
    return true;
}

bool Room::addListener(Listener listener)
{
    m_listeners.push_back(listener);
    return true;
}

bool Room::addSource(Source source)
{
    m_sources.push_back(source);
    return true;
}

//------------------------------------------------------------------------

void Room::getBoundingBox(Vector3& mn, Vector3& mx) const
{
    if( numElements() == 0 )
    {
        mn.set(0.f, 0.f, 0.f);
        mx = mn;
        return;
    }
    
    mn = mx = getElement(0).m_polygon[0];
    
    for( int i=0; i < numElements(); i++ )
    {
        for( int j=0; j < m_elements[i].m_polygon.numPoints(); j++ )
        {
            const Vector3& p = getElement(i).m_polygon[j];
            for( int k=0; k < 3; k++ )
            {
                mn[k] = min2(mn[k], p[k]);
                mx[k] = max2(mx[k], p[k]);
            }
        }
    }
}

float Room::getMaxLength(void) const
{
    Vector3 mn, mx;
    getBoundingBox(mn, mx);
    
    mx -= mn;
    float len = mx.x;
    len = max2(len, mx.y);
    len = max2(len, mx.z);
    
    return len;
}

Vector3 Room::getCenter(void) const
{
    Vector3 mn, mx;
    getBoundingBox(mn, mx);
    return .5f*(mn+mx);
}

//------------------------------------------------------------------------

void Room::render(void) const
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);
    
    for( int i=0; i < numConvexElements(); i++ )
    {
        const Room::Element& e = getConvexElement(i);
        Vector3 color = e.m_color;
        
        float ldot = dot(e.m_polygon.getNormal(), normalize(Vector3(3,2,4)));
        ldot = .25f + .75f*fabsf(ldot);
        color *= ldot;
        glColor3fv(&color.x);
        
        /*
         glBegin(GL_POLYGON);
         for (int j=0; j < e.m_polygon.numPoints(); j++)
         glVertex3fv(&e.m_polygon[j].x);
         
         glEnd();
         */
    }
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    glLineWidth(1.f);
    for( int i=0; i < numElements(); i++ )
    {
        const Room::Element& e = getElement(i);
        glColor3f(1.f, 1.f, 1.f);
        Vector3 color = e.m_color;
        glColor3fv(&color.x);
        glBegin(GL_LINE_LOOP);
        for( int j=0; j < e.m_polygon.numPoints(); j++ )
        {
            glVertex3fv(&e.m_polygon[j].x);
        }
        glEnd();
    }
    glPopAttrib();
}

