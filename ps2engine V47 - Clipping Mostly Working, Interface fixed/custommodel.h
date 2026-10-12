#ifndef CUSTOMMODEL_H
#define CUSTOMMODEL_H

#include "matrix.h"
#include "dmapacket.h"
#include "dmamemory.h"
#include "texture.h"

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
#define NORESET_T 16
#define DONTAPPLYMODELCAMERA_T 32
#define ALPHA_T 64

class Custom_Model
{
private:
	Dma_Packet *modelPacket;
	matrixf *modelMatrix;
	matrixf *colorMatrix;
	matrixf *lightMatrix;

	static vectorf normal;

	uint32 *modelGifHeader;

	uint32 *modelVertexHeader;
	uint32 *modelNormalHeader;
	uint32 *modelColorHeader;
	uint32 *modelSTHeader;

	Matrix_Manager *MatrixManager;

	Texture_Engine *TextureEngine;
	
	int mode;
	int channel;
	int addChangeValue;

	int dataType;

	int addCount;

	int addFlg;
	
	int textureID;

	int doVuAdd(int flg);
	
public:
	Custom_Model(Matrix_Manager *tmpMatrixManager, int tmpChannel, Dma_Memory_Engine *DmaMemoryEngine, Texture_Engine *tmpTextureEngine);
	~Custom_Model();
	
	Dma_Packet *getModelPacket();
	int begin(int tmpMode, int tmpDataType);
	int end();
	int setTextureID(int tID);

	int addVertex(vectorf *vertexData);
	int addVertex(float verX, float verY, float verZ, float verW = 1.0f);

	int addColor(vectorf *colorData);
	int addColor(float red, float green, float blue, float alpha = 1.0f);

	int addNormal(vectorf *norm);
	int addNormal(float normX, float normY, float normZ);


	int addST(vectorf *stData);
	int addST(float s, float t);

	int setAddChangeValue(int newAddChangeValue);


	//void createDummy();
};

vectorf Custom_Model::normal;

#endif
