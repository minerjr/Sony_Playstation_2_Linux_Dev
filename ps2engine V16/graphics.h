#include "sps2inc.h"
#include "vumgr.h"
#include "dmamemory.h"
#include "dmascheduler.h"

class Graphics_Engine
{
private:

	VU_Manger *GEVUManager;
	Dma_MemoryEngine *GEDMAMemoryEngine;
	Dma_Scheduler *GEDMAScheduler;
	//Key_Frame_Model *KeyFrameModelList;
	//Skel_Model *SkelModelList;
	//Custom_Model *CustomModelList;
	//Static_Model *StaticModel;
	//Matrix_Engine *MatrixEngine;
	atrix_Manager *MatrixManager;


public:
	Graphics_Engine(int numberOf4KPages);
	~Graphics_Engine();

	int clearScene(int red, int blue, int green);
		
	int renderScene();

	int loadModel(char *fileName);
	int startCustomModel()

	int loadTexture(char *fileName);


};