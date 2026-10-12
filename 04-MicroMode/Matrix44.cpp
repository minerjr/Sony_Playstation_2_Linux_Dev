/** @file Matrix44.cpp
	Contains code for Matrix44 Class
	This file contains all the code for member functions of Matrix44 class, it does
	not contain operators that are included in Matrix44Operation.cpp.
	@author		Alkampfer
*/
#include <iomanip>
#include <cmath>

#include"PS2Math.h"

using namespace PS2Math;

/** .
	Build a Matrix44 Object, an optional bool parameter is used to requests
	an identity matrix. If you want to set a View or Projection matrix you
	do not need to have an identity matrix so some time can be saved because
	elements of the matrix are not to be initialized.
	@param IdentityMatrix Bool parameters used to ask the creation of 
								 Identity Matrix, if the parameter is false or if
								 it's omitted, than the element of the matrix will have
								 random values after the creation.
*/
Matrix44::Matrix44(bool IdentityMatrix) {

	if (IdentityMatrix) {
	
		memset(elem, 0, sizeof(elem));
		elem[0][0] = 1.0f;
		elem[1][1] = 1.0f;
		elem[2][2] = 1.0f;
		elem[3][3] = 1.0f;
	}
}

/** .
*/
Matrix44 &Matrix44::Transpose() {

	asm __volatile__(" 

		lq			  $8,		0(%0)
		lq			  $9,		16(%0)
		lq			  $10,	32(%0)
		lq			  $11,	48(%0)

		#First part of the swizzle (consult tutorial)
		pextlw     $12,	$9,		$8
		pextuw     $13,	$9,		$8
		pextlw     $14,	$11,		$10
		pextuw     $15,	$11,		$10

		#Second part of the swizzle (consult tutorial)
		pcpyld     $8,		$14,		$12
		pcpyud     $9,		$12,		$14
		pcpyld     $10,	$15,		$13
		pcpyud     $11,	$13,		$15

		sq			  $8,		0(%0)
		sq			  $9,		16(%0)
		sq			  $10,	32(%0)
		sq			  $11,	48(%0)

	": : "r" (this));
	
	return *this;
}
/** .
	 Sometimes there is the need to obtain immediatly a matrix that represent
	 a rotation transform, this constructor is used to build directly a rotation
	 matrix about one of the main three axixs.
	 @param Angle Angle of desidered rotation expressed in radiant.
	 @param Axis  Member of MainAxis enum, does specify the axix of rotation.
*/
Matrix44::Matrix44(float Angle, MainAxis Axis) {

	switch (Axis) {
	
		case X_AXIS:
			SetRotationX(Angle);
		break;
		case Y_AXIS:
			SetRotationY(Angle);
		break;
		case Z_AXIS:
			SetRotationZ(Angle);
		break;
		default: 
		break;
	}
}

/** .
	 If this member function is called then the matrix become a Rotation 
	 matrix about X axis, all preceeding content is deleted, and there
	 is no prerequisites about values contained before this function is
	 called, because element of the matrix are resetted as the first operation.
	 @param Angle Angle of desidered rotation expressed in radiant.
	 @returns A reference to the same matrix object, this is usually done 
				 to permit chain operation such as MyMatrix.SetRotationX(...).AddRotationY(...)
*/
Matrix44 &Matrix44::SetRotationX(float Angle) {

	memset(elem, 0, sizeof(elem));
	elem[0][0] = elem[3][3] = 1;
	elem[2][2] = elem[1][1] = cos(Angle);
	elem[2][1] = sin(Angle);
	elem[1][2] = -elem[2][1]; 
	return *this;
}

/** .
	 If this member function is called then the matrix become a Rotation 
	 matrix about Y axis, all preceeding content is deleted, and there
	 is no prerequisites about values contained before this function is
	 called, because element of the matrix are resetted as the first operation.
	 @param Angle Angle of desidered rotation expressed in radiant.
	 @returns A reference to the same matrix object, this is usually done 
				 to permit chain operation such as MyMatrix.SetRotationY(...).AddRotationX(...)
*/
Matrix44 &Matrix44::SetRotationY(float Angle) {

	memset(elem, 0, sizeof(elem));
	elem[1][1] = elem[3][3] = 1;
	elem[0][0] = elem[2][2] = cos(Angle);
	elem[0][2] = sin(Angle);
	elem[2][0] = -elem[0][2]; 
	return *this;
}

/** .
	 If this member function is called then the matrix become a Rotation 
	 matrix about Z axis, all preceeding content is deleted, and there
	 is no prerequisites about values contained before this function is
	 called, because element of the matrix are resetted as the first operation.
	 @param Angle Angle of desidered rotation expressed in radiant.
	 @returns A reference to the same matrix object, this is usually done 
				 to permit chain operation such as MyMatrix.SetRotationZ(...).AddRotationX(...)
*/
Matrix44 &Matrix44::SetRotationZ(float Angle) {

	memset(elem, 0, sizeof(elem));
	elem[2][2] = elem[3][3] = 1;
	elem[0][0] = elem[1][1] = cos(Angle);
	elem[1][0] = sin(Angle);
	elem[0][1] = -elem[1][0]; 
	return *this;
}

/** .
	 This member function is used to add a rotation about X axis to current
	 transform expressed by the matrix. Code is simple, a temporary Matrix44
	 representing desidered rotation is created and then it is composed with
	 current matrix. Remember that since column vector notation is used, to 
	 add a transformation to an existing matrix, it has to be premultiplicated
	 by new transform. For this reason the Matrix44MultiplyR is used.
	 @param Angle The angle of rotation about X axis to add to current transform
	 @return A reference to the new matrix.
*/
Matrix44 &Matrix44::AddRotationX(float Angle) {

	Matrix44 Rot(Angle, X_AXIS);
	Matrix44MultiplyR(Rot, *this);
	return *this;
}

/** .
	 This member function is used to add a rotation about Y axis to current
	 transform expressed by the matrix. Code is simple, a temporary Matrix44
	 representing desidered rotation is created and then it is composed with
	 current matrix. Remember that since column vector notation is used, to 
	 add a transformation to an existing matrix, it has to be premultiplicated
	 by new transform. For this reason the Matrix44MultiplyR is used.
	 @param Angle The angle of rotation about Y axis to add to current transform
	 @return A reference to the new matrix.
*/
Matrix44 &Matrix44::AddRotationY(float Angle) {

	Matrix44 Rot(Angle, Y_AXIS);
	Matrix44MultiplyR(Rot, *this);
	return *this;
}

/** .
	 This member function is used to add a rotation about Z axis to current
	 transform expressed by the matrix. Code is simple, a temporary Matrix44
	 representing desidered rotation is created and then it is composed with
	 current matrix. Remember that since column vector notation is used, to 
	 add a transformation to an existing matrix, it has to be premultiplicated
	 by new transform. For this reason the Matrix44MultiplyR is used.
	 @param Angle The angle of rotation about Z axis to add to current transform
	 @return A reference to the new matrix.
*/
Matrix44 &Matrix44::AddRotationZ(float Angle) {

	Matrix44 Rot(Angle, Z_AXIS);
	Matrix44MultiplyR(Rot, *this);
	return *this;
}

/** .
	 This member function is used to add a rotation about one of main axis to current
	 transform expressed by the matrix. Code is simple, a temporary Matrix44
	 representing desidered rotation is created and then it is composed with
	 current matrix. Remember that since column vector notation is used, to 
	 add a transformation to an existing matrix, it has to be premultiplicated
	 by new transform. For this reason the Matrix44MultiplyR is used.
	 @param Angle The angle of rotation 
	 @param MainAxis the axis of rotation
	 @return A reference to the new matrix.
*/
Matrix44 &Matrix44::AddRotation(float Angle, MainAxis Axis) {

	switch (Axis) {
	
		case X_AXIS:
			AddRotationX(Angle);
		break;
		case Y_AXIS:
			AddRotationY(Angle);
		break;
		case Z_AXIS:
			AddRotationZ(Angle);
		break;
		default: 
		break;
	}
	return *this;
}

/** .
	 Build a Left Handed View Matrix used to translate geomety from World Coordinate
	 System to Camera (View) Coordinate System. The 
	 @param Eye A vector4 Object specifying the position of the eye of the camera.
	 @param At A vector4 Object specifying the point to look at
	 @param Up A vector4 Object specifying a general up direction for the matrix
	 @return A reference to the current matrix.
*/
Matrix44 &Matrix44::ViewLH(const Vector4 &Eye, const Vector4 &At, const Vector4 &Up) {

	//First of all the vector N is the direction of view, 
	Vector4 N = At - Eye;
	N.Normalize();

	//Up vector is calculated with a simple cross product between the 
	//direction vector and general Up direction
	Vector4 U;
	U = N * Up;
	U.Normalize();

	//Once direction and U vector are known V (Exact Up direction) is 
	//obtained by a simple cross product, note that since the product is
	//U * N the three vector form a Left Handed coordinate system
	Vector4 V = U * N;

	//Now simply build the matrix that perform change of coordinate from
	//World Coordinate System to View Coordinate System
	elem[0][0] = U.x;
	elem[0][1] = U.y;
	elem[0][2] = U.z;
	elem[0][3] = -Dot(U, Eye);
	elem[1][0] = V.x;
	elem[1][1] = V.y;
	elem[1][2] = V.z;
	elem[1][3] = -Dot(V, Eye);
	elem[2][0] = N.x;
	elem[2][1] = N.y;
	elem[2][2] = N.z;
	elem[2][3] = -Dot(N, Eye);
	elem[3][0] = 0;
	elem[3][1] = 0;
	elem[3][2] = 0;
	elem[3][3] = 1;

	//Return a reference to current object
	return *this;
}

/** .
	Build a perspective matrix left handed
	Consult appendix A of the tutorial to have furter details on how this matrix
	is build.
	@param	Width		Width of the viewing frustum
	@param   Height	Height of the viewing frustum
	@param	zNear		Distance of the viewing frustum
	@param   zFar     Far clipping plane of the viewing frustum
*/
Matrix44 &Matrix44::PerspectiveLH(float Width, float Height, float zNear, float zFar) {

	memset(elem, 0, sizeof(elem));
	elem[0][0] = 2 * zNear / Width;
	elem[1][1] = 2 * zNear / Height;
	elem[2][2] = (zFar + zNear) / (zFar - zNear);
	elem[3][2] = 1;
	elem[2][3] = (2 * zFar * zNear) / (zNear - zFar);

	return *this;
}

/** .
	 Buld a Perspective Projection Matrix Rigth handed with the near plane of
	 Viewing Frustum centered on direction of view. To have a complete explanation
	 on how to build such a matrix, consult appendix A of the tutorial.
	 @param Width Width of near plane of viewing frustum
	 @param Height Height of near plane of viewing frustum
	 @param zNear Z-Value of near plane of viewing frustum
	 @param zFar Z-Value of far plane of viewing frustum
	 @return A reference to current function, remember that this matrix has to 
			   mapped to viewport to obtain the full Projection matrix used to 
				transform coordinates from 3D to 2D
*/
Matrix44 &Matrix44::PerspectiveRH(float Width, float Height, float zNear, float zFar) {

	//Reset all element of the matrix
	memset(elem, 0, sizeof(elem));

	//Simply build the matrix, these factors are calculated and explanated
	//into Appendix A of the tutorial.
	elem[0][0] = 2 * zNear / Width;
	elem[1][1] = 2 * zNear / Height;
	elem[2][2] = (zFar + zNear) / (zNear - zFar);
	elem[3][2] = -1;
	elem[2][3] = (2 * zFar * zNear) / (zNear - zFar);

	//Simply return a referente to current object.
	return *this;
}

/** .
    This method does a simple mapping to a given viewport. To know why the
	 MapMatrix has this particular form, consult Appendix A of the tutorial
	 in witch there is a full explanation on how to build a Projection Matrix.
*/
Matrix44 &Matrix44::MapToViewPort(float xMin, float xMax, 
											 float yMin, float yMax,
											 float zMin, float zMax) {
		
   //first of all calculate scaling factor 
	float cx = (xMax - xMin) / 2;
	float cy = (yMax - yMin) / 2;
	float cz = (zMax - zMin) / 2;

	//Then declare a matrix that will represent a Mapping Transform
	Matrix44 MapMatrix;
	MapMatrix.Reset();

	//set the scaling factor in MapMatrix
	MapMatrix.elem[0][0] = cx;
	MapMatrix.elem[1][1] = cy;
	MapMatrix.elem[2][2] = cz;

	//set the translation element
	MapMatrix.elem[0][3] = cx + xMin;
	MapMatrix.elem[1][3] = cy + yMin;
	MapMatrix.elem[2][3] = (cz + zMin);

	//Last row is [0, 0, 0, 1]
	MapMatrix.elem[3][3] = 1;

	//simply multiply MapMatrix for current matrix and store back
	//the result.
	*this = MapMatrix * *this;
	return *this;
}

/** .
	This is the usual * operator for 4x4 Matrix, both matrix object are
	passed as const reference because they must not be modified.
	@param M1 First matrix operand in moltiplication operation
	@param M2 Second matrix operand in moltiplication operation
	@return A temporary const matrix representing the result of the 
			  mulplication operation.
*/
const Matrix44 PS2Math::operator*(const Matrix44 &M1, 
											 const Matrix44 &M2) {
								 
	Matrix44 Result;
	Matrix44Multiply(M1, M2, Result);
	return Result;
}

/** .
	 Operator *= for class Matrix44, it internally use the Matrix44MultiplyL
	 function that multiply two matrix storing result in the Matrix44 object 
	 passed as first(left) parameter. 
	 It perform the operation M1 *= M2, since M2 matrix is not modified it
	 is passed as const reference.
	 @param M1 Matrix44 object used as left matrix
	 @param M2 Matrix44 object used as rigth parameter
	 @return A non const referece to the M1 Matrix object
*/
Matrix44 &PS2Math::operator*=(Matrix44 &M1, 
										const Matrix44 &M2) {
							
	return Matrix44MultiplyL(M1, M2);					
}

/** .
	 This is a standard implementation of operator << to output the 
	 content of a Matrix44 object to a std::outStream, it can be very
	 useful because it's very easy to output content of a matrix to a file
	 for debug purpose. 
*/
std::ostream &PS2Math::operator<<(std::ostream &outStream, const Matrix44 &Mat) {

	outStream << std::setprecision(3);
	outStream << "[" << Mat.elem[0][0] << "\t" << Mat.elem[0][1] << "\t" << 
					 Mat.elem[0][2] << "\t" << Mat.elem[0][3] << "]\n";
	outStream << "[" << Mat.elem[1][0] << "\t" << Mat.elem[1][1] << "\t" << 
					Mat.elem[1][2] << "\t" << Mat.elem[1][3] << "]\n";
	outStream << "[" << Mat.elem[2][0] << "\t" << Mat.elem[2][1] << "\t" << 
					Mat.elem[2][2] << "\t" << Mat.elem[2][3] << "]\n";
	outStream << "[" << Mat.elem[3][0] << "\t" << Mat.elem[3][1] << "\t" << 
					Mat.elem[3][2] << "\t" << Mat.elem[3][3] << "]\n";
	return outStream;
}



