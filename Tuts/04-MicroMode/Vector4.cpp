/** @file Vector4.cpp
	Contains code for Matrix44 Class
	This file contains all the code for member functions of Matrix44 class, it does
	not contain operators that are included in Matrix44Operation.cpp.
	@author		Alkampfer
*/

#include <cmath>
#include <cassert>

#include "PS2Math.h"

using namespace PS2Math;

/** .
	Simple constructor from explicit float values. Last value
	is defaulted to 1.
*/
Vector4::Vector4(float x, float y, float z, float w) {

	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

/** .
	Simply divide x, y, z by w coordinate to move from
	omogeneuos coordinate to normal cartesian coordinate 
	system.
*/
Vector4 &Vector4::Omogenize() {

	x /= w;
	y /= w;
	z /= w;
	w = 1;

	return *this;
}

/** .
	Do normalization of the vector dividing all coordinate
	for the module of the vector itself.
*/
Vector4 &Vector4::Normalize() {

	float Module = w / sqrt(x*x + y*y + z*z);
	x *= Module;
	y *= Module;
	z *= Module;

	return *this;
}

/** .
	Used in conjunction with projection matrix. Some type of projection
	matrix are W-friendly, this means that after transformation Z coordinate
	of the vertices are stored in w coordinate of projected vertices.
	This function is used to bring vector back to normal 3D coordinate
	system after a projection.
*/
Vector4 &Vector4::OmogenizeWtoZ() {
	
	x /= w;
	y /= w;
	z = w;
	w = 1;

	return *this;
}

/** .
	Operator [] is normally used to access the vector4 structure like
	an array.
*/
float &Vector4::operator[](int Index) {

	switch(Index) {
	
		case 0:
		return x;
		case 1:
		return y;
		case 2:
		return z;
		case 3:
		return w;
	}
	
	assert(false);
	return x;
}

/** .
*/
const float Vector4::operator[](int Index) const {

	switch(Index) {
	
		case 0:
		return x;
		case 1:
		return y;
		case 2:
		return z;
		case 3:
		return w;
	}

	assert(false);
	return x;
}

/** .
	Debugging routine that permit to output in a stream the content of a 
	Vector44 object.
*/
std::ostream &PS2Math::operator<<(std::ostream &outStream, const Vector4 &V) {

	outStream << "[" << V[0] << "\t" << V[1] << "\t" << 
							  V[2] << "\t" << V[3] << "]\n";
	
	return outStream;
}

/** .
	Unary minus operator is useful to change sign of a vector
*/
const Vector4 PS2Math::operator-(const Vector4 &V) {

	return Vector4(-V.x, -V.y, -V.z, V.w);
}

/** .
*/
const Vector4 PS2Math::operator*(const Vector4 &V1, const Vector4 &V2) {
 
	return Vector4(V1.y * V2.z - V1.z * V2.y, 
						V1.z * V2.x - V1.x * V2.z, 
						V1.x * V2.y - V1.y * V2.x, 
						V1.w * V2.w);
}

/** .
*/
const Vector4 PS2Math::operator-(const Vector4 &V1, const Vector4 &V2) {

	return V1 + -V2;
}

/** .
*/
const Vector4 PS2Math::operator+(const Vector4 &V1, const Vector4 &V2) {

	return Vector4(V2.w * V1.x + V1.w * V2.x, 
						V2.w * V1.y + V1.w * V2.y,
						V2.w * V1.z + V1.w * V2.z,
						V2.w * V1.w);
}

/** .
*/
float PS2Math::Dot(const Vector4 &V1, const Vector4 &V2) {

	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z) / (V1.w * V2.w);
}

/** .
	This routine is peculiar to PS2 and it's used to convert coordinate
	values from normal floating point representation to fixed point
	representation used by front buffer of ps2. Vector is deomogenized 
	during conversion since this operation is done only before sending
	data to front buffer. If texture Perspective correction is used
	(S,T,Q) remember that this routine is to be called immediatly after
	projection matrix is applied, this because perspective correction 
	algorithm needs to know W value of the vector after projection. 
	@param Vertex Array of 4 integer that will contain at exit converted
			 value of the vertex. 
   @returns Inverse of W coordinate, this is necessary to do perspective
				correct texturing in witch GS interpolate U/W, V/W and 1/W. 
*/
float	 PS2Math::Vector4::ConvertToFixedPoint(int Vertex[]) {

  float divw;
  
  divw = 1.0f / w;
  Vertex[0] = (int) (x * divw) << 4;
  Vertex[1] = (int) (y * divw) << 4;
  Vertex[2] = (int) (z * divw);
  Vertex[3] = 1;
  
  return divw;
}