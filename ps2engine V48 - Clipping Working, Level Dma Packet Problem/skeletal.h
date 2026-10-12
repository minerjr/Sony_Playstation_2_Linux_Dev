#ifndef SKELETAL_H
#define SKELETAL_H

struct Skeletal_Model_Data
{
	int modelID;
	int textureID;


	Dma_Packet *modelPacket;
};



class Skeletal_Model_Engine
{

	Skeletal_Model_Data *head;

public:

	Skeletal_Model_Engine();
	~Skeletal_Model_Engine();

	int addModel(char *fileName);

	int removeModel(int modelID);
	int runAnimation(int modelID, int animationID);
	int resetAnimation(int modelID, int animationID);

	Dma_Packet *getPacket(int modelID);
	int getTextureID(int modelID);
	int getNumberOfAnimations();


};


#endif