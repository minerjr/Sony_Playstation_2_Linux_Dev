/** @file Matrix44Operation.cpp
	Contain code for various operations for Matrix44 Class.
	This file contains helper functions such as Matrix44Multiply used to
	implement Matrix44 Class.
	@author		Alkampfer
*/

#include "PS2Math.h"

using namespace PS2Math;

/** .
	This function is used to multiply two Matrix44 object, pay attenction that the
	destination matrix cannot be one of the two matrix used for the operation. 
	A call of the type Matrix44Multiply(M1, M1, M2) is wrong, use 
	Matrix44MultiplyL(Matrix44 &, const Matrix44 &) instead. This fact is due
	to the implementation of this function that calculate one element at a time
	and immediatly store them in result matrix. This function is not to be used
	directly: to multiply two Matrix44 object is preferible to use the * operator.
	@param M1 Right Matrix involved into the multiplication
	@param M2 Left Matrix involved into the multiplication
	@param Result Matrix in with store the result
	@return A reference to Result matrix passed as parameters.
	@deprecated
*/
Matrix44 &PS2Math::Matrix44Multiply(const Matrix44 &M1, 
												const Matrix44 &M2, 
														Matrix44 &Result) {

	Result.elem[0][0] = M1.elem[0][0] * M2.elem[0][0] + M1.elem[0][1] * M2.elem[1][0] + M1.elem[0][2] * M2.elem[2][0] + M1.elem[0][3] * M2.elem[3][0];
	Result.elem[0][1] = M1.elem[0][0] * M2.elem[0][1] + M1.elem[0][1] * M2.elem[1][1] + M1.elem[0][2] * M2.elem[2][1] + M1.elem[0][3] * M2.elem[3][1];
	Result.elem[0][2] = M1.elem[0][0] * M2.elem[0][2] + M1.elem[0][1] * M2.elem[1][2] + M1.elem[0][2] * M2.elem[2][2] + M1.elem[0][3] * M2.elem[3][2];
	Result.elem[0][3] = M1.elem[0][0] * M2.elem[0][3] + M1.elem[0][1] * M2.elem[1][3] + M1.elem[0][2] * M2.elem[2][3] + M1.elem[0][3] * M2.elem[3][3];
	
	Result.elem[1][0] = M1.elem[1][0] * M2.elem[0][0] + M1.elem[1][1] * M2.elem[1][0] + M1.elem[1][2] * M2.elem[2][0] + M1.elem[1][3] * M2.elem[3][0];
	Result.elem[1][1] = M1.elem[1][0] * M2.elem[0][1] + M1.elem[1][1] * M2.elem[1][1] + M1.elem[1][2] * M2.elem[2][1] + M1.elem[1][3] * M2.elem[3][1];
	Result.elem[1][2] = M1.elem[1][0] * M2.elem[0][2] + M1.elem[1][1] * M2.elem[1][2] + M1.elem[1][2] * M2.elem[2][2] + M1.elem[1][3] * M2.elem[3][2];
	Result.elem[1][3] = M1.elem[1][0] * M2.elem[0][3] + M1.elem[1][1] * M2.elem[1][3] + M1.elem[1][2] * M2.elem[2][3] + M1.elem[1][3] * M2.elem[3][3];

	Result.elem[2][0] = M1.elem[2][0] * M2.elem[0][0] + M1.elem[2][1] * M2.elem[1][0] + M1.elem[2][2] * M2.elem[2][0] + M1.elem[2][3] * M2.elem[3][0];
	Result.elem[2][1] = M1.elem[2][0] * M2.elem[0][1] + M1.elem[2][1] * M2.elem[1][1] + M1.elem[2][2] * M2.elem[2][1] + M1.elem[2][3] * M2.elem[3][1];
	Result.elem[2][2] = M1.elem[2][0] * M2.elem[0][2] + M1.elem[2][1] * M2.elem[1][2] + M1.elem[2][2] * M2.elem[2][2] + M1.elem[2][3] * M2.elem[3][2];
	Result.elem[2][3] = M1.elem[2][0] * M2.elem[0][3] + M1.elem[2][1] * M2.elem[1][3] + M1.elem[2][2] * M2.elem[2][3] + M1.elem[2][3] * M2.elem[3][3];

	Result.elem[3][0] = M1.elem[3][0] * M2.elem[0][0] + M1.elem[3][1] * M2.elem[1][0] + M1.elem[3][2] * M2.elem[2][0] + M1.elem[3][3] * M2.elem[3][0];
	Result.elem[3][1] = M1.elem[3][0] * M2.elem[0][1] + M1.elem[3][1] * M2.elem[1][1] + M1.elem[3][2] * M2.elem[2][1] + M1.elem[3][3] * M2.elem[3][1];
	Result.elem[3][2] = M1.elem[3][0] * M2.elem[0][2] + M1.elem[3][1] * M2.elem[1][2] + M1.elem[3][2] * M2.elem[2][2] + M1.elem[3][3] * M2.elem[3][2];
	Result.elem[3][3] = M1.elem[3][0] * M2.elem[0][3] + M1.elem[3][1] * M2.elem[1][3] + M1.elem[3][2] * M2.elem[2][3] + M1.elem[3][3] * M2.elem[3][3];

	return Result;
}

/* .
	This version of the Matrix44 Multiplication is used to do operation such 
	M1 = M1 * M2, normal version of the function does not use a temporary object so 
	it cannot be used. This version of the function execute operation M1 * M2 and
	store the result in the left operand (M1). This is the reason for suffix L
	(Left) in his name.
	@param M1 Left matrix involved into the operation
	@param M2 Right matrix involved into the operation
	@return A reference to the left matrix (first parameter) 
*/
Matrix44 &PS2Math::Matrix44MultiplyL(Matrix44 &M1_result, 
										 const Matrix44 &M2) {

	Matrix44 Temp;
	Temp.elem[0][0] = M1_result.elem[0][0] * M2.elem[0][0] + M1_result.elem[0][1] * M2.elem[1][0] + M1_result.elem[0][2] * M2.elem[2][0] + M1_result.elem[0][3] * M2.elem[3][0];
	Temp.elem[0][1] = M1_result.elem[0][0] * M2.elem[0][1] + M1_result.elem[0][1] * M2.elem[1][1] + M1_result.elem[0][2] * M2.elem[2][1] + M1_result.elem[0][3] * M2.elem[3][1];
	Temp.elem[0][2] = M1_result.elem[0][0] * M2.elem[0][2] + M1_result.elem[0][1] * M2.elem[1][2] + M1_result.elem[0][2] * M2.elem[2][2] + M1_result.elem[0][3] * M2.elem[3][2];
	Temp.elem[0][3] = M1_result.elem[0][0] * M2.elem[0][3] + M1_result.elem[0][1] * M2.elem[1][3] + M1_result.elem[0][2] * M2.elem[2][3] + M1_result.elem[0][3] * M2.elem[3][3];
	
	Temp.elem[1][0] = M1_result.elem[1][0] * M2.elem[0][0] + M1_result.elem[1][1] * M2.elem[1][0] + M1_result.elem[1][2] * M2.elem[2][0] + M1_result.elem[1][3] * M2.elem[3][0];
	Temp.elem[1][1] = M1_result.elem[1][0] * M2.elem[0][1] + M1_result.elem[1][1] * M2.elem[1][1] + M1_result.elem[1][2] * M2.elem[2][1] + M1_result.elem[1][3] * M2.elem[3][1];
	Temp.elem[1][2] = M1_result.elem[1][0] * M2.elem[0][2] + M1_result.elem[1][1] * M2.elem[1][2] + M1_result.elem[1][2] * M2.elem[2][2] + M1_result.elem[1][3] * M2.elem[3][2];
	Temp.elem[1][3] = M1_result.elem[1][0] * M2.elem[0][3] + M1_result.elem[1][1] * M2.elem[1][3] + M1_result.elem[1][2] * M2.elem[2][3] + M1_result.elem[1][3] * M2.elem[3][3];

	Temp.elem[2][0] = M1_result.elem[2][0] * M2.elem[0][0] + M1_result.elem[2][1] * M2.elem[1][0] + M1_result.elem[2][2] * M2.elem[2][0] + M1_result.elem[2][3] * M2.elem[3][0];
	Temp.elem[2][1] = M1_result.elem[2][0] * M2.elem[0][1] + M1_result.elem[2][1] * M2.elem[1][1] + M1_result.elem[2][2] * M2.elem[2][1] + M1_result.elem[2][3] * M2.elem[3][1];
	Temp.elem[2][2] = M1_result.elem[2][0] * M2.elem[0][2] + M1_result.elem[2][1] * M2.elem[1][2] + M1_result.elem[2][2] * M2.elem[2][2] + M1_result.elem[2][3] * M2.elem[3][2];
	Temp.elem[2][3] = M1_result.elem[2][0] * M2.elem[0][3] + M1_result.elem[2][1] * M2.elem[1][3] + M1_result.elem[2][2] * M2.elem[2][3] + M1_result.elem[2][3] * M2.elem[3][3];

	Temp.elem[3][0] = M1_result.elem[3][0] * M2.elem[0][0] + M1_result.elem[3][1] * M2.elem[1][0] + M1_result.elem[3][2] * M2.elem[2][0] + M1_result.elem[3][3] * M2.elem[3][0];
	Temp.elem[3][1] = M1_result.elem[3][0] * M2.elem[0][1] + M1_result.elem[3][1] * M2.elem[1][1] + M1_result.elem[3][2] * M2.elem[2][1] + M1_result.elem[3][3] * M2.elem[3][1];
	Temp.elem[3][2] = M1_result.elem[3][0] * M2.elem[0][2] + M1_result.elem[3][1] * M2.elem[1][2] + M1_result.elem[3][2] * M2.elem[2][2] + M1_result.elem[3][3] * M2.elem[3][2];
	Temp.elem[3][3] = M1_result.elem[3][0] * M2.elem[0][3] + M1_result.elem[3][1] * M2.elem[1][3] + M1_result.elem[3][2] * M2.elem[2][3] + M1_result.elem[3][3] * M2.elem[3][3];
	
	M1_result = Temp;
	return M1_result;
}

/* .
	This version of the Matrix44 Multiplication is used to do operation such 
	M2 = M1 * M2, normal version of the function does not use a temporary object so 
	it cannot be used. This version of the function execute operation M1 * M2 and
	store the result in the RIGHT operand (M2). This is the reason for suffix L
	(Left) in his name.
	@param M1 Left matrix involved into the operation
	@param M2 Right matrix involved into the operation
	@return A reference to the right matrix (second parameter) 
*/
Matrix44 &PS2Math::Matrix44MultiplyR(const Matrix44 &M1, 
												       Matrix44 &M2_result) {
			
   //For this version simple operator * is used.
	M2_result = M1 * M2_result;
	return M2_result;
}

/** .
	 This is used to Transform a Vector44 by a transformation function 
	 expressed by a Matrix44, result is transformed vector.
	 @param M Matrix of the Transformation to apply to the vector
	 @param V Vector to be transformed
	 @return A new const Vector4 object that represent V transformed by 
				matrix M
*/
const Vector4 PS2Math::operator*(const Matrix44 &M, const Vector4 &V) {

	Vector4 Result;
	Result[0] = M[0][0] * V[0] + M[0][1] * V[1] + M[0][2] * V[2] + M[0][3] * V[3];
	Result[1] = M[1][0] * V[0] + M[1][1] * V[1] + M[1][2] * V[2] + M[1][3] * V[3];
	Result[2] = M[2][0] * V[0] + M[2][1] * V[1] + M[2][2] * V[2] + M[2][3] * V[3];
	Result[3] = M[3][0] * V[0] + M[3][1] * V[1] + M[3][2] * V[2] + M[3][3] * V[3];

	return Result;
}