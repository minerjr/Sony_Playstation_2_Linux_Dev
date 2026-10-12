/** @file PS2Math.h
	Contains definitions of maths classes.
	<h4>Defined Classes:</h4>
		PS2Math::Matrix44<br>
		PS2Math::Vector4<br>
*/
#ifndef NABLA_MATRIX_H__
#define NABLA_MATRIX_H__

#include <iostream>
#include <cstdlib>
#include <cstring>

/** Namespace for maths needed by the tutorials
    This namespace contains various classes to handle math needed by tutorial
	 examples. 
*/
namespace PS2Math {

	//!This enum is used to specify rotation axis for various functions of Matrix44
	enum MainAxis {
	
		X_AXIS,
		Y_AXIS, 
		Z_AXIS
	};

	class Vector4;

	/** Class to handle square 4x4 matrix
		 This class is build not to handle a general 4x4 matrix but to work with
		 linear function in omogeneous space. Class Matrix44 has special member 
		 function to use rotation, translation and to set particular type of 
		 transformation as View transform, Projection transform of Viewport 
		 mapping transform. The use of operator overloading make simple and 
		 efficient the use of this class, all code is not optimized because this
		 class has to be use as suppor for a tutorial and has to be simpliest as
		 possible.
	*/
	class Matrix44 {

	public:

		//! Array that contains the element of the matrix.
		float			elem[4][4]; 

		//! Constructor 1, if Identity is true then an identity 4x4 matrix is created
		Matrix44(bool Identity = false);	

		//! To build directly a RotationMatrix
		Matrix44(float Angle, MainAxis Axis);	

		//! Reset function set all values of the matrix to 0.0f
		void Reset() {memset(elem, 0, sizeof(elem));}
		
		//! Add a rotation to the current matrix
		Matrix44 &AddRotation(float Angle, MainAxis Axis);

		/** @name Rotations member functions. 
		These functions are used to set or add a particular kind of rotation to the 
		instance of the matrix. 
		@{
		*/
		//!Set a rotation of desidered angle about X axis
		Matrix44 &SetRotationX(float Angle);
		//!Set a rotation of desidered angle about Y axis
		Matrix44 &SetRotationY(float Angle);
		//!Set a rotation of desidered angle about Z axis
		Matrix44 &SetRotationZ(float Angle);
		//!Add a rotation of desidered angle about X axis
		Matrix44 &AddRotationX(float Angle);
		//!Add a rotation of desidered angle about Y axis
		Matrix44 &AddRotationY(float Angle);
		//!Add a rotation of desidered angle about Z axis
		Matrix44 &AddRotationZ(float Angle);
		//! @}

		//! Set this instance of the matrix to a Left Handed View Matrix
		Matrix44 &ViewLH(const Vector4 &Eye, const Vector4 &At, const Vector4 &Up);
		//! Set this instance of the matrix to a Rigth Handed View Matrix
		Matrix44 &ViewRH(const Vector4 &Eye, const Vector4 &At, const Vector4 &Up);
		
		//! Set this instance of the matrix to a Left Handed Projection Matrix
		Matrix44 &PerspectiveLH(float Width, float Height, float zNear, float zFar);
		//! Set this instance of the matrix to a Rigth Handed Projection Matrix
		Matrix44 &PerspectiveRH(float Width, float Height, float zNear, float zFar);
		//! Map a projection matrix to a ViewPort, it is used to buld final projection matrix
		Matrix44 &MapToViewPort(float xMin, float xMax, 
										float yMin, float yMax,
										float zMin,	float zMax);

		//! Operator [] used to access row as array of float.
		float * const operator[](int Index) {return elem[Index];}
		//! Operator [] used to access row as array of float. (Const version)
		const float *operator[](int Index) const {return elem[Index];}
	};

	//!Matrix Multiplication, result is stored in new matrix
	Matrix44 &Matrix44Multiply(const Matrix44 &M1, 
									   const Matrix44 &M2, 
												Matrix44 &Result); 
	//!Matrix Multiplication, result is stored in first matrix.
	Matrix44 &Matrix44MultiplyL(Matrix44 &M1_result, 
									   const Matrix44 &M2); 
	//!Matrix Multiplication, result is stored in first matrix.
	Matrix44 &Matrix44MultiplyR(const Matrix44 &M1, 
									   Matrix44 &M2_result); 

	//!Operators * for class Matrix44
	const Matrix44 operator*(const Matrix44 &M1, 
									 const Matrix44 &M2);

	//!Operators *= for class Matrix44
	Matrix44 &operator*=(Matrix44 &M1, 
							   const Matrix44 &M2);

	//! To print Matrix44 Element with standard stream, useful for debugging.
	std::ostream &operator<<(std::ostream &outStream, const Matrix44 &Mat);

	/** Class representing an omogeneous vector.
		 This class is used to deal with omogeneous vector, such a class is needed
		 to have operators that works with matrix44 class and to define vectors 
		 operations such as dot and cross product using omogeneous coordinates.
	*/
	class Vector4 {
	
	public:

		//! Values storing coordinates of the vertex
		float x, y, z, w;

		//! Simple constructor, there is nothing to initialize
		Vector4() {}
		//! Contructor to initialize the vector with coordinates values
		Vector4(float x, float y, float z, float w = 1);

		//! Divide x,y,z by w to find real R3 coordinates.
		Vector4 &Omogenize();

		//! Normalize vector dividing by the module
		Vector4 &Normalize();

		//! Divide x, y by w, put w to z and set w to 1, used to omogenize a projected vertex
		Vector4 &OmogenizeWtoZ();
		
		//! Transform data into fixed point format of front buffer of GS
		float	  ConvertToFixedPoint(int []);

		//! operator to use this class as an array.
		float &operator[](int Index);
		//! operator to use this class as an array. (Const version)
		const float operator[](int Index) const;
		//! operator to use vector as an array of float.
	};

	//! Multiply a Matrix44 for a vector, remember that I use column vectors
	const Vector4 operator*(const Matrix44 &M, const Vector4 &V);
	//! Negate a vector, same module, same direction opposite verse
	const Vector4 operator-(const Vector4 &V);
	//! Cross product between two vectors in a Rigth handed coordinate system
	const Vector4 operator*(const Vector4 &V1, const Vector4 &V2);
	//! Subtraction between two vertex
	const Vector4 operator-(const Vector4 &V1, const Vector4 &V2);
	//! Addiction between two vertex.
	const Vector4 operator+(const Vector4 &V1, const Vector4 &V2);

	/** External function to do Dot operation, this because operator * is used 
		 for cross product.
	*/
	float Dot(const Vector4 &V1, const Vector4 &V2);
	
	//! To print Vector4 Element with standard stream, useful for debugging.
	std::ostream &operator<<(std::ostream &outStream, const Vector4 &V);
}

#endif
