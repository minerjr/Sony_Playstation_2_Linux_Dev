#include "level.h"

Level_Engine::Level_Engine(Matrix_Manager *tmpMatrixManager, Dma_Memory_Engine *tmpDmaMemoryEngine, Texture_Engine *tmpTextureEngine)
{
	latestLevelID = 0;
	currentOutside = NULL;
	currentInside = NULL;
	MatrixManager = tmpMatrixManager;
	DmaMemoryEngine = tmpDmaMemoryEngine;
	TextureEngine = tmpTextureEngine;
}

Level_Engine::~Level_Engine()
{
	if (currentOutside)
	{
		delete currentOutside->levelModel;
		delete currentOutside;
	}
	if (currentInside)
	{
		delete currentInside->q3Level;
		delete currentInside->levelModel;
		delete [] currentInside->clipVertexList;
		delete currentInside;
	}
}

Dma_Packet *Level_Engine::getPacket(int levelID)
{
	if (currentOutside)
		if (currentOutside->levelID == levelID)return currentOutside->levelModel->getModelPacket();
	if (currentInside)
		if (currentInside->levelID == levelID)return currentInside->levelModel->getModelPacket();

	return NULL;
}

int Level_Engine::load(char *fileName)
{

	if (strstr(fileName, ".bsp") != NULL)
	{
		if (currentInside)delete currentInside;
		currentInside = new Inside_Level_Data;
		return buildQ3Level(currentInside, fileName);
	}
	return -1;
}


int Level_Engine::convertQ3Node2CustomModel(int faceIndex)
{
	bspFace *pFace = NULL;
	bspVertex *pVert = NULL;
	int index1, index2, index3;
	int triSend;
	int index;

	//printf("Exit q3modet2custommdoel\n");
	//return 1;	

	if (faceIndex == -1)
	{
		currentInside->levelModel->begin(TRIANGLE, VERTEX_T | COLOR_T | NORMAL_T | ST_T | DONTAPPLYMODELCAMERA_T);
		currentInside->levelModel->setAddChangeValue(61);
		return 2;
	}
	else if (faceIndex == -2)
	{
		currentInside->levelModel->end(); 
		return 2;
	}


	pFace = &currentInside->q3Level->m_pFaces[faceIndex];
	pVert = &currentInside->q3Level->m_pVerts[pFace->startVertIndex];
	currentInside->currentClipIndex = 0;

	//printf("Perform Clipping\n");
	for (int x = 0; x < pFace->numMeshVerts; x+=3)
	{
		index1 = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x].offset;
		index2 = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + 1].offset;
		index3 = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + 2].offset;
		clipQ3ToPlane(&pVert[index1], &pVert[index2], &pVert[index3]);
	}
	
	//printf("Finished Clipping\n");
	



	int size = currentInside->currentClipIndex;
	//printf("Size: %d\n", size);

	for (int y = 0; y < size; y += 60)
	{
		triSend = 60;
		if (y + triSend > size)
			triSend = size % 60;
	
		//Add the vertexs first
		index = y;
		for (int x = 0; x < triSend; x++)
		{
			currentInside->levelModel->addVertex(currentInside->clipVertexList[index + x].position[0], currentInside->clipVertexList[index + x].position[1], currentInside->clipVertexList[index + x].position[2]);
			if (x == 0)
			{
				currentInside->levelModel->setAddChangeValue(triSend + 1);
			}

		}

		index = y;
		//Add the normals second
		for (int x = 0; x < triSend; x++)
		{
			currentInside->levelModel->addNormal(currentInside->clipVertexList[index + x].normal[0], currentInside->clipVertexList[index + x].normal[1], currentInside->clipVertexList[index + x].normal[2]);
		}

		index = y;
		//Add the colors third
		for (int x = 0; x < triSend; x++)
		{
			currentInside->levelModel->addColor(currentInside->clipVertexList[index + x].color[0], currentInside->clipVertexList[index + x].color[1], currentInside->clipVertexList[index + x].color[2], currentInside->clipVertexList[index + x].color[3]);
		}

		index = y;
		//Add the st's last
		for (int x = 0; x < triSend; x++)
		{
			currentInside->levelModel->addST(currentInside->clipVertexList[x].textureCoord[index + x] * 255.0f, currentInside->clipVertexList[index + x].textureCoord[1] * 255.0f);
		}

	}

	return 1;
	
}

int Level_Engine::clipQ3ToPlane(bspVertex *inputVertex1, bspVertex *inputVertex2, bspVertex *inputVertex3)
{
	int outCounter = 0;
	int inCounter = 0;
	int clipTestFlag[3] = {0};
	float testDistence[3];
	float tmpDistence;
	int prevIndex;
	bspVertex *workVertex = inputVertex1;
	bspVertex *prevVertex = inputVertex3;

	vectorf v1, v2, dv, result;
	vectorf transVertex1;
	vectorf transVertex2;
	vectorf transVertex3;
	float *workTransVertex = transVertex1;
	float *prevTransVertex = transVertex3;
	float t = 0.0f;
	matrixf workMatrix;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			workMatrix[x][y] = currentInside->transformMatrix[x][y];
			
		}
		
	}
	

	memcpy(transVertex1, inputVertex1->position, sizeof(float) * 3);
	memcpy(transVertex2, inputVertex2->position, sizeof(float) * 3);
	memcpy(transVertex3, inputVertex3->position, sizeof(float) * 3);
	transVertex1[3] = 1.0f;
	transVertex2[3] = 1.0f;
	transVertex3[3] = 1.0f;

	//printf("Pos X1 %f Y1 %f Z1 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);

	ps2_vu0_apply_matrix(transVertex1, workMatrix, transVertex1);
	ps2_vu0_apply_matrix(transVertex2, workMatrix, transVertex2);
	ps2_vu0_apply_matrix(transVertex3, workMatrix, transVertex3);
	

	//printf("Pos X2 %f Y2 %f Z2 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);
	//printf("Pos X2 %f Y2 %f Z2 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);
	//printf("Pos X2 %f Y2 %f Z2 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);


	for (int x = 0; x < 3; x++)
	{

		testDistence[x] = currentInside->znearPlane[0] * workTransVertex[0] + 
			currentInside->znearPlane[1] * workTransVertex[1] + 
			currentInside->znearPlane[2] * workTransVertex[2] + 
			currentInside->znearPlane[3];

		//printf("Distence = %f\n", testDistence[x]);
		if (testDistence[x] < 0.0f)
		{
			clipTestFlag[x] = -1;
			outCounter++;
		}
		else if (testDistence[x] > 0.0f)
		{
			clipTestFlag[x] = 1;
			inCounter++;
		}
		if (x == 1)workTransVertex = transVertex2;
		else if (x == 2)workTransVertex = transVertex3;
		
	}
	//printf("In Counter = %d Out Counter = %d\n", inCounter, outCounter);
	//printf("Out Counter = %d In Counter = %d\n", outCounter, inCounter);
	if (outCounter == 0)
	{
		currentInside->clipVertexList[currentInside->currentClipIndex] = *inputVertex1;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex].position, transVertex1, sizeof(float) * 3);
		currentInside->clipVertexList[currentInside->currentClipIndex + 1] = *inputVertex2;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 1].position, transVertex2, sizeof(float) * 3);
		currentInside->clipVertexList[currentInside->currentClipIndex + 2] = *inputVertex3;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 2].position, transVertex3, sizeof(float) * 3);
		currentInside->currentClipIndex += 3;
		return 1;
	}
	else if (inCounter == 0)
	{
		//Could delete all for clipping test
		currentInside->clipVertexList[currentInside->currentClipIndex] = *inputVertex1;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex].position, transVertex1, sizeof(float) * 3);
		currentInside->clipVertexList[currentInside->currentClipIndex + 1] = *inputVertex2;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 1].position, transVertex2, sizeof(float) * 3);
		currentInside->clipVertexList[currentInside->currentClipIndex + 2] = *inputVertex3;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 2].position, transVertex3, sizeof(float) * 3);
		currentInside->currentClipIndex += 3;
		return 0;
	}

	prevIndex = 2;
	
	//If there are two points just shorten up the two points to the znear plane
	workTransVertex = transVertex1;
	if (outCounter == 2)
	{

		for (int x = 0; x < 3; x++)
		{
			
			if (clipTestFlag[x] < 0)
			{
				//printf("Enter Actual Clipping\n");
				memcpy(v1, prevTransVertex, sizeof(float) * 3);
				memcpy(v2, workTransVertex, sizeof(float) * 3);
		

				v1[3] = 1.0f;
				v2[3] = 1.0f;

				ps2_vu0_sub_vector(dv, v1, v2);
			
				tmpDistence = currentInside->znearPlane[0] * dv[0] + 
					currentInside->znearPlane[1] * dv[1] + 
					currentInside->znearPlane[2] * dv[2] + 
					currentInside->znearPlane[3];				
				t = testDistence[x] / tmpDistence;

				ps2_vu0_inter_vector_xyz(result, v2, dv, t);
				currentInside->clipVertexList[currentInside->currentClipIndex] = *workVertex;
				currentInside->clipVertexList[currentInside->currentClipIndex].position[0] = result[0];
				currentInside->clipVertexList[currentInside->currentClipIndex].position[1] = result[1];
				currentInside->clipVertexList[currentInside->currentClipIndex].position[2] = result[2];
				
				v1[0] = prevVertex->textureCoord[0];
				v1[1] = prevVertex->textureCoord[1];
				v1[2] = 1.0f;
				v1[3] = 1.0f;

				v2[0] = workVertex->textureCoord[0];
				v2[1] = workVertex->textureCoord[1];
				v2[2] = 1.0f;
				v2[3] = 1.0f;

				ps2_vu0_inter_vector_xyz(result, v2, v1, t);

				currentInside->clipVertexList[currentInside->currentClipIndex].textureCoord[0] = result[0];
				currentInside->clipVertexList[currentInside->currentClipIndex].textureCoord[1] = result[1];
				currentInside->currentClipIndex++;


			}
			else
			
			{
				currentInside->clipVertexList[currentInside->currentClipIndex] = *workVertex;
				memcpy(currentInside->clipVertexList[currentInside->currentClipIndex].position, workTransVertex, sizeof(float) * 3);
				currentInside->currentClipIndex++;
			}

			prevIndex = x;
			prevVertex = workVertex;
			prevTransVertex = workTransVertex;
			if (x == 1)
			{
				workVertex = inputVertex2;
				workTransVertex = transVertex2;
			}
			else if (x == 2)
			{
				workVertex = inputVertex3;
				workTransVertex = transVertex3;
			}
			
		}
	}
	//Else inCounter = 1, then generate two points
	else
	{
		for (int x = 0; x < 3; x++)
		{
			/*
			if (clipTestFlag[x] < 0)
			{

			}
			else
			*/
			{
				currentInside->clipVertexList[currentInside->currentClipIndex] = *workVertex;
				memcpy(currentInside->clipVertexList[currentInside->currentClipIndex].position, workTransVertex, sizeof(float) * 3);
				currentInside->currentClipIndex++;
			}

			prevIndex = x;
			prevVertex = workVertex;
			prevTransVertex = workTransVertex;
			if (x == 1)
			{
				workVertex = inputVertex2;
				workTransVertex = transVertex2;
			}
			else if (x == 2)
			{
				workVertex = inputVertex3;
				workTransVertex = transVertex3;
			}
			
		}
	}



	
	


	return 1;
}

int Level_Engine::buildPlane(float znear)
{
	vectorf V1;
	vectorf V2;
	vectorf V3;
	vectorf result1;
	vectorf result2;
	vectorf normal;
	

	V1[0] = 10.0f;
	V1[1] = 10.0f;
	V1[2] = znear;
	V1[3] = 1.0f;

	V2[0] = 5.0f;
	V2[1] = 0.0f;
	V2[2] = znear;
	V2[3] = 1.0f;


	V3[0] = -10.0f;
	V3[1] = -10.0f;
	V3[2] = znear;
	V3[3] = 1.0f;

	ps2_vu0_sub_vector(result1, V3, V2);
	ps2_vu0_sub_vector(result2, V1, V2);

	ps2_vu0_outer_product(normal, result1, result2);

	ps2_vu0_normalize(normal, normal);
	
	currentInside->znearPlane[0] = normal[0];
	currentInside->znearPlane[1] = normal[1];
	currentInside->znearPlane[2] = normal[2];

	currentInside->znearPlane[3] = -ps2_vu0_inner_product(normal, V1);


	return 1;
}

int Level_Engine::buildQ3Level(Inside_Level_Data *newInside, char *fileName)
{

	newInside->levelID = latestLevelID;
	latestLevelID++;
	newInside->q3Level = new Q3BSP_Loader(fileName);

	newInside->levelModel = new Custom_Model(MatrixManager, 1, DmaMemoryEngine, TextureEngine);

	newInside->currentClipIndex = 0;
	newInside->maxNumberOfClipVertexs = 0;
	//printf("Number of q3level faces: %d\n",newInside->q3Level->numberOfFaces); 
	for (int x = 0; x < newInside->q3Level->numberOfFaces; x++)
	{
		if (newInside->q3Level->m_pFaces[x].numMeshVerts > newInside->maxNumberOfClipVertexs)
		{
			newInside->maxNumberOfClipVertexs = newInside->q3Level->m_pFaces[x].numMeshVerts;
		}
	}

	buildPlane(-32.0f);

	//printf("Plane X %f Y %f Z %f D %f\n", currentInside->znearPlane[0], currentInside->znearPlane[1], currentInside->znearPlane[2], currentInside->znearPlane[3]);
	//Make a buffer for the clipped vertexs to be twice the most verts there are in
	//faces stored in the quake 3 level file.
	newInside->maxNumberOfClipVertexs *= 2;
	newInside->clipVertexList = new bspVertex[newInside->maxNumberOfClipVertexs];

	//printf("Finlished making clipVertexList\n");
	updateQ3Level();

	return newInside->levelID;
}

int Level_Engine::updateLevel(int levelID)
{
	if (currentInside)
		if (currentInside->levelID == levelID)return updateQ3Level();
	
	if (currentOutside)
		if (currentOutside->levelID == levelID)return 1;
	

	return 1;
}

int Level_Engine::updateQ3Level()
{
	
	if (currentInside == NULL)return -1;
	matrixf workMatrix;

	
 	//Get the model view matrix and store it
	int oldMode = MatrixManager->setMode(MODELVIEW);
	workMatrix = *MatrixManager->getMatrix();

	//Get the camera matrix and apply it to the model view matrix and store it.
	MatrixManager->setMode(CAMERA);
	
	ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);

	MatrixManager->setMode(oldMode);

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			currentInside->transformMatrix[x][y] = workMatrix[x][y];
		}
		
	}

	

	int x = currentInside->q3Level->numberOfFaces;

	currentInside->q3Level->facesDrawn.ClearAll();
	convertQ3Node2CustomModel(-1);
	while(x--)
	{
		if (x <= -1)break;
		if (currentInside->q3Level->m_pFaces[x].type != FACE_POLYGON)continue;
		if (!currentInside->q3Level->facesDrawn.On(x))
		{
			currentInside->q3Level->facesDrawn.Set(x);
			convertQ3Node2CustomModel(x);
		}
	}
	convertQ3Node2CustomModel(-2);


	return 1;
}

int Level_Engine::buildDisplayList(int levelID)
{
	return 1;
}

