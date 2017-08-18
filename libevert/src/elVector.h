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
 

#ifndef __ELVECTOR_HPP
#define __ELVECTOR_HPP

#if !defined (__ELDEFS_HPP)
    #include "elDefs.h"
#endif

#include <cmath>

namespace EL
{

class Vector3;
class Vector4;
class Matrix3;
class Matrix3x4;

//------------------------------------------------------------------------

class Matrix3
{
    
public:
    
    EL_FORCE_INLINE	Matrix3 ();
    EL_FORCE_INLINE	Matrix3 (const Matrix3& m);
    EL_FORCE_INLINE	Matrix3 (float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
    EL_FORCE_INLINE	Matrix3 (float m);
    EL_FORCE_INLINE	~Matrix3 ();
    EL_FORCE_INLINE Matrix3& operator= (const Matrix3& m);
    EL_FORCE_INLINE Vector3& operator[] (int i);
    EL_FORCE_INLINE const Vector3& operator[] (int i) const;
    EL_FORCE_INLINE void set (float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22);
    EL_FORCE_INLINE const Vector3 getRow (int i) const;
    EL_FORCE_INLINE const Vector3 getColumn (int i) const;
    EL_FORCE_INLINE void setRow (int i, const Vector3& v);
    EL_FORCE_INLINE void setColumn (int i, const Vector3& v);
    EL_FORCE_INLINE void operator*= (const Matrix3& m);
    EL_FORCE_INLINE void operator*=	(float f);
    EL_FORCE_INLINE void identity (void);
    EL_FORCE_INLINE void transpose (void);
    EL_FORCE_INLINE float det (void) const;
    
    bool invert			(void);
    void rotate (float radians, const Vector3& aboutThis);
    const Vector3 toEuler (void) const;
    
    
private:
    
    float							matrix[3][3];
    
};

//------------------------------------------------------------------------

class Matrix3x4
{
    
public:
    
    EL_FORCE_INLINE Matrix3x4 (void);
    EL_FORCE_INLINE	Matrix3x4 (const Matrix3& m);
    EL_FORCE_INLINE	Matrix3x4 (const Matrix3x4& m);
    EL_FORCE_INLINE	Matrix3x4 (float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23);
    EL_FORCE_INLINE	~Matrix3x4 (void);
    EL_FORCE_INLINE Matrix3x4& operator= (const Matrix3x4& m);
    EL_FORCE_INLINE Vector4& operator[] (int i);
    EL_FORCE_INLINE const Vector4& operator[] (int i) const;
    EL_FORCE_INLINE void set (float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23);
    EL_FORCE_INLINE const Vector4 getRow (int i) const;
    EL_FORCE_INLINE const Vector3 getColumn (int i) const;
    EL_FORCE_INLINE void setRow (int i, const Vector4& v);
    EL_FORCE_INLINE void setColumn (int i, const Vector3& v);
    EL_FORCE_INLINE const Matrix3 getRotation (void) const;
    EL_FORCE_INLINE const Vector3 getTranslation (void) const;
    EL_FORCE_INLINE void setRotation (const Matrix3& m);
    EL_FORCE_INLINE void setTranslation (const Vector3& v);
    EL_FORCE_INLINE void operator*= (const Matrix3x4& m);
    EL_FORCE_INLINE void operator*=	(float f);
    EL_FORCE_INLINE void identity (void);
    bool invert (void);
    EL_FORCE_INLINE	float det (void) const;
    void rotate (float radians, const Vector3& aboutThis);
    EL_FORCE_INLINE void translate (const Vector3& v);
    
    
private:
    
    float							matrix[3][4];
};

//------------------------------------------------------------------------

class Vector3
{
public:
	float							x, y, z;		// uninitialized

	EL_FORCE_INLINE					Vector3			(void)							{}
	EL_FORCE_INLINE					Vector3			(const Vector3& v)				{ x = v.x; y = v.y; z = v.z; }
	EL_FORCE_INLINE					Vector3			(float f)	{ x = y = z = f; }
	EL_FORCE_INLINE					Vector3			(float fx, float fy, float fz)	{ x = fx; y = fy; z = fz; }
	EL_FORCE_INLINE					~Vector3		(void)							{}
	EL_FORCE_INLINE Vector3&		operator=		(const Vector3& v)				{ x = v.x; y = v.y; z = v.z; return *this; }
	EL_FORCE_INLINE float&			operator[]		(int i)							{ EL_ASSERT(i>=0&&i<3); return (&x)[i]; }
	EL_FORCE_INLINE const float&	operator[]		(int i) const					{ EL_ASSERT(i>=0&&i<3); return (&x)[i]; }
	EL_FORCE_INLINE void			set				(float fx, float fy, float fz)	{ x = fx; y = fy; z = fz; }
	EL_FORCE_INLINE void			operator*=		(float f)						{ x *= f; y *= f; z *= f; }
	EL_FORCE_INLINE void			operator*=		(const Matrix3& m)				{ float tx = x,ty = y,tz = z; x = tx*m[0][0]+ty*m[1][0]+tz*m[2][0]; y = tx*m[0][1]+ty*m[1][1]+tz*m[2][1]; z = tx*m[0][2]+ty*m[1][2]+tz*m[2][2]; }
	EL_FORCE_INLINE void			operator+=		(const Vector3& v)				{ x += v.x; y += v.y; z += v.z; }
	EL_FORCE_INLINE void			operator-=		(const Vector3& v)				{ x -= v.x; y -= v.y; z -= v.z; }
	EL_FORCE_INLINE const Vector3	operator-		(void) const					{ return Vector3(-x,-y,-z); }
	EL_FORCE_INLINE bool			normalize		(void)							{ float l = x*x+y*y+z*z; if( l == float(0) ) return false; l = 1.f/sqrtf(l); x *= l; y *= l; z *= l; return true; }
	EL_FORCE_INLINE float			length			(void) const					{ return sqrtf(x*x+y*y+z*z); }
	EL_FORCE_INLINE float			lengthSqr		(void) const					{ return x*x+y*y+z*z; }
	EL_FORCE_INLINE void			scale			(const Vector3& v)				{ x *= v.x; y *= v.y; z *= v.z; }
};


//------------------------------------------------------------------------

class Vector4
{
public:
	float							x, y, z, w;		// uninitialized

	EL_FORCE_INLINE					Vector4			(void)							{}
	EL_FORCE_INLINE					Vector4			(const Vector4& v)				{ x = v.x; y = v.y; z = v.z; w = v.w; }
	EL_FORCE_INLINE					Vector4			(float fx, float fy, float fz, float fw) { x = fx; y = fy; z = fz; w = fw; }
	EL_FORCE_INLINE					~Vector4		(void)							{}
	EL_FORCE_INLINE Vector4&		operator=		(const Vector4& v)				{ x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
	EL_FORCE_INLINE float&			operator[]		(int i)							{ EL_ASSERT(i>=0&&i<4); return (&x)[i]; }
	EL_FORCE_INLINE const float&	operator[]		(int i) const					{ EL_ASSERT(i>=0&&i<4); return (&x)[i]; }
	EL_FORCE_INLINE void			set				(float fx, float fy, float fz, float fw) { x = fx; y = fy; z = fz; w = fw; }
	EL_FORCE_INLINE void			operator*=		(float f)						{ x *= f; y *= f; z *= f; w *= f; }
	EL_FORCE_INLINE void			operator+=		(const Vector4& v)				{ x += v.x; y += v.y; z += v.z; w += v.w; }
	EL_FORCE_INLINE void			operator-=		(const Vector4& v)				{ x -= v.x; y -= v.y; z -= v.z; w -= v.w; }
	EL_FORCE_INLINE const Vector4	operator-		(void) const					{ return Vector4(-x,-y,-z,-w); }
	EL_FORCE_INLINE bool			normalize		(void)							{ float len = sqrtf(x*x+y*y+z*z); if (len==0.f) return false; len = 1.f/len; x *= len; y *= len; z *= len; w *= len; return true; }
	EL_FORCE_INLINE bool			normalizeByW	(void)							{ if (w==0.f) return false; float iw = 1.f/w; x *= iw; y *= iw; z *= iw; w = 1.f; return true; }
	EL_FORCE_INLINE void			scale			(const Vector4& v)				{ x *= v.x; y *= v.y; z *= v.z; w *= v.w; }
};

//------------------------------------------------------------------------

EL_FORCE_INLINE bool			operator==	(const Matrix3& m1, const Matrix3& m2)	{ for(int i=0;i<3;i++) for(int j=0;j<3;j++) if(m1[i][j] != m2[i][j]) return false; return true; }
EL_FORCE_INLINE bool			operator!=	(const Matrix3& m1, const Matrix3& m2)	{ return !(m1 == m2); }
EL_FORCE_INLINE const Matrix3	operator*	(const Matrix3& m1, const Matrix3& m2)	{ Matrix3 t; for(int i=0;i<3;i++) for(int j=0;j<3;j++) t[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j]; return t; }
EL_FORCE_INLINE const Matrix3	operator*	(float f, const Matrix3& m)				{ Matrix3 t(m); t *= f; return t; }
EL_FORCE_INLINE const Matrix3	operator*	(const Matrix3& m, float f)				{ Matrix3 t(m); t *= f; return t; }
EL_FORCE_INLINE const Matrix3	transpose	(const Matrix3& m)						{ Matrix3 t(m); t.transpose(); return t; }
EL_FORCE_INLINE const Matrix3	invert		(const Matrix3& m)						{ Matrix3 t(m); t.invert(); return t; }

EL_FORCE_INLINE					Matrix3::Matrix3	(void)							{ identity(); }
EL_FORCE_INLINE					Matrix3::Matrix3	(const Matrix3& m)				{ *this = m; }
EL_FORCE_INLINE					Matrix3::Matrix3	(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)	{ set(m00,m01,m02,m10,m11,m12,m20,m21,m22); }
EL_FORCE_INLINE					Matrix3::Matrix3	(float m)	{ set(m,m,m,m,m,m,m,m,m); }
EL_FORCE_INLINE					Matrix3::~Matrix3	(void)							{}
EL_FORCE_INLINE Matrix3&		Matrix3::operator=	(const Matrix3& m)				{ for(int i=0;i<3;i++) for(int j=0;j<3;j++) matrix[i][j] = m.matrix[i][j]; return *this; }
EL_FORCE_INLINE Vector3&		Matrix3::operator[]	(int i)							{ EL_ASSERT(i>=0&&i<3); return (Vector3&)matrix[i][0]; }
EL_FORCE_INLINE const Vector3&	Matrix3::operator[]	(int i) const					{ EL_ASSERT(i>=0&&i<3); return (const Vector3&)matrix[i][0]; }
EL_FORCE_INLINE void			Matrix3::set		(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) { 	matrix[0][0] = m00; matrix[0][1] = m01; matrix[0][2] = m02; matrix[1][0] = m10; matrix[1][1] = m11; matrix[1][2] = m12; matrix[2][0] = m20; matrix[2][1] = m21; matrix[2][2] = m22; }
EL_FORCE_INLINE const Vector3	Matrix3::getRow		(int i) const					{ EL_ASSERT(i>=0&&i<3); return Vector3(matrix[i][0],matrix[i][1],matrix[i][2]); }
EL_FORCE_INLINE const Vector3	Matrix3::getColumn	(int i) const					{ EL_ASSERT(i>=0&&i<3); return Vector3(matrix[0][i],matrix[1][i],matrix[2][i]); }
EL_FORCE_INLINE void			Matrix3::setRow		(int i, const Vector3& v)		{ EL_ASSERT(i>=0&&i<3); matrix[i][0] = v.x; matrix[i][1] = v.y; matrix[i][2] = v.z; }
EL_FORCE_INLINE void			Matrix3::setColumn	(int i, const Vector3& v)		{ EL_ASSERT(i>=0&&i<3); matrix[0][i] = v.x; matrix[1][i] = v.y; matrix[2][i] = v.z; }
EL_FORCE_INLINE void			Matrix3::operator*=	(const Matrix3& m)				{ *this = *this * m; }
EL_FORCE_INLINE void			Matrix3::operator*=	(float f)						{ for(int i=0;i<3;i++) for(int j=0;j<3;j++) matrix[i][j] *= f; }
EL_FORCE_INLINE void			Matrix3::identity	(void)							{ for(int i=0;i<3;i++) for(int j=0;j<3;j++) matrix[i][j] = (i == j) ? float(1) : float(0); }
EL_FORCE_INLINE void			Matrix3::transpose	(void)							{ swap(matrix[1][0], matrix[0][1]); swap(matrix[2][0], matrix[0][2]); swap(matrix[2][1], matrix[1][2]); }
EL_FORCE_INLINE float			Matrix3::det		(void) const					{ return matrix[0][0] * (matrix[1][1]*matrix[2][2] - matrix[2][1]*matrix[1][2]) + matrix[0][1] * (matrix[2][0]*matrix[1][2] - matrix[1][0]*matrix[2][2]) + matrix[0][2] * (matrix[1][0]*matrix[2][1] - matrix[2][0]*matrix[1][1]); }
    
//------------------------------------------------------------------------

EL_FORCE_INLINE bool			operator==	(const Matrix3x4& m1, const Matrix3x4& m2)	{ for(int i=0;i<3;i++) for(int j=0;j<4;j++) if(m1[i][j] != m2[i][j]) return false; return true; }
EL_FORCE_INLINE bool			operator!=	(const Matrix3x4& m1, const Matrix3x4& m2)	{ return !(m1 == m2); }
				const Matrix3x4	operator*	(const Matrix3x4& m1, const Matrix3x4& m2);
EL_FORCE_INLINE const Matrix3x4	operator*	(float f, const Matrix3x4& m)				{ Matrix3x4 t(m); t *= f; return t; }
EL_FORCE_INLINE const Matrix3x4	operator*	(const Matrix3x4& m, float f)				{ Matrix3x4 t(m); t *= f; return t; }
EL_FORCE_INLINE const Matrix3x4	invert		(const Matrix3x4& m)						{ Matrix3x4 t(m); t.invert(); return t; }

EL_FORCE_INLINE					Matrix3x4::Matrix3x4		(void)						{ identity(); }
EL_FORCE_INLINE					Matrix3x4::Matrix3x4		(const Matrix3& m)			{ set(m[0][0],m[0][1],m[0][2],float(0),m[1][0],m[1][1],m[1][2],float(0),m[2][0],m[2][1],m[2][2],float(0)); }
EL_FORCE_INLINE					Matrix3x4::Matrix3x4		(const Matrix3x4& m)		{ *this = m; }
EL_FORCE_INLINE					Matrix3x4::Matrix3x4		(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23)	{ set(m00,m01,m02,m03,m10,m11,m12,m13,m20,m21,m22,m23); }
EL_FORCE_INLINE					Matrix3x4::~Matrix3x4		(void)						{}
EL_FORCE_INLINE Matrix3x4&		Matrix3x4::operator=		(const Matrix3x4& m)		{ for(int i=0;i<3;i++) for(int j=0;j<4;j++) matrix[i][j] = m.matrix[i][j]; return *this; }
EL_FORCE_INLINE Vector4&		Matrix3x4::operator[]		(int i)						{ EL_ASSERT(i>=0&&i<3); return (Vector4&)matrix[i][0]; }
EL_FORCE_INLINE const Vector4&	Matrix3x4::operator[]		(int i) const				{ EL_ASSERT(i>=0&&i<3); return (const Vector4&)matrix[i][0]; }
EL_FORCE_INLINE void			Matrix3x4::set				(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23) { matrix[0][0] = m00; matrix[0][1] = m01; matrix[0][2] = m02; matrix[0][3] = m03; matrix[1][0] = m10; matrix[1][1] = m11; matrix[1][2] = m12;  matrix[1][3] = m13; matrix[2][0] = m20; matrix[2][1] = m21; matrix[2][2] = m22; matrix[2][3] = m23; }
EL_FORCE_INLINE const Vector4	Matrix3x4::getRow			(int i) const				{ EL_ASSERT(i>=0&&i<3); return Vector4(matrix[i][0],matrix[i][1],matrix[i][2],matrix[i][3]); }
EL_FORCE_INLINE const Vector3	Matrix3x4::getColumn		(int i) const				{ EL_ASSERT(i>=0&&i<4); return Vector3(matrix[0][i],matrix[1][i],matrix[2][i]); }
EL_FORCE_INLINE void			Matrix3x4::setRow			(int i, const Vector4& v)	{ EL_ASSERT(i>=0&&i<3); matrix[i][0] = v.x; matrix[i][1] = v.y; matrix[i][2] = v.z; matrix[i][3] = v.w; }
EL_FORCE_INLINE void			Matrix3x4::setColumn		(int i, const Vector3& v)	{ EL_ASSERT(i>=0&&i<4); matrix[0][i] = v.x; matrix[1][i] = v.y; matrix[2][i] = v.z; }
EL_FORCE_INLINE const Matrix3	Matrix3x4::getRotation		(void) const				{ return Matrix3(matrix[0][0],matrix[0][1],matrix[0][2],matrix[1][0],matrix[1][1],matrix[1][2],matrix[2][0],matrix[2][1],matrix[2][2]); }
EL_FORCE_INLINE const Vector3	Matrix3x4::getTranslation	(void) const				{ return Vector3(matrix[0][3],matrix[1][3],matrix[2][3]); }
EL_FORCE_INLINE void			Matrix3x4::setRotation		(const Matrix3& m)			{ matrix[0][0] = m[0][0]; matrix[0][1] = m[0][1]; matrix[0][2] = m[0][2]; matrix[1][0] = m[1][0]; matrix[1][1] = m[1][1]; matrix[1][2] = m[1][2]; matrix[2][0] = m[2][0]; matrix[2][1] = m[2][1]; matrix[2][2] = m[2][2]; }
EL_FORCE_INLINE void			Matrix3x4::setTranslation	(const Vector3& v)			{ matrix[0][3] = v[0]; matrix[1][3] = v[1]; matrix[2][3] = v[2]; }
EL_FORCE_INLINE void			Matrix3x4::operator*=		(const Matrix3x4& m)		{ *this = (*this * m); }
EL_FORCE_INLINE void			Matrix3x4::operator*=		(float f)					{ for(int i=0;i<3;i++) for(int j=0;j<4;j++) matrix[i][j] *= f; }
EL_FORCE_INLINE void			Matrix3x4::identity			(void)						{ for(int i=0;i<3;i++) for(int j=0;j<4;j++) matrix[i][j] = (i == j) ? float(1) : float(0); }
EL_FORCE_INLINE float			Matrix3x4::det				(void) const				{ return getRotation().det(); }
EL_FORCE_INLINE void			Matrix3x4::translate		(const Vector3& v)			{ matrix[0][3] += v.x; matrix[1][3] += v.y; matrix[2][3] += v.z; }

//------------------------------------------------------------------------

EL_FORCE_INLINE bool			operator==	(const Vector3& v1, const Vector3& v2)	{ return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z); }
EL_FORCE_INLINE bool			operator!=	(const Vector3& v1, const Vector3& v2)	{ return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z); }
EL_FORCE_INLINE const Vector3	operator*	(float f, const Vector3& v)				{ return Vector3(v.x*f,v.y*f,v.z*f); }
EL_FORCE_INLINE const Vector3	operator*	(const Vector3& v, float f)				{ return Vector3(v.x*f,v.y*f,v.z*f); }
EL_FORCE_INLINE const Vector3	operator+	(const Vector3& v1, const Vector3& v2)	{ return Vector3(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z); }
EL_FORCE_INLINE const Vector3	operator-	(const Vector3& v1, const Vector3& v2)	{ return Vector3(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z); }
EL_FORCE_INLINE float			dot			(const Vector3& v1, const Vector3& v2)	{ return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z; }
EL_FORCE_INLINE const Vector3	cross		(const Vector3& v1, const Vector3& v2)	{ return Vector3( v1.y*v2.z-v1.z*v2.y, v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x ); }
EL_FORCE_INLINE const Vector3	normalize	(const Vector3& v)						{ float l = dot(v,v); if( l != float(0) ) l = 1.f/sqrtf(l); return v * l; }
EL_FORCE_INLINE const Vector3	scale		(const Vector3& v1, const Vector3& v2)	{ return Vector3(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z); }
EL_FORCE_INLINE const Vector3	operator*	(const Vector3& v, const Matrix3& m)	{ return Vector3( v.x*m[0][0]+v.y*m[1][0]+v.z*m[2][0], v.x*m[0][1]+v.y*m[1][1]+v.z*m[2][1], v.x*m[0][2]+v.y*m[1][2]+v.z*m[2][2] ); }
EL_FORCE_INLINE const Vector3	operator*	(const Matrix3& m, const Vector3& v)	{ return Vector3( v.x*m[0][0]+v.y*m[0][1]+v.z*m[0][2], v.x*m[1][0]+v.y*m[1][1]+v.z*m[1][2], v.x*m[2][0]+v.y*m[2][1]+v.z*m[2][2] ); }
EL_FORCE_INLINE bool			operator<	(const Vector3& a, const Vector3& b)	{ if (a.x < b.x) return true; if (a.x > b.x) return false; if (a.y < b.y) return true; if (a.y > b.y) return false; return a.z < b.z; }

//------------------------------------------------------------------------

EL_FORCE_INLINE bool			operator==	(const Vector4& v1, const Vector4& v2)	{ return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w); }
EL_FORCE_INLINE bool			operator!=	(const Vector4& v1, const Vector4& v2)	{ return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z) || (v1.w != v2.w); }
EL_FORCE_INLINE const Vector4	operator*	(float f, const Vector4& v)				{ return Vector4(v.x*f,v.y*f,v.z*f,v.w*f); }
EL_FORCE_INLINE const Vector4	operator*	(const Vector4& v, float f)				{ return Vector4(v.x*f,v.y*f,v.z*f,v.w*f); }
EL_FORCE_INLINE const Vector4	operator+	(const Vector4& v1, const Vector4& v2)	{ return Vector4(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z, v1.w+v2.w); }
EL_FORCE_INLINE const Vector4	operator-	(const Vector4& v1, const Vector4& v2)	{ return Vector4(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z, v1.w-v2.w); }
EL_FORCE_INLINE float			dot			(const Vector4& v1, const Vector4& v2)	{ return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z+v1.w*v2.w; }
EL_FORCE_INLINE const Vector4	normalizeByW(const Vector4& v)						{ if (v.w==0.f) return v; float iw = 1.f/v.w; return Vector4(v.x*iw, v.y*iw, v.z*iw, 1.f); }
EL_FORCE_INLINE const Vector3	divByW		(const Vector4& v)						{ if (v.w==0.f) return Vector3(0.f, 0.f, 0.f); float iw = 1.f/v.w; return Vector3(v.x*iw, v.y*iw, v.z*iw); }
EL_FORCE_INLINE const Vector4	scale		(const Vector4& v1, const Vector4& v2)	{ return Vector4(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z, v1.w*v2.w); }

//------------------------------------------------------------------------

EL_FORCE_INLINE float			dot			(const Vector3& v, const Vector4& p)	{ return v.x*p.x+v.y*p.y+v.z*p.z+p.w; }
EL_FORCE_INLINE const Vector3	operator*	(const Matrix3x4& m, const Vector3& v)	{ return Vector3( v.x*m[0][0]+v.y*m[0][1]+v.z*m[0][2]+m[0][3], v.x*m[1][0]+v.y*m[1][1]+v.z*m[1][2]+m[1][3], v.x*m[2][0]+v.y*m[2][1]+v.z*m[2][2]+m[2][3] ); }
EL_FORCE_INLINE Vector3			mirror		(const Vector3& v, const Vector4& p)	{ float d = 2.f*dot(v,p); return Vector3(v.x-d*p.x, v.y-d*p.y, v.z-d*p.z); }
EL_FORCE_INLINE Vector4			mirror		(const Vector4& p, const Vector4& r)	{ Vector3 pn(p.x, p.y, p.z); Vector3 rn(r.x, r.y, r.z); float dpr=2.f*dot(pn,rn); Vector3 pp(pn-dpr*rn); float pw = p.w-r.w*dpr; return Vector4(pp.x,pp.y,pp.z,pw); }
EL_FORCE_INLINE Vector4			normalize	(const Vector4& p)						{ float len = sqrtf(p.x*p.x+p.y*p.y+p.z*p.z); if (len==0.f) return p; len = 1.f/len; return Vector4(p.x*len, p.y*len, p.z*len, p.w*len); }
				Vector4			getPlaneEquation(const Vector3& a, const Vector3& b, const Vector3& c);

//------------------------------------------------------------------------
} // namespace EL

#endif // __ELVECTOR_HPP
