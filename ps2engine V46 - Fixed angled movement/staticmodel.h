#ifndef STATICMODEL_H
#define STATICMODEL_H

struct Static_Model_Data
{
	int modelID;
	int textureID;


	Dma_Packet *modelPacket;
};



class Static_Model_Engine
{

	Static_Model_Data *head;

public:

	Static_Model_Engine();
	~Static_Model_Engine();

	int addModel(char *fileName);

	int removeModel(int modelID);

	Dma_Packet *getPacket(int modelID);
	int getTextureID(int modelID);


};


#endif