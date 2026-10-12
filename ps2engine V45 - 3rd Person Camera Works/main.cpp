#include <stdio.h>
#include "dmascheduler.h"
#include "dmapacket.h"
#include "dmamemory.h"
#include "addregisters.h"
#include "modelloader.h"
#include "vumgr.h"
#include "matrix.h"
#include "custommodel.h"
#include "keyframe.h"
#include "texture.h"
#include "ps2defines.h"
#include "gameclock.h"
#include "level.h"
#include "input.h"
#include <math.h>


Dma_Memory_Engine *DmaMemoryEngine;
Dma_Memory_Scheduler *DmaMemoryScheduler;
VU_Manager *VUManager;
Matrix_Manager *MatrixManager;
Texture_Engine *TextureEngine;
Game_Clock_Engine *GameClockEngine;
Level_Engine *LevelEngine;
Keyframe_Model_Engine *KeyframeEngine;
Input_Engine *InputEngine;
PS2Pad *joypad;


int setupGame();
int shutDownGame();

int main()
{
	bool done = false;

	int lightFlg = NORMAL_T;

	float mainCharacterx = 0.0f;
	float mainCharactery = 0.0f;
	float mainCharacterz = 0.0f;
	float mainCharacterAngle = 0.0f;

	float worldx = 0.0f;
	float worldy = 0.0f;
	float worldz = 0.0f;
	float worldAngle = 0.0f;

	float npcx = 64.0f;
	float npcy = 0.0f;
	float npcz = -100.0f;
	
	vectorf upVector = {0.0f, 0.0f, -1.0f, 1.0f};
	vectorf mainCharacterFaceAngle = {1.0f, 0.0f, 0.0f, 1.0f};
	

	setupGame();



	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	LevelEngine->load("data/maps/Level01.bsp");


	//Add the texture to the dma scheduler

	DmaMemoryScheduler->add(TextureEngine->getPacket(2));
	DmaMemoryScheduler->add(LevelEngine->getPacket(0));


	DmaMemoryScheduler->add(TextureEngine->getPacket(0));

	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(0));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(0));	

	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(1));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(1));	


	//DmaMemoryScheduler->add(TestPacket);
	DmaMemoryScheduler->add(TextureEngine->getPacket(1));
	//Add the custom model, TestModel, to the dma scheduler

	//Add the custom model, TestVuModel, to the dma scheduler


	printf("done\n");
	

	printf("Drawing Started\n");


	vectorf testVect = { 0.0f, 0.0f, 0.0f, 1.0f};
	
	//done = 1;

	int runAnimation = 0;
	while(!done)
	{
		
		

		runAnimation = 0;
		if (InputEngine->readInput())
		{

			if(joypad->IsButtonPressed(PS2Pad::BUTTON_TRIANGLE))
				done = true;
			else if(joypad->IsButtonPressed(PS2Pad::BUTTON_CIRCLE))
			{
				if(lightFlg)
				{
					//TextureEngine->getPacket(0)->setSend(1);
					lightFlg = 0;
				}
				else
				{
					lightFlg = NORMAL_T;
					//TextureEngine->getPacket(1)->setSend(1);
				}
			
			}
			
			if ((joypad->GetLeftX() != 0.0f) || (joypad->GetLeftY() != 0.0f))
			{
				runAnimation = 1;
				//forward += (joypad->GetLeftY() + joypad->GetLeftX()) * 0.5f ;
				mainCharacterFaceAngle[0] = joypad->GetLeftX();
				mainCharacterFaceAngle[2] = -joypad->GetLeftY();
				

				if ((-joypad->GetLeftY() >= 0.0f) && (joypad->GetLeftX() >= 0.0f))
				{
					upVector[0] = 1.0f;
					upVector[1] = 0.0f;
					upVector[2] = 0.0f;
					upVector[3] = 1.0f;
					mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);		
					mainCharacterAngle = PI - (mainCharacterAngle * PI * 0.5f);
				}
				else if (-joypad->GetLeftY() >= 0.0f && joypad->GetLeftX() <= 0.0f)
				{
					upVector[0] = 0.0f;
					upVector[1] = 0.0f;
					upVector[2] = 1.0f;
					upVector[3] = 1.0f;
					mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);
					mainCharacterAngle = -PI + ((1.0f - mainCharacterAngle) * PI * 0.5f);
				}
				else if (-joypad->GetLeftY() <= 0.0f && joypad->GetLeftX() <= 0.0f)
				{
					upVector[0] = -1.0f;
					upVector[1] = 0.0f;
					upVector[2] = 0.0f;
					upVector[3] = 1.0f;
					mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);
					mainCharacterAngle = -PI * 0.5f + ((1.0f - mainCharacterAngle) * PI * 0.5f);
				}
				else if (-joypad->GetLeftY() <= 0.0f && joypad->GetLeftX() >= 0.0f)
				{
					upVector[0] = 0.0f;
					upVector[1] = 0.0f;
					upVector[2] = -1.0f;
					upVector[3] = 1.0f;
					
					mainCharacterAngle = ps2_vu0_inner_product(upVector, mainCharacterFaceAngle);
					mainCharacterAngle = PI * 0.5f - (mainCharacterAngle * PI * 0.5f);
				}


				//printf("Main Character Angle = %f\n", mainCharacterAngle);

				

				mainCharacterx -= sinf(mainCharacterAngle) * (2.0f);
				mainCharacterz -= cosf(mainCharacterAngle) * (2.0f);
								
				//if (angy > PI)angy = -PI;
				//else if (angy < -PI)angy = PI;
				

			}
			
			//printf("Sin Angle %f\n", sinf(mainCharacterAngle));
			//printf("-Cos Angle %f\n", -cosf(mainCharacterAngle));

			if ((joypad->GetRightX() != 0.0f) || (joypad->GetRightY() != 0.0f))
			{
				
				//mainCharacterz += joypad->GetRightY();
				//if (mainCharacterz > -32.0f) mainCharacterz = -32.0f;
				//if (mainCharacterz < -150.0f) mainCharacterz = -150.0f;
				
				worldAngle -= joypad->GetRightX() * 0.1f;
				if (worldAngle > PI)worldAngle = -PI;
				else if (worldAngle < -PI)worldAngle = PI;
				//worldx -= sinf(worldAngle) * joypad->GetRightY();
				//worldz -= -cosf(worldAngle) * joypad->GetRightY();

			}


		}

		//printf("Left stick x/y: %f %f\n", joypad->GetLeftX(), -joypad->GetLeftY());
		//printf("Right stick x/y: %f %f\n", joypad->GetRightX(), -joypad->GetRightY());
		
		printf("WorldX: %f WorldZ %f\n", mainCharacterx, mainCharacterz);
		MatrixManager->setMode(MODELVIEW);
		MatrixManager->loadIdentity();
		MatrixManager->translate(0.0f, 0.0f, -100.0f);
		MatrixManager->rotate(-worldAngle, 0.0f, 1.0f, 0.0f);
		//MatrixManager->translate(mainCharacterx, mainCharactery, mainCharacterz);
		MatrixManager->translate(mainCharacterx, mainCharactery, mainCharacterz);
		//MatrixManager->translate(-mainCharacterx, 0.0f, -mainCharacterz);
		

		//MatrixManager->translate(0.0f, 0.0f, -100.0f);
		//MatrixManager->translate(mainCharacterx, -mainCharactery, -mainCharacterz);
		//ps2_vu0_apply_matrix(testVect, *MatrixManager->getMatrix(), testVect);
		//printf("Test Vector X: %f, Y: %f Z: %f\n", testVect[0], testVect[1], testVect[2]);
		//MatrixManager->translate(worldx, worldy, worldz);
		
		

		//MatrixManager->rotate(PI / 2, 0.0f, 0.0f, 1.0f);
		MatrixManager->push();
		//MatrixManager->translate(worldx, worldy, worldz);
		

		LevelEngine->updateLevel(0);
		
		

		//Clear the screen with the default color
		sps2UScreenClear(0x00, 0x00, 0x00);

		
		//Build the list of things to transfer along
		//Dma channel 1 and 2
		DmaMemoryScheduler->buildList(CHANNEL1);
		//Start the transfer along channel 1
		DmaMemoryScheduler->transfer(CHANNEL1);
		//Wait for the transfer to finish

		
		MatrixManager->pop();
		//Try out the scale matrix function
		//MatrixManager->scale(0.5f, 0.5f, 0.5f);
		//Update the position of the square
		//axisx += sinf(angy) * forward;
		//axisz += -cosf(angy) * forward;
		MatrixManager->translate(npcx, npcy, npcz);
		//Rotate the square along its z coordinate
		
		MatrixManager->push();
		//MatrixManager->rotate(-PI / 2,0.0f, 1.0f, 0.0f);
		//MatrixManager->translate(5.0f, 0.0f, 0.0f);
		KeyframeEngine->runAnimation(1, 2);
		KeyframeEngine->flipSendPacket(1);
		MatrixManager->pop();


		MatrixManager->push();
		MatrixManager->loadIdentity();
		MatrixManager->translate(0.0f, 0.0f, -100.0f);
		MatrixManager->rotate(-PI / 2,0.0f, 1.0f, 0.0f);
		MatrixManager->rotate(mainCharacterAngle, 0.0f, 1.0f, 0.0f);
		
		
		
		

		KeyframeEngine->runAnimation(0, runAnimation);
		KeyframeEngine->flipSendPacket(0);

		MatrixManager->pop();

		DmaMemoryScheduler->waitForTransfer(CHANNEL1);
		
		
		//Flip the video buffers
		sps2UScreenSwap();


	}


	shutDownGame();

	return 0;

}


int setupGame()
{

	printf("\nPS2 engine - test program\n");
	
	printf("Create Input Engine: ");
	InputEngine = new Input_Engine();
	joypad = InputEngine->getInputDevice();
	printf("done\n");

	printf("Create Game Clock Engine: ");
	GameClockEngine = new Game_Clock_Engine();
	printf("done\n");

	printf("Create Memory Engine: ");
	//Allocate 8 megs for dma memory, IE textures and models ( 2072 * 4K)
	DmaMemoryEngine = new Dma_Memory_Engine(2072);
	//Setup the dma scheduler, takes care of the complicated transfers
	DmaMemoryScheduler = new Dma_Memory_Scheduler(DmaMemoryEngine);
	printf("done\n");


	//Setup the video
	sps2UScreenInit(0);	

	printf("Create VU Manager: ");
	//Setup the vu manager used to upload programs and gain accesss the
	//uu's.
	VUManager = new VU_Manager(DmaMemoryEngine, VU0_ENABLED | VU1_ENABLED);
	printf("done\n");


	printf("Create Texture Engine: ");
	TextureEngine = new Texture_Engine(DmaMemoryEngine);
	printf("done\n");


	printf("Create Matrix Manager: ");
	MatrixManager = new Matrix_Manager();
	printf("done\n");

	printf("Create Keyframe Engine: ");
	KeyframeEngine = new Keyframe_Model_Engine(MatrixManager, DmaMemoryEngine, TextureEngine);
	KeyframeEngine->addModel("data/models/tris.md2");
	KeyframeEngine->addModel("data/models/Lamp_post.md2");
	//KeyframeEngine->addModel("data/models/test.md2");
	printf("done\n");

	printf("Create Level Engine: ");
	LevelEngine = new Level_Engine(MatrixManager, DmaMemoryEngine, TextureEngine);
	printf("done\n");

	printf("Upload VU1 micro program: ");
	VUManager->vuUpload(VU1,"libexec/vu1/trans.vu");
	printf("done\n");

	printf("Load Texture: ");
	TextureEngine->loadTexture("data/textures/tris.bmp", 0);
	TextureEngine->loadTexture("data/textures/texture02.cif", 1);
	TextureEngine->loadTexture("data/textures/texture03.bmp", 2);
	printf("done\n");

	

	//Setup the view port matrix
	MatrixManager->setMode(VIEW);
	MatrixManager->setView((float)sps2UScreenGetWidth(), (float)sps2UScreenGetHeight(), 2048.0f, 2048.0f, (float)(1 << 24));
	MatrixManager->dump("View.txt");
	
	//Setup the projection matrix
	MatrixManager->setMode(PROJECTION);
	MatrixManager->setProjection(60.0f, 4.0f / 3.0f, -5.0f, -1000.0f);
	MatrixManager->dump("Projection.txt");

	MatrixManager->setProjectionScale(2048.0f, 2048.0f, ((float)0xFFFFFF) / 32.0f);
	

	//Setup the camera(ie the world matrix)
	vectorf cameraEye = {0.0f, 0.0f, 10.0f, 1.0f };
	vectorf cameraLookAt = {0.0f, 0.0f, -10.0f, 1.0f};
	vectorf cameraUp = { 0.0f, 1.0f, 0.0f, 1.0f};
	MatrixManager->setMode(CAMERA);
	MatrixManager->loadIdentity();
	//MatrixManager->setCamera(cameraEye, cameraLookAt, cameraUp);
	MatrixManager->dump("Camera.txt");

	//Setup color matrix(test)
	vectorf color0 = {0.4f, 0.4f, 0.4f, 1.0f};
	vectorf color1 = {0.4f, 0.4f, 0.4f, 1.0f};
	vectorf color2 = {0.4f, 0.4f, 0.4f, 1.0f};
	vectorf ambient = {0.2f, 0.2f, 0.2f, 0.0f};
	MatrixManager->setMode(COLOR);
	MatrixManager->setColor(color0, color1, color2, ambient);
	MatrixManager->dump("Color.txt");

	//Setup light matrix(test)
	vectorf lightd0 = {1.0f, 0.0f, 0.3f, 0.0f};
	vectorf lightd1 = {0.0f, 1.0f, 0.3f, 0.0f};
	vectorf lightd2 = {0.0f, 0.0f, 1.0f, 0.0f};
	
	MatrixManager->setMode(LIGHT);
	MatrixManager->setLight(lightd0, lightd1, lightd2);
	MatrixManager->dump("Light.txt");

	return 1;
}

int shutDownGame()
{

	//Shut down the joypad library
	DmaMemoryScheduler->dump("DmaSche.txt");

	printf("Drawing Finished\n");
	//Shutdown the video
	sps2UScreenShutdown();


	printf("Delete Input Engine: ");
	delete InputEngine;
	printf("done\n");

	printf("Delete Game Clock Engine: ");
	delete GameClockEngine;
	printf("done\n");

	printf("Delete Keyframe Engine: ");
	//KeyframeEngine->getSecondPacket(0)->dump("KeyframeModel01.txt");
	delete KeyframeEngine;
	printf("done\n");

	printf("Delete Level Engine: ");
	LevelEngine->getPacket(0)->dump("TestMap.txt"); 
	delete LevelEngine;
	printf("done\n");
	
	printf("Deleting VU Manager: ");
	delete VUManager;
	printf("done\n");

	printf("Deleting Texture Engine: ");
	TextureEngine->getPacket(0)->dump("Texture01.txt");
	TextureEngine->getPacket(1)->dump("Texture02.txt");
	delete TextureEngine;
	printf("done\n");

	printf("Deleting Matrix Manager: ");
	delete MatrixManager;
	printf("done\n");

	printf("Deleting Memory Engine: ");
	DmaMemoryScheduler->dump("Scheduler.txt");
	delete DmaMemoryScheduler;

	//Must delete the memory manager last.
	delete DmaMemoryEngine;
	printf("done\n");

	return 1;
}