#ifndef __ELBEAM_HPP
#define __ELBEAM_HPP
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

#if !defined (__ELPOLYGON_HPP)
    #include "elPolygon.h"
#endif
#if !defined (__ELVECTOR_HPP)
    #include "elVector.h"
#endif

namespace EL
{

class Beam
{
    
public:
    
    Beam (void);
    Beam (const Vector3& top, const Polygon& polygon);
    Beam (const Beam& beam);
    ~Beam (void);
    
    const Beam&	operator= (const Beam& beam);
    
    const Vector3& getTop (void) const { return m_top; }
    const Polygon& getPolygon (void) const { return m_polygon; }
    
    int	numPleqs (void) const
    {
        return (int)m_pleqs.size();
    }
    
    const Vector4& getPleq (int i) const
    {
        EL_ASSERT(i >= 0 && i < numPleqs());
        return m_pleqs[i];
    }
    
    //void render (const Vector3& color) const;
    EL_FORCE_INLINE bool contains (const Vector3& p) const
    {
        for (int i=0; i < numPleqs(); i++)
        {
            if (dot(p, getPleq(i)) < 0.f)
            {
                return false;
            }
        }
        return true;
    }
    
    
private:
    
    void calculatePleqs	(void);
    
    Vector3 m_top;
    Polygon m_polygon;
    std::vector<Vector4> m_pleqs;
};
    
} // namespace EL

#endif // __ELBEAM_HPP
