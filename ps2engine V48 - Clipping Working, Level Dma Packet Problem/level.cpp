#include "level.h"
#include <assert.h>

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

/*

//Clipping Version Does not work
int Level_Engine::convertQ3Node2CustomModel(int faceIndex)
{

	//FILE *testFile = fopen("/root/test2.txt", "a+");
	bspFace *pFace = NULL;
	bspVertex *pVert = NULL;
	int index1, index2, index3;
	int triSend;
	int index;
	int numberOfVertexs = 0;

	//printf("Face #%d\n", faceIndex);
	//printf("Exit q3modet2custommdoel\n");
	//return 1;	

	if (faceIndex == -1)
	{
		currentInside->levelModel->begin(TRIANGLE, VERTEX_T | COLOR_T | NORMAL_T | ST_T | DONTAPPLYMODELCAMERA_T);
		currentInside->levelModel->setAddChangeValue(61);
		return 0;
	}
	else if (faceIndex == -2)
	{
		currentInside->levelModel->end(); 
		return 0;
	}

	//memset(currentInside->clipVertexList, 0,  sizeof(bspVertex) * currentInside->maxNumberOfClipVertexs);


	pFace = &currentInside->q3Level->m_pFaces[faceIndex];
	pVert = &currentInside->q3Level->m_pVerts[pFace->startVertIndex];
	//printf("Number Of Vertexs: %d\n", pFace->numMeshVerts);
	currentInside->currentClipIndex = 0;
	
	//fprintf(testFile, "Perform Clipping\n");
	for (int x = 0; x < pFace->numMeshVerts; x+=3)
	{
		index1 = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x].offset;
		index2 = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + 1].offset;
		index3 = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + 2].offset;
		numberOfVertexs += clipQ3ToPlane(&pVert[index1], &pVert[index2], &pVert[index3]);
	}
	
	//fprintf(testFile, "Finished Clipping\n");
	
	//fprintf(testFile,"Face #%d Number Of Vertexs: %d\n", faceIndex,  numberOfVertexs);
	//fclose(testFile);

	if (numberOfVertexs == 0)
	{
		return 0;
	}


	int size = numberOfVertexs;//(rand() % 2) ? 3 : 0;//numberOfVertexs;
	printf("numberOfVertexs: %d\n", numberOfVertexs);
	
	assert(numberOfVertexs % 3 == 0);
	if (numberOfVertexs % 3 != 0)
	{
		printf("Found Error:\n");
		exit(0);
	}

	for (int y = 0; y < size; y += 60)
	{
		triSend = 60;
		if (y + triSend > size)
			triSend = size % 60;
	
		//Add the vertexs first
		index = y;
		for (int x = 0; x < triSend; x++)
		{
			//currentInside->levelModel->addVertex(0.0f, 0.0f, 0.0f);
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
			//currentInside->levelModel->addNormal(0.0f, 0.0f, 0.0f);
			currentInside->levelModel->addNormal(currentInside->clipVertexList[index + x].normal[0], currentInside->clipVertexList[index + x].normal[1], currentInside->clipVertexList[index + x].normal[2]);
		}

		index = y;
		//Add the colors third
		for (int x = 0; x < triSend; x++)
		{
			//currentInside->levelModel->addColor(1.0f, 0.0f, 0.0f, 1.0f);
			currentInside->levelModel->addColor(currentInside->clipVertexList[index + x].color[0], currentInside->clipVertexList[index + x].color[1], currentInside->clipVertexList[index + x].color[2], currentInside->clipVertexList[index + x].color[3]);
		}

		index = y;
		//Add the st's last
		for (int x = 0; x < triSend; x++)
		{
			//currentInside->levelModel->addST(0.0f, 0.0f);
			currentInside->levelModel->addST(currentInside->clipVertexList[index + x].textureCoord[0], currentInside->clipVertexList[index + x].textureCoord[1]);
		}

	}

	//printf("Number Of Vertexs After Clip: %d\n", numberOfVertexs);
	return numberOfVertexs;
	
}

*/

//Good Version - No Clipping
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
		currentInside->levelModel->begin(TRIANGLE, VERTEX_T | COLOR_T | NORMAL_T | ST_T);
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




	int size = pFace->numMeshVerts;
	//printf("Size: %d\n", size);

	for (int y = 0; y < size; y += 60)
	{
		triSend = 60;
		if (y + triSend > size)
			triSend = size % 60;
	
		//Add the vertexs first
		//index = y;
		for (int x = 0; x < triSend; x++)
		{

			index = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + y].offset;
			currentInside->levelModel->addVertex(pVert[index].position[0], pVert[index].position[1], pVert[index].position[2]);
			if (x == 0)
			{
				currentInside->levelModel->setAddChangeValue(triSend + 1);
			}

		}

		//index = y;
		//Add the normals second
		for (int x = 0; x < triSend; x++)
		{
			index = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + y].offset;
			currentInside->levelModel->addNormal(pVert[index].normal[0], pVert[index].normal[1], pVert[index].normal[2]);
		}

		//index = y;
		//Add the colors third
		for (int x = 0; x < triSend; x++)
		{
			index = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + y].offset;
			currentInside->levelModel->addColor(pVert[index].color[0], pVert[index].color[1], pVert[index].color[2], pVert[index].color[3]);
		}

		//index = y;
		//Add the st's last
		for (int x = 0; x < triSend; x++)
		{
			index = currentInside->q3Level->m_pMeshVerts[pFace->meshVertIndex + x + y].offset;
			currentInside->levelModel->addST(pVert[index].textureCoord[0], pVert[index].textureCoord[1]);
		}

	}

	return 1;
	
}




int Level_Engine::clipQ3ToPlane(bspVertex *inputVertex1, bspVertex *inputVertex2, bspVertex *inputVertex3)
{
	int outCounter = 0;
	int inCounter = 0;
	int clipTestFlag[3] = {0};
	float testDistence;
	
	int vIndex1 = 0;
	int vIndex2 = 1;
	int vIndex3 = 2;
	vectorf v0, v1, v2, dv, result;
	vectorf transVertex1;
	vectorf transVertex2;
	vectorf transVertex3;
	vectorf uv0, uv1;
	bspVertex tmpV0, tmpV1;
	
	float *workTransVertex = transVertex1;
	float t = 0.0f;
	matrixf workMatrix;

	

	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			workMatrix[x][y] = currentInside->transformMatrix[x][y];
			
		}
		
	}
	

	//Copy the three actual vertex's to translation vertexs
	memcpy(transVertex1, inputVertex1->position, sizeof(float) * 3);
	memcpy(transVertex2, inputVertex2->position, sizeof(float) * 3);
	memcpy(transVertex3, inputVertex3->position, sizeof(float) * 3);
	transVertex1[3] = 1.0f;
	transVertex2[3] = 1.0f;
	transVertex3[3] = 1.0f;

	//printf("Pos X1 %f Y1 %f Z1 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);

	//Apply the current modelview matrix to the three vertexs
	ps2_vu0_apply_matrix(transVertex1, workMatrix, transVertex1);
	ps2_vu0_apply_matrix(transVertex2, workMatrix, transVertex2);
	ps2_vu0_apply_matrix(transVertex3, workMatrix, transVertex3);
	

	//printf("Pos X2 %f Y2 %f Z2 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);
	//printf("Pos X2 %f Y2 %f Z2 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);
	//printf("Pos X2 %f Y2 %f Z2 %f\n", transVertex1[0], transVertex1[1], transVertex1[2]);


	//Do clipping tests
	for (int x = 0; x < 3; x++)
	{

		testDistence = currentInside->znearPlane[0] * workTransVertex[0] + 
			currentInside->znearPlane[1] * workTransVertex[1] + 
			currentInside->znearPlane[2] * workTransVertex[2] + 
			currentInside->znearPlane[3];

		//printf("Distence = %f\n", testDistence[x]);
		if (testDistence < 0.0f)
		{
			clipTestFlag[x] = -1;
			outCounter++;
		}
		else if (testDistence > 0.0f)
		{
			clipTestFlag[x] = 1;
			inCounter++;
		}
		if (x == 0)workTransVertex = transVertex2;
		else if (x == 1)workTransVertex = transVertex3;
		
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

		return 3;
	}
	
	else if (inCounter == 0)
	{
		//Could delete all for clipping test
	
		/*
		currentInside->clipVertexList[currentInside->currentClipIndex] = *inputVertex1;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex].position, transVertex1, sizeof(float) * 3);
		currentInside->clipVertexList[currentInside->currentClipIndex + 1] = *inputVertex2;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 1].position, transVertex2, sizeof(float) * 3);
		currentInside->clipVertexList[currentInside->currentClipIndex + 2] = *inputVertex3;
		memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 2].position, transVertex3, sizeof(float) * 3);
		currentInside->currentClipIndex += 3;
		*/
		return 0;	
	}

	//return 0;

	
	
	//If there are two points just shorten up the two points to the znear plane
	
	if (outCounter == 2)
	{

		if (clipTestFlag[0] > 0)
		{
			memcpy(v0, transVertex1, sizeof(float) * 3);
			memcpy(v1, transVertex2, sizeof(float) * 3);
			memcpy(v2, transVertex3, sizeof(float) * 3);
		}
		else if (clipTestFlag[1] > 0)
		{
			memcpy(v0, transVertex2, sizeof(float) * 3);
			memcpy(v1, transVertex1, sizeof(float) * 3);
			memcpy(v2, transVertex3, sizeof(float) * 3);
			vIndex1 = 1;
			vIndex2 = 0;
			vIndex3 = 2;
		}
		else
		{
			memcpy(v0, transVertex3, sizeof(float) * 3);
			memcpy(v1, transVertex2, sizeof(float) * 3);
			memcpy(v2, transVertex1, sizeof(float) * 3);
			vIndex1 = 2;
			vIndex2 = 1;
			vIndex3 = 0;
		}

		//Add v0 to the clip list
		switch(vIndex1)
		{
		case 0:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex1] = *inputVertex1;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + vIndex1].position, transVertex1, sizeof(float) * 3);
			break;
		case 1:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex1] = *inputVertex2;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + vIndex1].position, transVertex2, sizeof(float) * 3);
			break;
		case 2:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex1] = *inputVertex3;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + vIndex1].position, transVertex3, sizeof(float) * 3);
			break;
		}

		//Recalculate v1 and add it to the clip list
		ps2_vu0_sub_vector(dv, v1, v0);
		if (dv[2] == 0.0f)
		{
			printf("Error Division by 0\n");
			return 0;
		}
		t = ((currentInside->znearPlane[3] - v0[2]) / dv[2]);
		//printf("t0: %f\n", t);
		ps2_vu0_inter_vector_xyz(result, v1, v0, t);

		switch(vIndex2)
		{
		case 0:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2] = *inputVertex1;
			break;
		case 1:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2] = *inputVertex2;
			break;
		case 2:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2] = *inputVertex3;
			break;
		}
		
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2].position[0] = result[0];
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2].position[1] = result[1];
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2].position[2] = currentInside->znearPlane[3];
		
		switch(vIndex1)
		{
		case 0:
			uv0[0] = inputVertex1->textureCoord[0];
			uv0[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv0[0] = inputVertex2->textureCoord[0];
			uv0[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv0[0] = inputVertex3->textureCoord[0];
			uv0[1] = inputVertex3->textureCoord[1];
			break;
		}

			uv0[2] = 1.0f;
			uv0[3] = 1.0f;


		switch(vIndex2)
		{
		case 0:
			uv1[0] = inputVertex1->textureCoord[0];
			uv1[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv1[0] = inputVertex2->textureCoord[0];
			uv1[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv1[0] = inputVertex3->textureCoord[0];
			uv1[1] = inputVertex3->textureCoord[1];
			break;
		}
		uv1[2] = 1.0f;
		uv1[3] = 1.0f;

		ps2_vu0_inter_vector_xyz(result, uv1, uv0, t);

		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2].textureCoord[0] = result[0];
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex2].textureCoord[1] = result[1];

		//Recalculate v2 and add it to the clip list	
		ps2_vu0_sub_vector(dv, v2, v0);

		if (dv[2] == 0.0f)
		{
			printf("Error Division by 0\n");
			return 0;
		}
		t = ((currentInside->znearPlane[3] - v0[2]) / dv[2]);
		//printf("t1: %f\n", t);
		ps2_vu0_inter_vector_xyz(result, v2, v0, t);

		switch(vIndex3)
		{
		case 0:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3] = *inputVertex1;
			break;
		case 1:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3] = *inputVertex2;
			break;
		case 2:
			currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3] = *inputVertex3;
			break;
		}
		
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3].position[0] = result[0];
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3].position[1] = result[1];
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3].position[2] = currentInside->znearPlane[3];
		
		switch(vIndex1)
		{
		case 0:
			uv0[0] = inputVertex1->textureCoord[0];
			uv0[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv0[0] = inputVertex2->textureCoord[0];
			uv0[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv0[0] = inputVertex3->textureCoord[0];
			uv0[1] = inputVertex3->textureCoord[1];
			break;
		}

			uv0[2] = 1.0f;
			uv0[3] = 1.0f;


		switch(vIndex3)
		{
		case 0:
			uv1[0] = inputVertex1->textureCoord[0];
			uv1[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv1[0] = inputVertex2->textureCoord[0];
			uv1[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv1[0] = inputVertex3->textureCoord[0];
			uv1[1] = inputVertex3->textureCoord[1];
			break;
		}
		uv1[2] = 1.0f;
		uv1[3] = 1.0f;

		ps2_vu0_inter_vector_xyz(result, uv1, uv0, t);

		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3].textureCoord[0] = result[0];
		currentInside->clipVertexList[currentInside->currentClipIndex + vIndex3].textureCoord[1] = result[1];


		currentInside->clipVertexList[currentInside->currentClipIndex].textureCoord[0] = 0.5f;
		currentInside->clipVertexList[currentInside->currentClipIndex].textureCoord[1] = 0.5f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 1].textureCoord[0] = 0.5f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 1].textureCoord[1] = 0.5f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 2].textureCoord[0] = 0.5f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 2].textureCoord[1] = 0.5f;



		currentInside->currentClipIndex += 3;
		return 3;

	}
	

	
	//Else inCounter = 1, then generate two points
	else
	{

		if (clipTestFlag[0] < 0)
		{
			memcpy(v0, transVertex1, sizeof(float) * 3);
			memcpy(v1, transVertex2, sizeof(float) * 3);
			memcpy(v2, transVertex3, sizeof(float) * 3);
		}
		else if (clipTestFlag[1] < 0)
		{
			memcpy(v0, transVertex2, sizeof(float) * 3);
			memcpy(v1, transVertex1, sizeof(float) * 3);
			memcpy(v2, transVertex3, sizeof(float) * 3);
			vIndex1 = 1;
			vIndex2 = 0;
			vIndex3 = 2;
		}
		else
		{
			memcpy(v0, transVertex3, sizeof(float) * 3);
			memcpy(v1, transVertex2, sizeof(float) * 3);
			memcpy(v2, transVertex1, sizeof(float) * 3);
			vIndex1 = 2;
			vIndex2 = 1;
			vIndex3 = 0;
		}		


		//Recalculate v1 and add it to the clip list
		ps2_vu0_sub_vector(dv, v1, v0);
		if (dv[2] == 0.0f)
		{
			printf("Error Division by 0\n");
			return 0;
		}
		t = ((currentInside->znearPlane[3] - v0[2]) / dv[2]);
		//printf("t0: %f\n", t);
		ps2_vu0_inter_vector_xyz(result, v1, v0, t);

		switch(vIndex2)
		{
		case 0:
			tmpV0 = *inputVertex1;
			break;
		case 1:
			tmpV0 = *inputVertex2;
			break;
		case 2:
			tmpV0 = *inputVertex3;
			break;
		}
		
		tmpV0.position[0] = result[0];
		tmpV0.position[1] = result[1];
		tmpV0.position[2] = currentInside->znearPlane[3];
		
		switch(vIndex1)
		{
		case 0:
			uv0[0] = inputVertex1->textureCoord[0];
			uv0[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv0[0] = inputVertex2->textureCoord[0];
			uv0[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv0[0] = inputVertex3->textureCoord[0];
			uv0[1] = inputVertex3->textureCoord[1];
			break;
		}

			uv0[2] = 1.0f;
			uv0[3] = 1.0f;


		switch(vIndex2)
		{
		case 0:
			uv1[0] = inputVertex1->textureCoord[0];
			uv1[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv1[0] = inputVertex2->textureCoord[0];
			uv1[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv1[0] = inputVertex3->textureCoord[0];
			uv1[1] = inputVertex3->textureCoord[1];
			break;
		}
		uv1[2] = 1.0f;
		uv1[3] = 1.0f;

		ps2_vu0_inter_vector_xyz(result, uv1, uv0, t);

		tmpV0.textureCoord[0] = result[0];
		tmpV0.textureCoord[1] = result[1];

		//Recalculate v2 and add it to the clip list	
		ps2_vu0_sub_vector(dv, v2, v0);
		if (dv[2] == 0.0f)
		{
			printf("Error Division by 0\n");
			return 0;
		}
		t = ((currentInside->znearPlane[3] - v0[2]) / dv[2]);
		//printf("t1: %f\n", t);
		ps2_vu0_inter_vector_xyz(result, v2, v0, t);

		switch(vIndex3)
		{
		case 0:
			tmpV1 = *inputVertex1;
			break;
		case 1:
			tmpV1 = *inputVertex2;
			break;
		case 2:
			tmpV1 = *inputVertex3;
			break;
		}
		
		tmpV1.position[0] = result[0];
		tmpV1.position[1] = result[1];
		tmpV1.position[2] = currentInside->znearPlane[3];
		
		switch(vIndex1)
		{
		case 0:
			uv0[0] = inputVertex1->textureCoord[0];
			uv0[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv0[0] = inputVertex2->textureCoord[0];
			uv0[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv0[0] = inputVertex3->textureCoord[0];
			uv0[1] = inputVertex3->textureCoord[1];
			break;
		}

			uv0[2] = 1.0f;
			uv0[3] = 1.0f;


		switch(vIndex3)
		{
		case 0:
			uv1[0] = inputVertex1->textureCoord[0];
			uv1[1] = inputVertex1->textureCoord[1];
			break;
		case 1:
			uv1[0] = inputVertex2->textureCoord[0];
			uv1[1] = inputVertex2->textureCoord[1];
			break;
		case 2:
			uv1[0] = inputVertex3->textureCoord[0];
			uv1[1] = inputVertex3->textureCoord[1];
			break;
		}
		uv1[2] = 1.0f;
		uv1[3] = 1.0f;

		ps2_vu0_inter_vector_xyz(result, uv1, uv0, t);

		tmpV1.textureCoord[0] = result[0];
		tmpV1.textureCoord[1] = result[1];
		
		

		//Add the new vertexs
		
		switch(vIndex1)
		{
		case 0:
			currentInside->clipVertexList[currentInside->currentClipIndex] = tmpV1;
			currentInside->clipVertexList[currentInside->currentClipIndex + 1] = tmpV0;
			currentInside->clipVertexList[currentInside->currentClipIndex + 2] = *inputVertex2;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 2].position, v1, sizeof(float) * 3);

			currentInside->clipVertexList[currentInside->currentClipIndex + 3] = tmpV1;
			currentInside->clipVertexList[currentInside->currentClipIndex + 4] = *inputVertex2;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 4].position, v1, sizeof(float) * 3);
			currentInside->clipVertexList[currentInside->currentClipIndex + 5] = *inputVertex3;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 5].position, v2, sizeof(float) * 3);
			break;
		case 1:

			currentInside->clipVertexList[currentInside->currentClipIndex] = tmpV1;
			currentInside->clipVertexList[currentInside->currentClipIndex + 1] = tmpV0;
			currentInside->clipVertexList[currentInside->currentClipIndex + 2] = *inputVertex1;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 2].position, v1, sizeof(float) * 3);

			currentInside->clipVertexList[currentInside->currentClipIndex + 3] = tmpV1;
			currentInside->clipVertexList[currentInside->currentClipIndex + 4] = *inputVertex1;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 4].position, v1, sizeof(float) * 3);
			currentInside->clipVertexList[currentInside->currentClipIndex + 5] = *inputVertex3;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 5].position, v2, sizeof(float) * 3);
	
			break;
		case 2:

			currentInside->clipVertexList[currentInside->currentClipIndex] = tmpV1;
			currentInside->clipVertexList[currentInside->currentClipIndex + 1] = tmpV0;
			currentInside->clipVertexList[currentInside->currentClipIndex + 2] = *inputVertex2;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 2].position, v1, sizeof(float) * 3);

			currentInside->clipVertexList[currentInside->currentClipIndex + 3] = tmpV1;
			currentInside->clipVertexList[currentInside->currentClipIndex + 4] = *inputVertex2;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 4].position, v1, sizeof(float) * 3);
			currentInside->clipVertexList[currentInside->currentClipIndex + 5] = *inputVertex1;
			memcpy(currentInside->clipVertexList[currentInside->currentClipIndex + 5].position, v2, sizeof(float) * 3);

			
			break;
		}

		currentInside->clipVertexList[currentInside->currentClipIndex].textureCoord[0] = 0.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex].textureCoord[1] = 0.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 1].textureCoord[0] = 0.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 1].textureCoord[1] = 0.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 2].textureCoord[0] = 0.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 2].textureCoord[1] = 0.0f;

		currentInside->clipVertexList[currentInside->currentClipIndex + 3].textureCoord[0] = 1.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 3].textureCoord[1] = 1.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 4].textureCoord[0] = 1.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 4].textureCoord[1] = 1.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 5].textureCoord[0] = 1.0f;
		currentInside->clipVertexList[currentInside->currentClipIndex + 5].textureCoord[1] = 1.0f;

		currentInside->currentClipIndex += 6;
		
	
		return 6;

		
	}
	

	return 0;
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

	//printf("Plane %f %f %f %f\n", currentInside->znearPlane[0], currentInside->znearPlane[1], currentInside->znearPlane[2], currentInside->znearPlane[3]);


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
		printf("Number of Verts For face #%d: %d\n", x, newInside->q3Level->m_pFaces[x].numMeshVerts);
		if (newInside->q3Level->m_pFaces[x].numMeshVerts > newInside->maxNumberOfClipVertexs)
		{
			newInside->maxNumberOfClipVertexs = newInside->q3Level->m_pFaces[x].numMeshVerts;
		}
	}

	buildPlane(-21.0f);

	//printf("Plane X %f Y %f Z %f D %f\n", currentInside->znearPlane[0], currentInside->znearPlane[1], currentInside->znearPlane[2], currentInside->znearPlane[3]);
	//Make a buffer for the clipped vertexs to be twice the most verts there are in
	//faces stored in the quake 3 level file.
	newInside->maxNumberOfClipVertexs *= 2;
	printf("Max Number Of Clip Vertexs: %d\n", newInside->maxNumberOfClipVertexs);
	newInside->clipVertexList = new bspVertex[newInside->maxNumberOfClipVertexs];
	if (newInside->clipVertexList == NULL)
	{
		printf("Error allocating mememory for clip list\n");
		return -1;
	}
	//printf("clipVertexList: %d\n", newInside->clipVertexList);

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
	
	//FILE *dmpFile;

	//fopen("test.txt", "a+");

	if (currentInside == NULL)return -1;
	matrixf workMatrix;
	int numberOfVertexs = 0;

	
 	//Get the model view matrix and store it
	int oldMode = MatrixManager->setMode(MODELVIEW);
	workMatrix = *MatrixManager->getMatrix();

	/*
	//Get the camera matrix and apply it to the model view matrix and store it.
	MatrixManager->setMode(CAMERA);
	
	ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
	*/
	MatrixManager->setMode(oldMode);

	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			currentInside->transformMatrix[x][y] = workMatrix[x][y];
		}
		
	}

	

	int x = currentInside->q3Level->numberOfFaces;

	currentInside->levelModel->getModelPacket()->setSend(1);

	currentInside->q3Level->facesDrawn.ClearAll();
	convertQ3Node2CustomModel(-1);
	while(x--)
	{
		if (x <= -1)break;
		if (currentInside->q3Level->m_pFaces[x].type != FACE_POLYGON)continue;
		if (!currentInside->q3Level->facesDrawn.On(x))
		{
			currentInside->q3Level->facesDrawn.Set(x);
			numberOfVertexs += convertQ3Node2CustomModel(x);
		}
	}
	//printf("Number Of Level Vertex: %d\n", numberOfVertexs);
	/*
	if (numberOfVertexs < 100)
	{
		//fprintf(dmpFile, "Level Not Drawn\n");
		currentInside->levelModel->getModelPacket()->setSend(0);
		return 0;
	}
	*/
	//fclose(dmpFile);
	convertQ3Node2CustomModel(-2);
	


	return 1;
}

int Level_Engine::buildDisplayList(int levelID)
{
	return 1;
}

