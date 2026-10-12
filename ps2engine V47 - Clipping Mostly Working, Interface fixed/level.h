#include "custommodel.h"
#include "q3bspfile.h"
#include "matrix.h"
#ifndef LEVEL_H
#define LEVEL_H


struct Outside_Level_Data
{
	Custom_Model *levelModel;
	int levelID;
};

struct Inside_Level_Data
{
	Q3BSP_Loader *q3Level;
	Custom_Model *levelModel;

	bspVertex *clipVertexList;
	int maxNumberOfClipVertexs;
	vectorf znearPlane;
	float transformMatrix[4][4];


	int levelID;
	int currentClipIndex;
};

class Level_Engine
{

private:
	//Q3BSP_Loader *InsideLoader;
	//Custom_Level_Loader *OutsideLoader;
	Outside_Level_Data *currentOutside;
	Inside_Level_Data *currentInside;
	int latestLevelID;

	Matrix_Manager *MatrixManager;
	Dma_Memory_Engine *DmaMemoryEngine;
	Texture_Engine *TextureEngine;

	int buildQ3Level(Inside_Level_Data *newInside, char *fileName);
	int updateQ3Level();
	int convertQ3Node2CustomModel(int faceIndex);
	int clipQ3ToPlane(bspVertex *inputVertex1, bspVertex *inputVertex2, bspVertex *inputVertex3);
	int buildPlane(float znear);

public:

	Level_Engine(Matrix_Manager *tmpMatrixManager, Dma_Memory_Engine *tmpDmaMemoryEngine, Texture_Engine *tmpTextureEngine);
	~Level_Engine();
	
	int load(char *fileName);
	//TODO
	//int buildDisplayList(Camera *cameraPosistion, int levelID);
	int buildDisplayList(int levelID);
	Dma_Packet *getPacket(int levelID);

	int updateLevel(int levelID);


};

#endif