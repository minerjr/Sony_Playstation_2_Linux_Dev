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

	Model_Engine *GEMModelEngine;

	Matrix_Manager *GEMatrixManager;

	Texture_Engine *GETextureEngine;

	Camera *GECameraPosition;

	Level_Engine *GELevelEngine;

	Effect_Generator *GEEffectGenerator;

	Game_HUD *GEGameHUD;

public:
	Graphics_Engine(int numberOf4KPages);
	~Graphics_Engine();

	int clearScene(int red, int blue, int green);
		
	int renderScene();

	int flipScene();

	int loadModel(char *fileName);
	int createCustomModel();

	int loadTexture(char *fileName);

	int loadLevel(Area *areaToLoad);

	int setCameraPosition(Camera *CPos);


};