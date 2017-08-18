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
 

#include "elVector.h"

using namespace EL;

//------------------------------------------------------------------------

bool Matrix3::invert(void)
{
    float det00 = matrix[1][1]*matrix[2][2] - matrix[2][1]*matrix[1][2];
    float det01 = matrix[2][0]*matrix[1][2] - matrix[1][0]*matrix[2][2];
    float det02 = matrix[1][0]*matrix[2][1] - matrix[2][0]*matrix[1][1];
    
    float det = matrix[0][0]*det00 + matrix[0][1]*det01 + matrix[0][2]*det02;
    
    if( det == 0.f ){ return false; }
    
    det = 1.f / det;
    
    Matrix3 t;
    t[0][0] = det * det00;
    t[1][0] = det * det01;
    t[2][0] = det * det02;
    t[0][1] = det * (matrix[2][1]*matrix[0][2] - matrix[0][1]*matrix[2][2]);
    t[1][1] = det * (matrix[0][0]*matrix[2][2] - matrix[2][0]*matrix[0][2]);
    t[2][1] = det * (matrix[2][0]*matrix[0][1] - matrix[0][0]*matrix[2][1]);
    t[0][2] = det * (matrix[0][1]*matrix[1][2] - matrix[1][1]*matrix[0][2]);
    t[1][2] = det * (matrix[1][0]*matrix[0][2] - matrix[0][0]*matrix[1][2]);
    t[2][2] = det * (matrix[0][0]*matrix[1][1] - matrix[1][0]*matrix[0][1]);
    *this = t;
    
    return true;
}

//------------------------------------------------------------------------

bool Matrix3x4::invert(void)
{
    Matrix3 rotation(getRotation());
    if( !rotation.invert() ){ return false; }
    
    Vector3 translation = rotation * getTranslation();
    
    set(rotation[0][0], rotation[0][1], rotation[0][2], -translation[0],
        rotation[1][0], rotation[1][1], rotation[1][2], -translation[1],
        rotation[2][0], rotation[2][1], rotation[2][2], -translation[2]);
    
    return true;
}

//------------------------------------------------------------------------

const Matrix3x4 EL::operator*(const Matrix3x4& m1, const Matrix3x4& m2)
{
    Matrix3 rotation = m1.getRotation() * m2.getRotation();
    Vector3 translation = m1.getRotation() * m2.getTranslation() + m1.getTranslation();
    
    return Matrix3x4(rotation[0][0], rotation[0][1], rotation[0][2], translation[0],
                     rotation[1][0], rotation[1][1], rotation[1][2], translation[1],
                     rotation[2][0], rotation[2][1], rotation[2][2], translation[2]);
}

//------------------------------------------------------------------------

void Matrix3::rotate(float radians, const Vector3& aboutThis)
{
    float c = cosf(radians);
    float s = -sinf(radians);
    float t = 1.f - c;
    Vector3 vn(aboutThis);
    if( !vn.normalize() ){ return; }
    
    Matrix3 m(t*vn.x*vn.x+c,	  t*vn.x*vn.y+s*vn.z, t*vn.x*vn.z-s*vn.y,
              t*vn.x*vn.y-s*vn.z, t*vn.y*vn.y+c,	  t*vn.y*vn.z+s*vn.x,
              t*vn.x*vn.z+s*vn.y, t*vn.y*vn.z-s*vn.x, t*vn.z*vn.z+c);
    
    *this *= m;
}

//------------------------------------------------------------------------

void Matrix3x4::rotate(float radians, const Vector3& aboutThis)
{
    float c = cosf(radians);
    float s = -sinf(radians);
    float t = 1.f - c;
    Vector3 vn( aboutThis );
    if( !vn.normalize( ) ){ return; }
    
    Matrix3x4 m(t*vn.x*vn.x+c,	    t*vn.x*vn.y+s*vn.z, t*vn.x*vn.z-s*vn.y, 0.f,
                t*vn.x*vn.y-s*vn.z, t*vn.y*vn.y+c,	    t*vn.y*vn.z+s*vn.x, 0.f,
                t*vn.x*vn.z+s*vn.y, t*vn.y*vn.z-s*vn.x, t*vn.z*vn.z+c,      0.f);
    
    *this *= m;
}

//------------------------------------------------------------------------

const Vector3 Matrix3::toEuler(void) const
{
    // return a the rotation component of the Matrix as a Vector3(rot_x,rot_y,rot_z) euler
    float rx = (float)atan2(matrix[2][1], matrix[2][2]);
    float ry = (float)atan2(-matrix[2][0], sqrt(pow(matrix[2][1],2) + pow(matrix[2][2],2)));
    float rz = (float)atan2(matrix[1][0], matrix[0][0]);
    Vector3 euler = Vector3(rx, ry, rz);
    return euler;
}

//------------------------------------------------------------------------

Vector4 EL::getPlaneEquation(const Vector3& a, const Vector3& b, const Vector3& c)
{
    float x1 = b.x - a.x;
    float y1 = b.y - a.y;
    float z1 = b.z - a.z;
    float x2 = c.x - a.x;
    float y2 = c.y - a.y;
    float z2 = c.z - a.z;
    float nx = (y1*z2)-(z1*y2);
    float ny = (z1*x2)-(x1*z2);
    float nz = (x1*y2)-(y1*x2);
    
    return Vector4(nx,ny,nz,-(a.x*nx+a.y*ny+a.z*nz));
}
