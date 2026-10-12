#ifndef MATRIX_H
#define MATRIX_H

#include "matrixfunc.h"

#define PROJECTION 1
#define CAMERA 2
#define MODELVIEW 4
#define LIGHT 8
#define COLOR 16

typedef ps2_vu0_fmatrix matrixf;
typedef ps2_vu0_fvector vectorf;

class Matrix_Manager
{
private:
	//ps2_vu0_fmatrix viewport;
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

	int currentMode;

public:
	Matrix_Manager();
	~Matrix_Manager();

	int matrixMode(int tmpCurrentMode);
	int setMatrix(matrixf *tmpMatrix);
	matrixf *getMatrix();
	int push();
	int pop();

	int multiMatrix(matrixf *tmpMatrix);

	int setProjection(float screenDistence, float aspectRatiox, float aspectRatioy, 
		float centerX, float centerY, float zMin, float zMax, float nearZ, float farZ);

	int setCamera(vectorf position, vectorf eyeDirection, vectorf verticalDirection, vectorf rotation); 
	
	int rotate(float angle, float rotX, float rotY, float rotZ);
	int rotate(vectorf rotation);
	//int rotate(int rotX, int rotY, int rotZ, float angle);

	int translate(float tranX, float tranY, float tranZ);
	int translate(vectorf translation);
	//int translate(int tranX, int tranY, int tranZ);

	int scale(float scaleX, float scaleY, float scaleZ);
	int scale(vectorf scalar);

	int setMode(int tmpCurrentMode);
	int reset();
	int loadIdentity();

	int dump(char *fileName);
};

matrixf Matrix_Manager::projectionMatrix[2];
matrixf Matrix_Manager::cameraMatrix[2];
matrixf Matrix_Manager::lightMatrix[2];
matrixf Matrix_Manager::colorMatrix[2];
matrixf Matrix_Manager::modelMatrix[32];




#endif
