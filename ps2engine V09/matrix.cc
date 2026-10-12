#include "matrix.h"
#include <stdio.h>

Matrix_Manager::Matrix_Manager()
{
	ps2_vu0_unit_matrix(projectionMatrix[0]);
	ps2_vu0_unit_matrix(cameraMatrix[0]);
	ps2_vu0_unit_matrix(modelMatrix[0]);
	ps2_vu0_unit_matrix(lightMatrix[0]);
	ps2_vu0_unit_matrix(colorMatrix[0]);
	currentMode = PROJECTION;
}

Matrix_Manager::~Matrix_Manager()
{

}

int Matrix_Manager::setMode(int tmpCurrentMode)
{
	currentMode = tmpCurrentMode;
	return 1;
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
	}
	
	return 0;
}


int Matrix_Manager::setProjection(float screenDistence, float aspectRatiox, float aspectRatioy, 
		float centerX, float centerY, float zMin, float zMax, float nearZ, float farZ)
{
	ps2_vu0_view_screen_matrix(*getMatrix(), screenDistence, aspectRatiox, aspectRatioy, centerX,
					centerY, zMin, zMax, nearZ, farZ);
	return 1;
}


int Matrix_Manager::setCamera(vectorf position, vectorf eyeDirection, vectorf verticalDirection, vectorf rotation)
{
	ps2_vu0_rot_camera_matrix(*getMatrix(), position, eyeDirection, verticalDirection, rotation);
	return 1;
}

int Matrix_Manager::rotate(float angle, float rotX, float rotY, float rotZ)
{
	vectorf rotVector = { 0.0f, 0.0f, 0.0f, 0.0f };
	rotVector[0] = rotX * angle;
	rotVector[1] = rotY * angle;
	rotVector[2] = rotZ * angle;

	ps2_vu0_rot_matrix(*getMatrix(), *getMatrix(), rotVector);

	return 1;
}

int Matrix_Manager::rotate(vectorf rotation)
{
	ps2_vu0_rot_matrix(*getMatrix(), *getMatrix(), rotation);

	return 1;
}

int Matrix_Manager::translate(float tranX, float tranY, float tranZ)
{
	vectorf transVector = { 0.0f, 0.0f, 0.0f, 0.0f };
	transVector[0] = tranX;
	transVector[1] = tranY;
	transVector[2] = tranZ;

	ps2_vu0_trans_matrix(*getMatrix(), *getMatrix(), transVector);

	return 1;
}

int Matrix_Manager::translate(vectorf translation)
{
	ps2_vu0_trans_matrix(*getMatrix(), *getMatrix(), translation);

	return 1;
}

int Matrix_Manager::scale(float scaleX, float scaleY, float scaleZ)
{
	vectorf scaleVector = { 0.0f, 0.0f, 0.0f, 0.0f };
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
			fprintf(outputFile, "%20.8f", work[x][y]);
		}
		fprintf(outputFile, "\n");
	}

	fclose(outputFile);

	return 1;
}


