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
 
 
#include "elBeam.h"

#include <iostream>

using namespace EL;

//------------------------------------------------------------------------

static const float EPS_EXPAND_BEAM = 1e-3f; 
// static const float EPS_EXPAND_BEAM = .0f; 

//------------------------------------------------------------------------

Beam::Beam(void) {}

Beam::Beam(const Vector3& top, const Polygon& polygon):
m_top( top ),
m_polygon( polygon )
{
    m_polygon.expand (EPS_EXPAND_BEAM);
    calculatePleqs();
}

Beam::Beam(const Beam& beam):
m_top (beam.m_top),
m_polygon(beam.m_polygon),
m_pleqs (beam.m_pleqs)
{
}

Beam::~Beam(void) {}

const Beam& Beam::operator=(const Beam& beam)
{
	m_top = beam.m_top;
	m_polygon = beam.m_polygon;
	m_pleqs = beam.m_pleqs;
	return *this;
}

//------------------------------------------------------------------------

void Beam::calculatePleqs(void)
{
    int n = m_polygon.numPoints();
    
    m_pleqs.resize(n + 1);
    Vector3 p1 = m_polygon[n-1];
    
    float sign = dot(m_top, m_polygon.getPleq()) > 0.f ? -1.f : 1.f;
    
    for( int i=0; i < n; i++ )
    {
        Vector3 p0 = p1;
        p1 = m_polygon[i];
        m_pleqs[i+1] = sign * normalize(getPlaneEquation(m_top, p0, p1));
    }
    m_pleqs[0] = sign * m_polygon.getPleq();
}

/*
void Beam::render(const Vector3& color) const
{
	static const float extrudeDist = 100000.f;	// 100 m

	if (m_polygon.numPoints() == 0)
		return;

	glColor3fv(&color.x);
	glBegin(GL_LINE_LOOP);
	for (int i=0; i < m_polygon.numPoints(); i++)
		glVertex3fv(&m_polygon[i].x);
	glEnd();

	glBegin(GL_LINES);
	for (int i=0; i < m_polygon.numPoints(); i++)
	{
		Vector3 p = m_polygon[i];
		glVertex3fv(&p.x);
		p += extrudeDist * normalize(p-m_top);
		glVertex3fv(&p.x);
	}
	glEnd();

	glBegin(GL_POLYGON);
	for (int i=0; i < m_polygon.numPoints(); i++)
		glVertex3fv(&m_polygon[i].x);
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (int i=0; i <= m_polygon.numPoints(); i++)
	{
		Vector3 p = m_polygon[i % m_polygon.numPoints()];
		Vector3 e = p + extrudeDist * normalize(p-m_top);
		glVertex3fv(&p.x);
		glVertex3fv(&e.x);
	}
	glEnd();
}
*/
