#ifndef MATRIX_H
#define MATRIX_H

#include "matrixfunc.h"

#define PI 3.1415926535897932384626433832795f

#define PROJECTION 1
#define CAMERA 2
#define MODELVIEW 4
#define LIGHT 8
#define COLOR 16
#define VIEW 32

typedef ps2_vu0_fmatrix matrixf;
typedef ps2_vu0_fvector vectorf;

class Matrix_Manager
{
private:
	//ps2_vu0_fmatrix viewport;
	static matrixf viewMatrix[2];
	static matrixf projectionMatrix[2];
	static matrixf cameraMatrix[2];
	static matrixf lightMatrix[2];
	static matrixf colorMatrix[2];
	static matrixf modelMatrix[32];

	int currentModelIndex;
	int currentLightIndex;
	int currentColorIndex;
	int currentCameraIndex;
	int currentProjectionIndex;
	int currentViewIndex;

	int currentMode;

	static vectorf projectionScale;
	float screenWidth;
	float screenHeight;


public:
	Matrix_Manager();
	~Matrix_Manager();

	int setMatrix(matrixf *tmpMatrix);
	matrixf *getMatrix();
	int push();
	int pop();

	int multiMatrix(matrixf *tmpMatrix);

	int setView(float width, float height, float offsetx, float offsety, float zbuffer);
	int setProjection(float fov, float aspect, float nearZ, float farZ);

	/*
	int setProjection(float screenWidth, float screenHeight, float nearZ, float farZ, float minZ, float maxZ);
	int setProjection(float screenDistence, float aspectRatiox, float aspectRatioy, 
		float centerX, float centerY, float zMin, float zMax, float nearZ, float farZ);
	*/
	//int setCamera(vectorf position, vectorf eyeDirection, vectorf verticalDirection, vectorf rotation); 
	int setCamera(vectorf position, vectorf eyeDirection, vectorf verticalDirection); 

	
	int setColor(vectorf color0, vectorf color1, vectorf color2, vectorf ambient);

	int setLight(vectorf direction1, vectorf direction2, vectorf direction3);

	int rotate(float angle, float rotX, float rotY, float rotZ);
	int rotate(vectorf rotation);
	//int rotate(int rotX, int rotY, int rotZ, float angle);

	int translate(float tranX, float tranY, float tranZ);
	int translate(vectorf translation);


	int scale(float scaleX, float scaleY, float scaleZ);
	int scale(vectorf scalar);

	int setMode(int tmpCurrentMode);
	int reset();
	int loadIdentity();

	int setProjectionScale(float x, float y, float z);
	float *getProjectionScale();

	int dump(char *fileName);


};

matrixf Matrix_Manager::viewMatrix[2];
matrixf Matrix_Manager::projectionMatrix[2];
matrixf Matrix_Manager::cameraMatrix[2];
matrixf Matrix_Manager::lightMatrix[2];
matrixf Matrix_Manager::colorMatrix[2];
matrixf Matrix_Manager::modelMatrix[32];
vectorf Matrix_Manager::projectionScale;



#endif
