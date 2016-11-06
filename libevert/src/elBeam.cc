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
 * (C) 2008-2015 Markus Noisternig
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
