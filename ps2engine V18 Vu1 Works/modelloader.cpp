#include "modelloader.h"
#include "md2file.h"
#include "ms3dfile.h"
#include "addregisters.h"
#include <string.h>
#include <stdlib.h>

int loadMd2(Dma_Packet *tmpPacket, char *fileName)
{
	sps2GIFTag_t GifTag;

	sps2GIFPackedRegister_t VertexRegister;
	sps2GIFPackedRegister_t ColorRegister;

	Md2File *newMd2;

	newMd2 = new Md2File;
	newMd2->Load(fileName, 0);

	
	GifTag = AddGifTag(3 * newMd2->m_Header.numTriangles, 1, 1, 11, GIF_FLG_PACKED, 0);
	
	printf("Add data to gif tag...");
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
	// Add the XYZ2 register to the giftag. 
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
	printf("done\n");

	printf("Add gif tag...");
	tmpPacket->add((uint128)GifTag.i128);

	for (int x = 0; x < newMd2->m_Header.numTriangles; x++)
	{
		
		for (int y = 0; y < 3; y++)
		{
			int triangleIndex1 = newMd2->m_pTriangles[x].vertexIndices[y];

		ColorRegister = AddColour(rand() % 255, rand() % 255,rand() % 255,0x80);

			tmpPacket->add((uint128)ColorRegister.i128);

			VertexRegister = AddVertex( (int)newMd2->m_pFrames[0].pVertices[triangleIndex1].vertex[0], 
				(int)newMd2->m_pFrames[0].pVertices[triangleIndex1].vertex[1], 
				(int)newMd2->m_pFrames[0].pVertices[triangleIndex1].vertex[2], 0);
	
			tmpPacket->add((uint128)VertexRegister.i128);
		}
	}

	tmpPacket->finish();

	delete newMd2;

	return 1;
}

int loadMs3d(Dma_Packet *tmpPacket, char *fileName)
{
	sps2GIFTag_t GifTag;

	sps2GIFPackedRegister_t VertexRegister;
	sps2GIFPackedRegister_t ColorRegister;

	Ms3dFile *newMs3d;

	newMs3d = new Ms3dFile;
	newMs3d->Load(fileName);

	int numTriangles = 0;

	//Count how many triangles there are in total in the model
	for (int x = 0; x < newMs3d->numGroups; x++)
	{
		numTriangles += newMs3d->groups[x].numTriangles;
	}

	GifTag = AddGifTag(3 * numTriangles, 1, 1, 11, GIF_FLG_PACKED, 0);
	
	printf("Add data to gif tag...");
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
	// Add the XYZ2 register to the giftag. 
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
	printf("done\n");

	printf("Add gif tag...");
	tmpPacket->add((uint128)GifTag.i128);
	
	for (int loopG = 0; loopG < newMs3d->numGroups; loopG++)
	{
		for (int loopT = 0; loopT < newMs3d->groups[loopG].numTriangles; loopT++)
		{
			int triangleIndex = newMs3d->groups[loopG].triangleIndices[loopT];
			const MS3D_TRIANGLE* tri = &newMs3d->triangles[triangleIndex];

			for (int loopV = 0; loopV < 3; loopV++)
			{
				int vertexindex = tri->vertexIndices[loopV];

				ColorRegister = AddColour(rand() % 255, rand() % 255,rand() % 255,0x80);

				tmpPacket->add((uint128)ColorRegister.i128);

			  
				VertexRegister = AddVertex( (int)newMs3d->vertices[vertexindex].vertex[0], 
					(int)-newMs3d->vertices[vertexindex].vertex[1], 
					(int)newMs3d->vertices[vertexindex].vertex[2], 0);
	
				tmpPacket->add((uint128)VertexRegister.i128);
			}
		}
	}
	tmpPacket->finish();


	delete newMs3d;

	return 1;
}

int loadModel(Dma_Packet *tmpPacket, char *fileName)
{


	if (strstr(fileName,".ms3d") != NULL)
	{
		loadMs3d(tmpPacket, fileName);
	}
	else if (strstr(fileName, ".md2") != NULL)
	{
		loadMd2(tmpPacket, fileName);
	}
	else
	{
		printf("Non valid type of model\n");
	}




	return 1;
}

