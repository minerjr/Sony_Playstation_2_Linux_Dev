#include "keyframe.h"
#include "matrix.h"
#include "md2file.h"
#include "perftime.h"
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

Keyframe_Model_Engine::Keyframe_Model_Engine(Matrix_Manager *tmpMatrixManager, Dma_Memory_Engine *tmpDmaMemoryEngine, Texture_Engine *tmpTextureEngine)
{
	MatrixManager = tmpMatrixManager;
	DmaMemoryEngine = tmpDmaMemoryEngine;
	TextureEngine = tmpTextureEngine;
	characterHead = NULL;
	modelHead = NULL;
	latestModelID = 0;
	latestCharacterID = 0;
}

Keyframe_Model_Engine::~Keyframe_Model_Engine()
{

	Keyframe_Model_Data *workModel = modelHead;
	Keyframe_Model_Data *delModel = modelHead;
	
	Keyframe_Model_Animation_Data *workAnimation = NULL;
	Keyframe_Model_Animation_Data *delAnimation = NULL;
	


	Keyframe_Character_Data *workCharacter = characterHead;
	Keyframe_Character_Data *delCharacter = characterHead;


	while(workCharacter != NULL)
	{
		delCharacter = workCharacter;
		workCharacter = workCharacter->next;
		delete delCharacter->firstModel;
		delete delCharacter->secondModel;
		delete delCharacter;
	}

	workModel = modelHead;
	while (workModel != NULL)
	{
		delModel = workModel;
		workModel = workModel->next;
		
		workAnimation = delModel->animationHead;
		while(workAnimation != NULL)
		{
			delAnimation = workAnimation;
			workAnimation = workAnimation->next;
			delete delAnimation;
		}
		delete [] delModel->stData;
		delete [] delModel->triData;
		delete [] delModel->vertexData;
		delete delModel;
	}
	
}


//Adds a model the link list of models, returns -1 if there was an error
int Keyframe_Model_Engine::addModel(char *fileName)
{

	if (strstr(fileName, ".md2") != NULL)
	{
		return loadMD2(fileName);
	}
	else if (strstr(fileName, ".ckm") != NULL)
	{
		return loadCKM(fileName);
	}
	
	return -1;
}

int Keyframe_Model_Engine::loadMD2(char *fileName)
{
	char currentName[16];
	char *frameName;
	Md2File *md2Input = NULL;

	int workAnimationID = 0;
	int workFrameID = 0;
	Keyframe_Model_Data *newModel;
	Keyframe_Model_Data *workModel;
	Keyframe_Model_Animation_Data *newAnimation = NULL;
	Keyframe_Model_Animation_Data *workAnimation = NULL;

	Keyframe_Model_Vertex_Data *workVertex = NULL;

	md2Input = new Md2File(fileName);
	if (md2Input == NULL)return -1;

	newModel = new Keyframe_Model_Data;
	
	strcpy(currentName, "");

	newModel->modelID = latestModelID;
	latestModelID++;
		
	newModel->next = NULL;
	newModel->animationHead = NULL;
	newModel->numberOfFrames = md2Input->m_Header.numFrames;
	newModel->numberOfTriangles = md2Input->m_Header.numTriangles;
	newModel->numberOfST = md2Input->m_Header.numTexCoords;
	newModel->numberOfVerticies = md2Input->m_Header.numVertices;

	newModel->stData = new Keyframe_Model_ST_Data[newModel->numberOfST];
	newModel->triData = new Keyframe_Model_Mesh_Data[newModel->numberOfTriangles];
	newModel->vertexData = new Keyframe_Model_Vertex_Data[newModel->numberOfFrames *
		newModel->numberOfVerticies];
	

	for (int x = 0; x < newModel->numberOfST; x++)
	{
		newModel->stData[x].s = (float)md2Input->m_pTexCoords[x].u / (float)md2Input->m_Header.skinWidth;
		newModel->stData[x].t = (float)md2Input->m_pTexCoords[x].v / (float)md2Input->m_Header.skinHeight;
	}

	for (int x = 0; x < newModel->numberOfTriangles; x++)
	{
		newModel->triData[x].meshIndex[0] = md2Input->m_pTriangles[x].vertexIndices[0];
		newModel->triData[x].meshIndex[1] = md2Input->m_pTriangles[x].vertexIndices[1];
		newModel->triData[x].meshIndex[2] = md2Input->m_pTriangles[x].vertexIndices[2];

		newModel->triData[x].stIndex[0] = md2Input->m_pTriangles[x].textureIndices[0];
		newModel->triData[x].stIndex[1] = md2Input->m_pTriangles[x].textureIndices[1];
		newModel->triData[x].stIndex[2] = md2Input->m_pTriangles[x].textureIndices[2];
		
	}


	workVertex = newModel->vertexData;
	
	for (int x = 0; x < newModel->numberOfFrames; x++)
	{
		workVertex = &newModel->vertexData[newModel->numberOfVerticies * x];
		for (int y = 0; y < newModel->numberOfVerticies; y++)
		{
			workVertex[y].x = md2Input->m_pFrames[x].pVertices[y].vertex[0];
			workVertex[y].y = md2Input->m_pFrames[x].pVertices[y].vertex[1];
			workVertex[y].z = md2Input->m_pFrames[x].pVertices[y].vertex[2];
		}
	}


	//Build animation sets;
	for (int x = 0; x < newModel->numberOfFrames; x++)
	{
		printf("Frame Name: %s\n", md2Input->m_pFrames[x].strName);
		frameName = strtok(md2Input->m_pFrames[x].strName, "0123456789");
	 	if (strcmp(frameName, currentName) != 0)
		{
			strcpy(currentName, frameName);
			if (newModel->animationHead == NULL)
			{
				newAnimation = new Keyframe_Model_Animation_Data;
				
				newAnimation->animationID = workAnimationID;
				workAnimationID++;
				newAnimation->next = NULL;
				newAnimation->startIndex = workFrameID;
				workFrameID++;
				newAnimation->endIndex = -1;
				
				newModel->animationHead = newAnimation;
			}
			else
			{
				newAnimation->endIndex = workFrameID - 1;
				newAnimation = new Keyframe_Model_Animation_Data;

				workAnimation = newModel->animationHead;
				while(workAnimation->next != NULL)
					workAnimation = workAnimation->next;
				
				workAnimation->next = newAnimation;
				newAnimation->startIndex = workFrameID;
				workFrameID++;
				newAnimation->animationID = workAnimationID;
				workAnimationID++;
				newAnimation->next = NULL;
				newAnimation->endIndex = -1;
				
			}
		}
		else
		{
			workFrameID++;
		}
		
	}

	//printf("Done MD2 load\n");
	if (newAnimation->endIndex == -1)newAnimation->endIndex = workFrameID - 1;
	
	
	workModel = modelHead;
	if (modelHead == NULL)
	{
		modelHead = newModel;
	}
	else
	{
		while(workModel->next != NULL)
		{
			workModel = workModel->next;
		}
		workModel->next = newModel;
	}

	delete md2Input;
	return newModel->modelID;
}


int Keyframe_Model_Engine::loadCKM(char *fileName)
{
	return 1;
}


int Keyframe_Model_Engine::buildCharacter(int modelID, int textureID)
{
	Keyframe_Model_Data *workModel = modelHead;

	Custom_Model *makeModel = NULL;
	Keyframe_Character_Data *newCharacter = NULL;
	Keyframe_Character_Data *workCharacter = NULL;

	Keyframe_Model_Vertex_Data *workVertex;
	vectorf normalVec;
	vectorf firstVec;
	vectorf secondVec;
	vectorf thirdVec;

	vectorf result1Vec;
	vectorf result2Vec;
	vectorf normals[20];

	int index = 0;

	//Find the model to load the frames from
	while(workModel != NULL)
	{
		if (workModel->modelID == modelID)
			break;
		workModel = workModel->next;
	}
	if (workModel == NULL)return -1;

	newCharacter = new Keyframe_Character_Data;
	if (newCharacter == NULL)return -1;

	printf("Build Character\n");
	newCharacter->currentAnimationID = 0;
	newCharacter->currentFrameID = 0;
	newCharacter->currentFrameChange = 0.0f;
	newCharacter->firstModel = new Custom_Model(MatrixManager, 1, DmaMemoryEngine, TextureEngine);
	newCharacter->secondModel = new Custom_Model(MatrixManager, 1, DmaMemoryEngine, TextureEngine);
	newCharacter->currentModifyPacket = newCharacter->firstModel->getModelPacket();
	newCharacter->currentSendPacket = newCharacter->secondModel->getModelPacket();
	newCharacter->currentModifyPacket->setSend(0);
	newCharacter->currentSendPacket->setSend(1);
	newCharacter->textureID = textureID;
	newCharacter->firstModel->setTextureID(newCharacter->textureID);
	newCharacter->secondModel->setTextureID(newCharacter->textureID);
	poll_counters();
	newCharacter->lastUpdateTime = read_counter0();
	newCharacter->next = NULL;
	newCharacter->modelID = modelID;
	newCharacter->characterID = latestCharacterID;
	printf("Character ID = %d\n", latestCharacterID);
	latestCharacterID++;

	//Add the character to the link list of characters
	if (characterHead == NULL)
	{
		characterHead = newCharacter;
	}
	else
	{
		workCharacter = characterHead;
		while(workCharacter->next != NULL)
			workCharacter = workCharacter->next;
		workCharacter->next = newCharacter;
	}


	

	for (int x = 0; x < 2; x++)
	{
		if (x == 0)makeModel = newCharacter->firstModel;
		else makeModel = newCharacter->secondModel;
		
		printf("Begin Make Model:\n");
		

		
		makeModel->begin(TRIANGLE, VERTEX_T | COLOR_T | NORMAL_T | ST_T | NORESET_T);
		{
			

			//int test = 0;
			int size = workModel->numberOfTriangles;
			printf("Ok\n");
			//for (y = 0; y < 180; y += 60)
			workVertex = workModel->vertexData;
			for (int y = 0; y < size; y += 20)
			{
				int triSend = 20;
				if (y + triSend > size)
					triSend = size % 20;

				//printf("Count #%d\n", test);
				//test++;

				//Must add vertexs first
				index = y;
				for (int z = 0; z < triSend; z++)
				{
					
					//printf("V1 %f V2 %f V3 %f\n", workFrame->vertexData[index], workFrame->vertexData[index + 1], workFrame->vertexData[index + 2]);
					//makeModel->addVertex(workFrame->vertexData[index], workFrame->vertexData[index + 1], workFrame->vertexData[index + 2]);
					
					firstVec[0] = workVertex[workModel->triData[index].meshIndex[0]].x;
					firstVec[1] = workVertex[workModel->triData[index].meshIndex[0]].y;
					firstVec[2] = workVertex[workModel->triData[index].meshIndex[0]].z;


					secondVec[0] = workVertex[workModel->triData[index].meshIndex[2]].x;
					secondVec[1] = workVertex[workModel->triData[index].meshIndex[2]].y;
					secondVec[2] = workVertex[workModel->triData[index].meshIndex[2]].z;


					thirdVec[0] = workVertex[workModel->triData[index].meshIndex[1]].x;
					thirdVec[1] = workVertex[workModel->triData[index].meshIndex[1]].y;
					thirdVec[2] = workVertex[workModel->triData[index].meshIndex[1]].z;


					makeModel->addVertex(firstVec[0], firstVec[1], firstVec[2]);
					makeModel->addVertex(secondVec[0], secondVec[1], secondVec[2]);
					makeModel->addVertex(thirdVec[0], thirdVec[1], thirdVec[2]);
					
					ps2_vu0_sub_vector(result1Vec, thirdVec, firstVec);
					ps2_vu0_sub_vector(result2Vec, secondVec, firstVec);


					ps2_vu0_outer_product(normalVec, result1Vec, result2Vec);

					ps2_vu0_normalize(normals[z], normalVec);

					index++;
				}

				//Then the normals
				index = y;
				for (int z = 0; z < triSend; z++)
				{
					//printf("N1 %f N2 %f N3 %f\n", workFrame->normalData[index], workFrame->normalData[index + 1], workFrame->normalData[index + 2]);
					//makeModel->addNormal(workFrame->normalData[index], workFrame->normalData[index + 1], workFrame->normalData[index + 2]);
					makeModel->addNormal(normals[z][0], normals[z][1], normals[z][2] );
					makeModel->addNormal(normals[z][0], normals[z][1], normals[z][2] );
					makeModel->addNormal(normals[z][0], normals[z][1], normals[z][2] );

					index++;
				}

				//Then the colors
				for (int z = 0; z < triSend; z++)
				{
					makeModel->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					makeModel->addColor(1.0f, 1.0f, 1.0f, 1.0f);
					makeModel->addColor(1.0f, 1.0f, 1.0f, 1.0f);
				}
		
				//Finally the st's
				index = y;
				for (int z = 0; z < triSend; z++)
				{
					//makeModel->addST(workModel->stData[index], workModel->stData[index + 1]);
					makeModel->addST(workModel->stData[workModel->triData[index].stIndex[0]].s, workModel->stData[workModel->triData[index].stIndex[0]].t);
					makeModel->addST(workModel->stData[workModel->triData[index].stIndex[2]].s, workModel->stData[workModel->triData[index].stIndex[2]].t);
					makeModel->addST(workModel->stData[workModel->triData[index].stIndex[1]].s, workModel->stData[workModel->triData[index].stIndex[1]].t);
					//makeModel->addST(0.0f, 0.0f);
					//makeModel->addST(0.0f, 0.0f);
					//makeModel->addST(0.0f, 0.0f);
					index++;
				}
			}

			
		}
		
		//For every begin, there must be an end call
		makeModel->end();
		
	}

	return newCharacter->characterID;
}




int Keyframe_Model_Engine::removeModel(int modelID)
{
/*
	Keyframe_Model_Data *delModel;
	Keyframe_Model_Data *workModel;

	Keyframe_Model_Animation_Data *workAnimation = NULL;
	Keyframe_Model_Animation_Data *delAnimation = NULL;
	
	Keyframe_Model_Frame_Data *workFrame = NULL;
	Keyframe_Model_Frame_Data *delFrame = NULL;

	workModel = modelHead;

	if (modelHead->modelID == modelID)
	{
		delModel = modelHead;
		modelHead = modelHead->next;
		
		workAnimation = delModel->head;
		while(workAnimation != NULL)
		{
			//printf("Deleting 2\n");
			delAnimation = workAnimation;
			workAnimation = workAnimation->next;

			workFrame = delAnimation->head;
			while(workFrame != NULL)
			{
				//printf("Deleting 3\n");
				delFrame = workFrame;
				workFrame = workFrame->next;
				delete [] delFrame->vertexData;
				delete [] delFrame->normalData;
				delete delFrame;
			}

			delete delAnimation;
		}


		//delete delModel->firstModel;
		//delete delModel->secondModel;
		delete [] delModel->stData;
		delete delModel;
	}
	else
	{
		workModel = modelHead;

		while(workModel != NULL)
		{
			if (workModel->next != NULL)
			{
				if (workModel->next->modelID == modelID)
				{
					delModel = workModel->next;
					workModel->next = delModel->next;

					workAnimation = delModel->head;
					while(workAnimation != NULL)
					{
						//printf("Deleting 2\n");
						delAnimation = workAnimation;
						workAnimation = workAnimation->next;

						workFrame = delAnimation->head;
						while(workFrame != NULL)
						{
							//printf("Deleting 3\n");
							delFrame = workFrame;
							workFrame = workFrame->next;
							delete [] delFrame->vertexData;
							delete [] delFrame->normalData;
							delete delFrame;
						}

						delete delAnimation;
					}

					//delete delModel->firstModel;
					//delete delModel->secondModel;
					delete delModel->stData;
					delete delModel;
				}

			}
			workModel = workModel->next;

		}
	}
*/
	return 1;
}

int Keyframe_Model_Engine::runAnimation(int characterID, int animationID)
{

	Keyframe_Model_Data *workModel = modelHead;
	Keyframe_Character_Data *workCharacter = characterHead;
	vectorf firstFrameVec;
	vectorf secondFrameVec;

	vectorf normalVec;
	vectorf firstVec;
	vectorf secondVec;
	vectorf thirdVec;

	vectorf result1Vec;
	vectorf result2Vec;
	vectorf normals[20];
	Keyframe_Model_Animation_Data *workAnimation = NULL;
	Keyframe_Model_Vertex_Data *workVertex1;
	Keyframe_Model_Vertex_Data *workVertex2;
	int oldMode;
	matrixf workMatrix;
	int timeDifference = 0;
	int firstFrameID = -1;
	int secondFrameID = -1;
	int size = 0;	
	int index = 0;

	while (workCharacter != NULL)
	{
		if (workCharacter->characterID == characterID)break;
		workCharacter = workCharacter->next;
	}

	if (workCharacter == NULL)return -1;

	while(workModel != NULL)
	{
		if (workModel->modelID == workCharacter->modelID)break;
		workModel = workModel->next;
	}
	
	if (workModel == NULL)return -1;


	if (animationID != workCharacter->currentAnimationID)
	{
		workCharacter->currentAnimationID = animationID;
		workCharacter->currentFrameID = -1;
		workCharacter->currentFrameChange = 0.0f;
	}

	workAnimation = workModel->animationHead;

	while(workAnimation != NULL)
	{
		if (workAnimation->animationID == workCharacter->currentAnimationID)break;
		workAnimation = workAnimation->next;
	}

	if (workAnimation == NULL)return -1;


	if (workCharacter->currentFrameID == -1)
	{
		workCharacter->currentFrameID = workAnimation->startIndex;
	}


	poll_counters();
	timeDifference = (int)((read_counter0() - workCharacter->lastUpdateTime) / 1000);
	workCharacter->lastUpdateTime = read_counter0();

	//printf("timeDifference = %d %u\n", timeDifference, workModel->lastUpdateTime);
	workCharacter->currentFrameChange += 0.00002f * (float)timeDifference;
	//printf("Current Frame Change = %f\n", workModel->currentFrameChange);

	if (workCharacter->currentFrameChange > 1.0f)
	{
		workCharacter->currentFrameChange = 0.0f;
		workCharacter->currentFrameID++;
	}

	firstFrameID = workCharacter->currentFrameID;
	if (firstFrameID > workAnimation->endIndex)
	{
		workCharacter->currentFrameID = firstFrameID = workAnimation->startIndex;
	}
	secondFrameID = firstFrameID + 1;
	if (secondFrameID > workAnimation->endIndex)
		secondFrameID = workAnimation->startIndex;


	workCharacter->currentModifyPacket->updateReset();

	//Update the current matrix in the modify 

 	//Get the model view matrix and store it
	oldMode = MatrixManager->setMode(MODELVIEW);
	workMatrix = *MatrixManager->getMatrix();

	/*
	//Get the camera matrix and apply it to the model view matrix and store it.
	MatrixManager->setMode(CAMERA);
	
	ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
	*/
	//Multiply the projection onto the workMatrix
	MatrixManager->setMode(PROJECTION);
	ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
	
	//Add the work screen (view * projection) matrix to the packet
	workCharacter->currentModifyPacket->update((uint128 *)&workMatrix, 4);

	//Do the light matrix

	//Get the model view matrix and store it
	MatrixManager->setMode(MODELVIEW);
	workMatrix = *MatrixManager->getMatrix();

	/*
	//Get the camera matrix and apply it to the model view matrix and store it.
	MatrixManager->setMode(CAMERA);
	ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
	*/
	//Remove the tanslation from the matrix, just need the rotations
	workMatrix[3][0] = 0.0f;
	workMatrix[3][1] = 0.0f;
	workMatrix[3][2] = 0.0f;

	//Now apply the world view matrix to the light matrix and store it
	MatrixManager->setMode(LIGHT);
	ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
	//workMatrix = *MatrixManager->getMatrix();


	workCharacter->currentModifyPacket->update((uint128 *)&workMatrix, 4);

	//Add the light color matrix

	//Get the model view matrix and store it
	MatrixManager->setMode(COLOR);
	workMatrix = *MatrixManager->getMatrix();

	//Add the work color matrix to the packet
	//ps2_vu0_transpose_matrix(workMatrix, workMatrix);
	workCharacter->currentModifyPacket->update((uint128 *)&workMatrix, 4);


	//Set the matrix manager back to the orginal mode
	MatrixManager->setMode(oldMode);

	
	workCharacter->currentModifyPacket->updateMove(13);



	
	size = workModel->numberOfTriangles;
	
	
	workVertex1 = &workModel->vertexData[workModel->numberOfVerticies * firstFrameID];
	workVertex2 = &workModel->vertexData[workModel->numberOfVerticies * secondFrameID];
	if (firstFrameID == secondFrameID)
	{
		for (int y = 0; y < size; y += 20)
		{
			int triSend = 20;
			if (y + triSend > size)
				triSend = size % 20;


			//Must add vertexs first
			index = y;
			for (int z = 0; z < triSend; z++)
			{
				
				//printf("V1 %f V2 %f V3 %f\n", workFrame->vertexData[index], workFrame->vertexData[index + 1], workFrame->vertexData[index + 2]);
				firstFrameVec[0] = workVertex1[workModel->triData[index].meshIndex[0]].x;
				firstFrameVec[1] = workVertex1[workModel->triData[index].meshIndex[0]].y;
				firstFrameVec[2] = workVertex1[workModel->triData[index].meshIndex[0]].z;

				workCharacter->currentModifyPacket->update((uint32 *)&firstFrameVec[0],3);			

				firstFrameVec[0] = workVertex1[workModel->triData[index].meshIndex[2]].x;
				firstFrameVec[1] = workVertex1[workModel->triData[index].meshIndex[2]].y;
				firstFrameVec[2] = workVertex1[workModel->triData[index].meshIndex[2]].z;

				workCharacter->currentModifyPacket->update((uint32 *)&firstFrameVec[0],3);			

				firstFrameVec[0] = workVertex1[workModel->triData[index].meshIndex[1]].x;
				firstFrameVec[1] = workVertex1[workModel->triData[index].meshIndex[1]].y;
				firstFrameVec[2] = workVertex1[workModel->triData[index].meshIndex[1]].z;

				workCharacter->currentModifyPacket->update((uint32 *)&firstFrameVec[0],3);			
				
				index++;
			}
			
			workCharacter->currentModifyPacket->updateMove(2);
			//Then the normals
			index = y;
			for (int z = 0; z < triSend; z++)
			{
				//printf("N1 %f N2 %f N3 %f\n", workFrame->normalData[index], workFrame->normalData[index + 1], workFrame->normalData[index + 2]);
				workCharacter->currentModifyPacket->update((uint32)0.0f);
				workCharacter->currentModifyPacket->update((uint32)0.0f);
				workCharacter->currentModifyPacket->update((uint32)1.0f);
				
				workCharacter->currentModifyPacket->update((uint32)0.0f);
				workCharacter->currentModifyPacket->update((uint32)0.0f);
				workCharacter->currentModifyPacket->update((uint32)1.0f);

				
				workCharacter->currentModifyPacket->update((uint32)0.0f);
				workCharacter->currentModifyPacket->update((uint32)0.0f);
				workCharacter->currentModifyPacket->update((uint32)1.0f);

				index++;
			}

			workCharacter->currentModifyPacket->updateMove(2 + triSend * 3 * 4 + 2 + triSend * 3 * 2 + 9);
		
		}
	}
	else
	{
		for (int y = 0; y < size; y += 20)
		{
			int triSend = 20;
			if (y + triSend > size)
				triSend = size % 20;


			//Must add vertexs first
			index = y;
			for (int z = 0; z < triSend; z++)
			{
				
				
				firstFrameVec[0] = workVertex1[workModel->triData[index].meshIndex[0]].x;
				firstFrameVec[1] = workVertex1[workModel->triData[index].meshIndex[0]].y;
				firstFrameVec[2] = workVertex1[workModel->triData[index].meshIndex[0]].z;

				secondFrameVec[0] = workVertex2[workModel->triData[index].meshIndex[0]].x;
				secondFrameVec[1] = workVertex2[workModel->triData[index].meshIndex[0]].y;
				secondFrameVec[2] = workVertex2[workModel->triData[index].meshIndex[0]].z;
				
				ps2_vu0_inter_vector_xyz(firstFrameVec, secondFrameVec, firstFrameVec, workCharacter->currentFrameChange);

				memcpy(&firstVec, &firstFrameVec, sizeof(float) * 3);

				workCharacter->currentModifyPacket->update((uint32 *)&firstFrameVec[0],3);			

				firstFrameVec[0] = workVertex1[workModel->triData[index].meshIndex[2]].x;
				firstFrameVec[1] = workVertex1[workModel->triData[index].meshIndex[2]].y;
				firstFrameVec[2] = workVertex1[workModel->triData[index].meshIndex[2]].z;

				secondFrameVec[0] = workVertex2[workModel->triData[index].meshIndex[2]].x;
				secondFrameVec[1] = workVertex2[workModel->triData[index].meshIndex[2]].y;
				secondFrameVec[2] = workVertex2[workModel->triData[index].meshIndex[2]].z;
				
				ps2_vu0_inter_vector_xyz(firstFrameVec, secondFrameVec, firstFrameVec, workCharacter->currentFrameChange);

				memcpy(&thirdVec, &firstFrameVec, sizeof(float) * 3);

				workCharacter->currentModifyPacket->update((uint32 *)&firstFrameVec[0],3);			

				firstFrameVec[0] = workVertex1[workModel->triData[index].meshIndex[1]].x;
				firstFrameVec[1] = workVertex1[workModel->triData[index].meshIndex[1]].y;
				firstFrameVec[2] = workVertex1[workModel->triData[index].meshIndex[1]].z;

				secondFrameVec[0] = workVertex2[workModel->triData[index].meshIndex[1]].x;
				secondFrameVec[1] = workVertex2[workModel->triData[index].meshIndex[1]].y;
				secondFrameVec[2] = workVertex2[workModel->triData[index].meshIndex[1]].z;
				
				ps2_vu0_inter_vector_xyz(firstFrameVec, secondFrameVec, firstFrameVec, workCharacter->currentFrameChange);
				memcpy(&secondVec, &firstFrameVec, sizeof(float) * 3);

				workCharacter->currentModifyPacket->update((uint32 *)&firstFrameVec[0],3);			
				
				ps2_vu0_sub_vector(result1Vec, thirdVec, firstVec);
				ps2_vu0_sub_vector(result2Vec, secondVec, firstVec);


				ps2_vu0_outer_product(normalVec, result1Vec, result2Vec);

				ps2_vu0_normalize(normals[z], normalVec);				

				index++;
			}
			
			workCharacter->currentModifyPacket->updateMove(2);
			//Then the normals
			index = y;
			for (int z = 0; z < triSend; z++)
			{
				workCharacter->currentModifyPacket->update((uint32 *)normals[z], 3 );
				workCharacter->currentModifyPacket->update((uint32 *)normals[z], 3 );
				workCharacter->currentModifyPacket->update((uint32 *)normals[z], 3 );

				index++;
			}

			workCharacter->currentModifyPacket->updateMove(2 + triSend * 3 * 4 + 2 + triSend * 3 * 2 + 9);
		
		}
	}

			
			
	return 1;

}


	
int Keyframe_Model_Engine::resetAnimation(int characterID, int animationID)
{
	return 1;
}

int Keyframe_Model_Engine::flipSendPacket(int characterID)
{
	Keyframe_Character_Data *workCharacter = characterHead;

	while(workCharacter != NULL)
	{
		if (workCharacter->characterID == characterID)
			break;
		workCharacter = workCharacter->next;
	}

	if(workCharacter == NULL)return 0;

	if (workCharacter->currentSendPacket == workCharacter->firstModel->getModelPacket())
	{
		workCharacter->currentSendPacket->setSend(0);
		workCharacter->currentModifyPacket->setSend(1);
		workCharacter->currentSendPacket = workCharacter->secondModel->getModelPacket();
		workCharacter->currentModifyPacket = workCharacter->firstModel->getModelPacket();
	}
	else
	{
		workCharacter->currentSendPacket->setSend(1);
		workCharacter->currentModifyPacket->setSend(0);
		workCharacter->currentSendPacket = workCharacter->firstModel->getModelPacket();
		workCharacter->currentModifyPacket = workCharacter->secondModel->getModelPacket();
	}
	
	return 1;
}

Dma_Packet *Keyframe_Model_Engine::getFirstPacket(int characterID)
{
	Keyframe_Character_Data *workCharacter = characterHead;
	while(workCharacter != NULL)
	{
		if (workCharacter->characterID == characterID)
			break;
		workCharacter = workCharacter->next;
	}
	if (workCharacter == NULL)return NULL;
	return workCharacter->firstModel->getModelPacket();
}

Dma_Packet *Keyframe_Model_Engine::getSecondPacket(int characterID)
{
	Keyframe_Character_Data *workCharacter = characterHead;
	while(workCharacter != NULL)
	{
		if (workCharacter->characterID == characterID)
			break;
		workCharacter = workCharacter->next;
	}
	if (workCharacter == NULL)return NULL;
	return workCharacter->secondModel->getModelPacket();
}

int Keyframe_Model_Engine::getTextureID(int modelID)
{
	return 1;
}

int Keyframe_Model_Engine::getNumberOfAnimations(int characterID)
{
	int numAnimations = 0;
	Keyframe_Character_Data *workCharacter = characterHead;
	Keyframe_Model_Data *workModel = modelHead;
	Keyframe_Model_Animation_Data *workAnimation = NULL;

	while(workCharacter != NULL)
	{
		if(workCharacter->characterID == characterID)
			break;
		workCharacter = workCharacter->next;
	}

	if (workCharacter == NULL)return -1;

	while (workModel != NULL)
	{
		if (workModel->modelID == workCharacter->modelID)
			break;
		workModel = workModel->next;
	}

	if (workModel == NULL)return -1;

	workAnimation = workModel->animationHead;

	while(workAnimation != NULL)
	{
		numAnimations++;
		workAnimation = workAnimation->next;
	}

	return numAnimations;
}

