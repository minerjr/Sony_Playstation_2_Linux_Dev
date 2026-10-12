class Model_Engine
{

private:
	Keyframe_Model_Engine *KFMEngine;
	Skeletal_Model_Engine *SKMEngine;
	Static_Model_Engine *STMEngine;
	Custom_Model *CM;
	

	int enabledModelTypes;

public:
	Model_Engine(int enableModelTypes);
	~Model_Engine();

	int addModel(char *fileName);
	int removeModel(int modelID);
	int runAnimation(int modelID, int animationID);
	int resetAnimation(int modelID);

	int getTextureID(int modelID);
	int getNumberOfAnimations();
	Dma_Packet *getPacket(int modelID);

};