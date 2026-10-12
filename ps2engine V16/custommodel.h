#ifndef CUSTOMMODEL_H
#define CUSTOMMODEL_H

#include "matrix.h"
#include "dmapacket.h"
#include "dmamemory.h"

//mode
#define POINT 0
#define LINE 1
#define LINESTRIP 2
#define TRIANGLE 3
#define TRIANGLESTRIP 4
#define TRIANGLEFAN 5
#define SPRITE 6

//channel
#define CHANNEL1 1
#define CHANNEL2 2

//data type
#define VERTEX_T 1
#define COLOR_T 2
#define NORMAL_T 4
#define ST_T 8

class Custom_Model
{
private:
	Dma_Packet *modelPacket;
	matrixf *modelMatrix;
	matrixf *colorMatrix;
	matrixf *lightMatrix;

	uint32 *modelGifHeader;

	Matrix_Manager *MatrixManager;
	
	int mode;
	int channel;

	int dataType;

	int addCount;
	
public:
	Custom_Model(Matrix_Manager *tmpMatrixManager, int tmpChannel, Dma_Memory_Engine *DmaMemoryEngine);
	~Custom_Model();
	
	Dma_Packet *getModelPacket();
	int begin(int tmpMode, int tmpDataType);
	int end();

	int addVertex(vectorf *vertexData);
	int addVertex(float verX, float verY, float verZ, float verW = 1.0f);

	int addColor(vectorf *colorData);
	int addColor(float red, float green, float blue, float alpha = 1.0f);
/*
	int addNormal(vectorf *normalData);
	int addNormal(float normalX, float normalY, float normalZ, float normalW);

	int addST(vectorf *stData);
	int addST(float s, float t);
*/
};

#endif
