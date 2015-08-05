#ifndef __ELDEFS_HPP
#define __ELDEFS_HPP
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

//------------------------------------------------------------------------
// constants
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// windows.h specialities -- don't define the Polygon struct
//------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define NOGDI

//------------------------------------------------------------------------
// load standard libraries and disable a couple of warnings in MSVC
//------------------------------------------------------------------------

#if defined(_MSC_VER)
#	pragma warning(disable: 4505)	// unreferenced local function has been removed
#	pragma warning(disable: 4514)	// unreferenced inline function has been removed
#	pragma warning(disable: 4710)	// function x not inlined
#	pragma warning(disable: 4714)	// function x marked as __forceinline not inlined
#	pragma warning(disable: 4786)	// identifier was truncated to n characters in the debug information
#	pragma warning(push, 1)
#endif

#include <map>
#include <set>
#include <string>
#include <vector>
#include <cstdlib>

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

//------------------------------------------------------------------------
// assertions
//------------------------------------------------------------------------

#if defined(EL_DEBUG)
#	include <cassert>
#	define EL_ASSERT(e)		assert(e)
#else
#	define EL_ASSERT(e)		(static_cast<void>(0))					
#endif

//------------------------------------------------------------------------
// debug code
//------------------------------------------------------------------------

#if defined(EL_DEBUG)
#	define EL_DEBUG_CODE(X) X
#else
#	define EL_DEBUG_CODE(X)
#endif

//------------------------------------------------------------------------
// our stuff
//------------------------------------------------------------------------

namespace EL
{

#define	PI						3.141592654f
template<class TT> inline TT	RAD( TT x ) { return x * TT(PI) / TT(180); }
template<class TT> inline TT	DEG( TT x )	{ return x * TT(180) / TT(PI); }

//------------------------------------------------------------------------
// Debug macros
//------------------------------------------------------------------------

#if defined(_DEBUG)
#	undef EL_DEBUG
#	define EL_DEBUG
#endif // _DEBUG

//------------------------------------------------------------------------
// The for() fix macro for MSVC (as Visual C++ does not respect the C++
// standard)
//------------------------------------------------------------------------

#if defined(_MSC_VER)
#	if !defined (for)
		__forceinline bool getFalse(void) { return false; }
#		define for if(::EL::getFalse()); else for
#	endif // for
#endif

//------------------------------------------------------------------------
// Forced inlines
//------------------------------------------------------------------------

#if defined (_MSC_VER)
#	define EL_FORCE_INLINE __forceinline
#else
#	define EL_FORCE_INLINE inline
#endif

//------------------------------------------------------------------------
// Commonly used swap,min2,max2 templates
//------------------------------------------------------------------------

template <class T> inline void swap(T& a, T& b)
{
	T t = a;
	a = b;
	b = t;
}

template <class T> inline const T& min2 (const T& a, const T& b)
{
	if(a <= b) return a;
	else return b;
}

template <class T> inline const T& max2 (const T& a, const T& b)
{
	if(a >= b) return a;
	else return b;
}

inline float frand(void)
{
	return (float)arc4random() / (float)RAND_MAX;
}

//------------------------------------------------------------------------
}	   // namespace EL

#endif // __ELDEFS_HPP
