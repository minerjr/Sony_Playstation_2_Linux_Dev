#include "custommodel.h"
#include "structs.h"

#ifndef KEYFRAME_H
#define KEYFRAME_H


struct Keyframe_Model_Animation_Data
{
	int animationID;
	int startIndex;
	int endIndex;

	Keyframe_Model_Animation_Data *next;
};

struct Keyframe_Model_ST_Data
{
	float s;
	float t;
};

struct Keyframe_Model_Vertex_Data
{
	float x;
	float y;
	float z;
};

struct Keyframe_Model_Mesh_Data
{
	unsigned short meshIndex[3];
	unsigned short stIndex[3];
};

struct Keyframe_Model_Data
{
	int modelID;
	int numberOfFrames;
	int numberOfST;
	int numberOfTriangles;
	int numberOfVerticies;
	Keyframe_Model_ST_Data *stData;
	Keyframe_Model_Vertex_Data *vertexData;
	Keyframe_Model_Mesh_Data *triData;

	Keyframe_Model_Animation_Data *animationHead;
	
	Keyframe_Model_Data *next;
};


struct Keyframe_Character_Data
{
	int textureID;
	int modelID;
	int characterID;

	int currentAnimationID;
	int currentFrameID;
	float currentFrameChange;
	
	Custom_Model *firstModel;
	Custom_Model *secondModel;
	Dma_Packet *currentSendPacket;
	Dma_Packet *currentModifyPacket;
	uint64 lastUpdateTime;


	Keyframe_Character_Data *next;
};


class Keyframe_Model_Engine
{
private:
	Keyframe_Model_Data *modelHead;
	Keyframe_Character_Data *characterHead;
	int latestModelID;
	int latestCharacterID;
	Matrix_Manager *MatrixManager;
	Dma_Memory_Engine *DmaMemoryEngine;
	Texture_Engine *TextureEngine;

public:

	Keyframe_Model_Engine(Matrix_Manager *tmpMatrixManager, Dma_Memory_Engine *tmpDmaMemoryEngine, Texture_Engine *tmpTextureEngine);
	~Keyframe_Model_Engine();

	int addModel(char *fileName);

	int loadMD2(char *fileName);
	int loadCKM(char *fileName);
	int buildCharacter(int modelID, int textureID);

	int removeModel(int modelID);
	int removeCharacter(int characterID);
	int runAnimation(int characterID, int animationID);
	int resetAnimation(int characterID, int animationID);

	int flipSendPacket(int modelID);

	Dma_Packet *getFirstPacket(int characterID);
	Dma_Packet *getSecondPacket(int characterID);
	int getTextureID(int characterID);
	int getNumberOfAnimations(int characterID);
	
};


#endif