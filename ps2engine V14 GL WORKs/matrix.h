/*
Matrix Manager by David Froy

Last Modifed April 11, 2004

Matrix Manager designed after opengl way of doing matrixs.
WARNING the ps2 vu0 uses transposed matrixs (ie they are inverted)

Matrix Manager allows for use of pushing and poping of the different matrixs.
View Matrix - used for the view port, the push/pop stack is size 2
Projection Matrix - used for the projection, the push/pop stack is size 2
Camera Matrix - used for the world/camera view, the push/pop stack is size 2
Model Matrix - used for the model view, the push/pop stack is size 32
Light Matrix - used for the light system, the push/pop stack is size 2
Colol Matrix - used for the color system, the push/pop stack is size 2
*/

#ifndef MATRIX_H
#define MATRIX_H

//Include extra functions missing from the libvu0 code
#include "matrixfunc.h"

//Defines for the different types of matrixs
#define PROJECTION 1
#define CAMERA 2
#define MODELVIEW 4
#define LIGHT 8
#define COLOR 16
#define VIEW 32


//Typedef long names to shorter, more human names
typedef ps2_vu0_fmatrix matrixf;
typedef ps2_vu0_fvector vectorf;

class Matrix_Manager
{
private:
	
	//The actual matrix's, note that they are all static, needed
	//for the ps2 vu0 code to work
	static matrixf viewMatrix[2];
	static matrixf projectionMatrix[2];
	static matrixf cameraMatrix[2];
	static matrixf lightMatrix[2];
	static matrixf colorMatrix[2];
	static matrixf modelMatrix[32];

	//Indexs of all the matrixs, used for push/pop
	int currentModelIndex;
	int currentLightIndex;
	int currentColorIndex;
	int currentCameraIndex;
	int currentProjectionIndex;
	int currentViewIndex;

	//The current matrix mode
	int currentMode;


public:
	//Basic constructor/deconstor
	Matrix_Manager();
	~Matrix_Manager();

	//Set the current matrix to the given matrix
	int setMatrix(matrixf *tmpMatrix);
	//Gets the current matrix and returns it
	matrixf *getMatrix();
	//Push the current matrix onto the stack
	int push();
	//Pop the old matrix off the stack
	int pop();

	//Multiply the current matrix by the given matrix
	int multiMatrix(matrixf *tmpMatrix);

	//Setup the current matrix as a view port matrix
	int setView(float width, float height, float offsetx, float offsety, float zbuffer);
	//Setup the current matrix as a projection matrix(use this one)
	int setProjection(float fov, float aspect, float nearZ, float farZ);

	//Both of these are broken, failed attempts to figure out the view and perspective all in on
	//command,
	int setProjection(float screenWidth, float screenHeight, float nearZ, float farZ, float minZ, float maxZ);
	int setProjection(float screenDistence, float aspectRatiox, float aspectRatioy, 
		float centerX, float centerY, float zMin, float zMax, float nearZ, float farZ);

	//Don't use this one, it's broken horibly
	int setCamera(vectorf position, vectorf eyeDirection, vectorf verticalDirection, vectorf rotation); 
	//Setup the current matrix as a camera matrix (use this one)
	int setCamera(vectorf position, vectorf eyeDirection, vectorf verticalDirection); 

	//Setup the current matrix as a color matrix
	int setColor(vectorf color0, vectorf color1, vectorf color2, vectorf ambient);
	//Setup the current matrix as a light matrix
	int setLight(vectorf direction1, vectorf direction2, vectorf direction3);

	//Rotote the current matrix
	int rotate(float angle, float rotX, float rotY, float rotZ);
	int rotate(vectorf rotation);
	

	//Translates the current matrix
	int translate(float tranX, float tranY, float tranZ);
	int translate(vectorf translation);
	
	//Scales the current matrix
	int scale(float scaleX, float scaleY, float scaleZ);
	int scale(vectorf scalar);

	int setMode(int tmpCurrentMode);
	int reset();
	int loadIdentity();

	int dump(char *fileName);

	void applyMatrix(vectorf * const oVector,matrixf * const aMatrix, vectorf * const aVector);

};

matrixf Matrix_Manager::viewMatrix[2];
matrixf Matrix_Manager::projectionMatrix[2];
matrixf Matrix_Manager::cameraMatrix[2];
matrixf Matrix_Manager::lightMatrix[2];
matrixf Matrix_Manager::colorMatrix[2];
matrixf Matrix_Manager::modelMatrix[32];




#endif
