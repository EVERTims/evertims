#ifndef __ELORIENTEDPOINT_HPP
#define __ELORIENTEDPOINT_HPP
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

#if !defined (__ELVECTOR_HPP)
#	include "elVector.h"
#endif

namespace EL
{

//------------------------------------------------------------------------

class OrientedPoint
{
public:
									OrientedPoint	(void);
									OrientedPoint	(const OrientedPoint& s);
									~OrientedPoint	(void);

	const OrientedPoint&			operator=		(const OrientedPoint& s);

	EL_FORCE_INLINE const Vector3&	getPosition		(void) const					{ return m_position; }
	EL_FORCE_INLINE const Matrix3&	getOrientation	(void) const					{ return m_orientation; }
	void							setPosition		(const Vector3& position)		{ m_position = position; }
	void							setOrientation	(const Matrix3& orientation)	{ m_orientation = orientation; }
	void							translate		(const Vector3& translation)	{ m_position += translation; }
	void							rotate			(const Matrix3& rotation)		{ m_orientation *= rotation; }

	const std::string&				getName			(void) const					{ return m_name; }
	void							setName			(std::string name)				{ m_name = name; }

	Vector3							getDirection	(void) const					{ return m_orientation.getColumn(2); }
	Vector3							getUp			(void) const					{ return m_orientation.getColumn(1); }
	Vector3							getRight		(void) const					{ return m_orientation.getColumn(0); }

	//void							render			(const Vector3& color) const;

private:
	Vector3							m_position;
	Matrix3							m_orientation;
	std::string						m_name;
};

//------------------------------------------------------------------------
} // namespace EL

#endif // __ELORIENTEDPOINT_HPP
