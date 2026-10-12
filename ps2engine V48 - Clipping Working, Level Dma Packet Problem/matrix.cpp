#include "matrix.h"
#include <stdio.h>
#include <math.h>

Matrix_Manager::Matrix_Manager()
{
	ps2_vu0_unit_matrix(viewMatrix[0]);
	ps2_vu0_unit_matrix(projectionMatrix[0]);
	ps2_vu0_unit_matrix(cameraMatrix[0]);
	ps2_vu0_unit_matrix(modelMatrix[0]);
	ps2_vu0_unit_matrix(lightMatrix[0]);
	ps2_vu0_unit_matrix(colorMatrix[0]);
	currentMode = PROJECTION;

	
	currentViewIndex = 0;
	currentModelIndex = 0;
	currentCameraIndex = 0;
	currentLightIndex = 0;
	currentColorIndex = 0;
	currentProjectionIndex = 0;
}

Matrix_Manager::~Matrix_Manager()
{

}

int Matrix_Manager::setMode(int tmpCurrentMode)
{
	int oldMode = currentMode;
	currentMode = tmpCurrentMode;
	return oldMode;
}

int Matrix_Manager::setMatrix(matrixf *tmpMatrix)
{
	matrixf *updateMatrix = getMatrix();

	if (updateMatrix == 0)return 0;

	*updateMatrix = *tmpMatrix;

	return 1;
}

int Matrix_Manager::multiMatrix(matrixf *tmpMatrix)
{
	ps2_vu0_mul_matrix(*getMatrix(), *getMatrix(), *tmpMatrix);

	return 1;
}

matrixf *Matrix_Manager::getMatrix()
{
	switch(currentMode)
	{
	case PROJECTION:
		return &projectionMatrix[currentProjectionIndex];
		break;
	case CAMERA:
		return &cameraMatrix[currentCameraIndex];
		break;
	case MODELVIEW:
		return &modelMatrix[currentModelIndex];
		break;
	case LIGHT:
		return &lightMatrix[currentLightIndex];
		break;
	case COLOR:
		return &colorMatrix[currentColorIndex];
		break;
	case VIEW:
		return &viewMatrix[currentViewIndex];
		break;
	}

	return NULL;
}

int Matrix_Manager::reset()
{
	switch(currentMode)
	{
	case PROJECTION:
		currentProjectionIndex = 0;
		break;
	case CAMERA:
		currentCameraIndex = 0;
		break;
	case MODELVIEW:
		currentModelIndex = 0;
		break;
	case LIGHT:
		currentLightIndex = 0;
		break;
	case COLOR:
		currentColorIndex = 0;
		break;
	case VIEW:
		currentViewIndex = 0;
		break;
	}

	return 1;
}

int Matrix_Manager::loadIdentity()
{
	ps2_vu0_unit_matrix(*getMatrix());
	
	return 1;
}

int Matrix_Manager::push()
{
	switch(currentMode)
	{
	case PROJECTION:
		if (currentProjectionIndex < 1)
		{
			projectionMatrix[currentProjectionIndex + 1] = projectionMatrix[currentProjectionIndex];
			currentProjectionIndex++;
			return 1;
		}
		break;
	case CAMERA:
		if (currentCameraIndex < 1)
		{
			cameraMatrix[currentCameraIndex + 1] = cameraMatrix[currentCameraIndex];
			currentCameraIndex++;
			return 1;
		}
		break;
	case MODELVIEW:
		if (currentModelIndex < 31)
		{
			modelMatrix[currentModelIndex + 1] = modelMatrix[currentModelIndex];
			currentModelIndex++;
			return 1;
		}
		break;
	case LIGHT:
		if (currentLightIndex < 1)
		{
			lightMatrix[currentLightIndex + 1] = lightMatrix[currentLightIndex];
			currentLightIndex++;
			return 1;
		}
		break;
	case COLOR:
		if (currentColorIndex < 1)
		{
			colorMatrix[currentColorIndex + 1] = colorMatrix[currentColorIndex];
			currentColorIndex++;
			return 1;
		}
		break;
	case VIEW:
		if (currentViewIndex < 1)
		{
			viewMatrix[currentViewIndex + 1] = viewMatrix[currentViewIndex];
			currentViewIndex++;
			return 1;
		}
		break;
	}
	return 0;
}

int Matrix_Manager::pop()
{
	switch(currentMode)
	{
	case PROJECTION:
		if (currentProjectionIndex > 0)
		{
			currentProjectionIndex--;
			return 1;
		}
		break;
	case CAMERA:
		if (currentCameraIndex > 0)
		{
			currentCameraIndex--;
			return 1;
		}
		break;
	case MODELVIEW:
		if (currentModelIndex > 0)
		{
			currentModelIndex--;
			return 1;
		}
		break;
	case LIGHT:
		if (currentLightIndex > 0)
		{
			currentLightIndex--;
			return 1;
		}
		break;
	case COLOR:
		if (currentColorIndex > 0)
		{
			currentColorIndex--;
			return 1;
		}
		break;
	case VIEW:
		if (currentViewIndex > 0)
		{
			currentViewIndex--;
			return 1;
		}
		break;
	}
	
	return 0;
}

/*
int Matrix_Manager::setProjection(float fov, float aspect, float nearZ, float farZ)
{	
	float rads = (fov / 180.0f) * PI * 0.5f;
	float tanFOV = (sinf(rads) / cosf(rads));
	matrixf workMatrix;

	ps2_vu0_unit_matrix(workMatrix);
	

	workMatrix[0][0] = 1.0f / (aspect * tanFOV);
	workMatrix[1][1] = -1.0f / tanFOV;
	workMatrix[2][2] = (farZ + nearZ) / (farZ - nearZ);
	workMatrix[3][2] = -(2 * farZ * nearZ) / (farZ - nearZ);
	workMatrix[2][3] = -1.0f;
	workMatrix[3][3] = 0.0f;
	
	//ps2_vu0_transpose_matrix(workMatrix, workMatrix);
	setMatrix(&workMatrix);


	return 1;
}
*/

int Matrix_Manager::setProjection(float fov, float aspect, float nearZ, float farZ)
{	
	float rads = (fov / 180.0f) * PI * 0.5f;
	float cotFOV = 1.0f / (sinf(rads) / cosf(rads));
	matrixf workMatrix;

	ps2_vu0_unit_matrix(workMatrix);
	

	workMatrix[0][0] = cotFOV * (screenWidth / 4096.0f) / aspect; //1.0f / (aspect * tanFOV);
	workMatrix[1][1] = -(cotFOV * (screenHeight / 4096.0f));//-1.0f / tanFOV;
	workMatrix[2][2] = (farZ + nearZ) / (farZ - nearZ);
	workMatrix[3][2] = -(2 * farZ * nearZ) / (farZ - nearZ);
	workMatrix[2][3] = -1.0f;
	workMatrix[3][3] = 0.0f;

	//ps2_vu0_transpose_matrix(workMatrix, workMatrix);
	setMatrix(&workMatrix);

	return 1;
}


int Matrix_Manager::setProjectionScale(float x, float y, float z)
{
	projectionScale[0] = x;
	projectionScale[1] = y;
	projectionScale[2] = z;
	projectionScale[3] = 1.0f;
	return 1;
}

float *Matrix_Manager::getProjectionScale()
{
	return &projectionScale[0];
}

int Matrix_Manager::setView(float width, float height, float offsetx, float offsety, float zbuffer)
{
	matrixf workMatrix;

	ps2_vu0_unit_matrix(workMatrix);
	//workMatrix = getMatrix();
	screenWidth = width;
	screenHeight = height;

	workMatrix[0][0] = width * 0.5f * 16.0f;
	workMatrix[1][1] = height * 0.5f * 16.0f;
	workMatrix[2][2] = zbuffer * 0.5f;
	workMatrix[3][0] = offsetx * 16.0f;
	workMatrix[3][1] = offsety * 16.0f;
	workMatrix[3][2] = zbuffer * 0.5f;

	//ps2_vu0_transpose_matrix(workMatrix, workMatrix);
	
	setMatrix(&workMatrix);

	return 1;
}



int Matrix_Manager::setCamera(vectorf position, vectorf eyeDirection, vectorf verticalDirection)
{
	matrixf cameraMatrix;
	vectorf axisZ;
	vectorf axisX;
	vectorf axisY;

	ps2_vu0_sub_vector(axisZ, position, eyeDirection);
	ps2_vu0_normalize(axisZ, axisZ);

	ps2_vu0_outer_product(axisX, verticalDirection, axisZ);
	ps2_vu0_normalize(axisX, axisX); 	

	ps2_vu0_outer_product(axisY, axisZ, axisX);

	cameraMatrix[0][0] = axisX[0];
	cameraMatrix[1][0] = axisX[1];
	cameraMatrix[2][0] = axisX[2];
	cameraMatrix[3][0] = -ps2_vu0_inner_product(axisX, position);

	cameraMatrix[0][1] = axisY[0];
	cameraMatrix[1][1] = axisY[1];
	cameraMatrix[2][1] = axisY[2];
	cameraMatrix[3][1] = -ps2_vu0_inner_product(axisY, position);

	cameraMatrix[0][2] = axisZ[0];
	cameraMatrix[1][2] = axisZ[1];
	cameraMatrix[2][2] = axisZ[2];
	cameraMatrix[3][2] = -ps2_vu0_inner_product(axisZ, position);

	cameraMatrix[0][3] = 0.0f;
	cameraMatrix[1][3] = 0.0f;
	cameraMatrix[2][3] = 0.0f;
	cameraMatrix[3][3] = 1.0f;

	//ps2_vu0_transpose_matrix(cameraMatrix, cameraMatrix);

	setMatrix(&cameraMatrix);

	//ps2_vu0_camera_matrix(*getMatrix(), position, eyeDirection, verticalDirection);
	return 1;
}


int Matrix_Manager::setColor(vectorf color0, vectorf color1, vectorf color2, vectorf ambient)
{
	ps2_vu0_light_color_matrix(*getMatrix(), color0, color1, color2, ambient);
	return 1;
}

int Matrix_Manager::setLight(vectorf direction1, vectorf direction2, vectorf direction3)
{
	ps2_vu0_normal_light_matrix(*getMatrix(), direction1, direction2, direction3);
	return 1;
}

int Matrix_Manager::rotate(float angle, float rotX, float rotY, float rotZ)
{
	matrixf workMatrix;
	ps2_vu0_unit_matrix(workMatrix);
	vectorf rotVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	if (angle > PI || angle < -PI)return 0;
	rotVector[0] = rotX * angle;
	rotVector[1] = rotY * angle;
	rotVector[2] = rotZ * angle;
	
	ps2_vu0_rot_matrix(workMatrix, workMatrix, rotVector);
	multiMatrix(&workMatrix);

	return 1;
}

int Matrix_Manager::rotate(vectorf rotation)
{
	ps2_vu0_rot_matrix(*getMatrix(), *getMatrix(), rotation);

	return 1;
}

int Matrix_Manager::translate(float tranX, float tranY, float tranZ)
{
	matrixf workMatrix;
	ps2_vu0_unit_matrix(workMatrix);
	vectorf transVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	transVector[0] = tranX;
	transVector[1] = tranY;
	transVector[2] = tranZ;

	ps2_vu0_trans_matrix(workMatrix, workMatrix, transVector );
	multiMatrix(&workMatrix);

	return 1;
}

int Matrix_Manager::translate(vectorf translation)
{
	ps2_vu0_trans_matrix(*getMatrix(), *getMatrix(), translation);

	return 1;
}

int Matrix_Manager::scale(float scaleX, float scaleY, float scaleZ)
{
	vectorf scaleVector = { 0.0f, 0.0f, 0.0f, 1.0f };
	scaleVector[0] = scaleX;
	scaleVector[1] = scaleY;
	scaleVector[2] = scaleZ;

	ps2_vu0_scale_matrix(*getMatrix(), *getMatrix(), scaleVector);
	
	return 1;	
}

int Matrix_Manager::scale(vectorf scalar)
{
	ps2_vu0_scale_matrix(*getMatrix(), *getMatrix(), scalar);

	return 1;
}

int Matrix_Manager::dump(char *fileName)
{
	FILE *outputFile;
	matrixf work;
	int x, y;

	work = *getMatrix();
	
	outputFile = fopen(fileName,"w");
	if (outputFile == NULL)return 0;

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			fprintf(outputFile, "%20.8f", work[y][x]);
		}
		fprintf(outputFile, "\n");
	}

	fclose(outputFile);

	return 1;
}


