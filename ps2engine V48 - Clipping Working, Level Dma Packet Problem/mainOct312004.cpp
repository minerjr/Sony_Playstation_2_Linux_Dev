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


Dma_Memory_Engine *DmaMemoryEngine;
Dma_Memory_Scheduler *DmaMemoryScheduler;
VU_Manager *VUManager;
Matrix_Manager *MatrixManager;
Texture_Engine *TextureEngine;
Game_Clock_Engine *GameClockEngine;
Level_Engine *LevelEngine;
Keyframe_Model_Engine *KeyframeEngine;
Input_Engine *InputEngine;

int main()
{
	bool done = false;
	float axisz = -100.0f;
	float forward = 0.0f;
	float axisx = 0.0f;
	float axisy = 0.0f;
	float angz = 0.0f;
	int lightFlg = NORMAL_T;

	PS2Pad *joypad;

	Custom_Model *TestModel;

	Custom_Model *TestVuModel;

	
	Dma_Packet *TestPacket;
	sps2GIFTag_t GIFTag;
	sps2GIFPackedRegister_t Register;
	

	printf("\nPS2 engine - test program\n");
	
	printf("Create Input Engine: ");
	InputEngine = new Input_Engine();
	joypad = InputEngine->getInputDevice();
	printf("done\n");

	printf("Create Game Clock Engine: ");
	GameClockEngine = new Game_Clock_Engine();
	printf("done\n");

	printf("Create Memory Engine: ");
	//Allocate 12 megs for dma memory, IE textures and models ( 3072 * 4K)
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

	printf("Create custom model TestModel: ");
	TestModel = new Custom_Model(MatrixManager, CHANNEL2, DmaMemoryEngine, TextureEngine);
	printf("done\n");


	printf("Create custom model TestVuModel: ");
	TestVuModel = new Custom_Model(MatrixManager, CHANNEL1, DmaMemoryEngine, TextureEngine);
	printf("done\n");

	printf("Load Texture: ");
	TextureEngine->loadTexture("data/textures/tris.bmp", 0);
	TestModel->setTextureID( TextureEngine->loadTexture("data/textures/texture02.cif", 1) );
	TextureEngine->loadTexture("data/textures/texture03.bmp", 2);
	printf("done\n");

	
	TestPacket = new Dma_Packet(1,CHANNEL2, 0, DmaMemoryEngine);



	int clutDst = sps2UScreenGetFirstFreeGSPage() << 5;

		//Calculate the number of blocks the clut takes and set
		//the next texture address to it.
		//Cacluation works in this formula.
		//((texture width + block width - 1) / block width) * 
		//	((texture height + block height - 1) / block height)
		//32 bit textures uses 8 for the block height and width.
		//The clut is a 16 by 16 32bit image.
	int textureDst = clutDst + ((16 + 8 - 1) / 8) * ((16 + 8 - 1) / 8);

	printf("clutDst = %d\n", clutDst);
	printf("textureDst = %d\n", textureDst);

	//Gif tag for setting the actual use registers
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 3;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	TestPacket->add((uint128)GIFTag.i128);

	Register.i128 = 0;
	Register.A_D.s.DATA = ALPHA_SET(0, 1, 0, 1, 128);
	Register.A_D.s.ADDR = GIF_A_D_REG_ALPHA_1;
	TestPacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TEX1_SET(1, 0, 1, 1, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TEX1_1;
	TestPacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TEX0_SET(textureDst, 4, PSM_PSMT8, 8, 8, 1, 0, clutDst, 0, 0, 0, 1);
	Register.A_D.s.ADDR = GIF_A_D_REG_TEX0_1;
	TestPacket->add((uint128)Register.i128);

  
	//Create Test Sprite for texture
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 1;
	GIFTag.s.PRE = 1;
	GIFTag.s.EOP = 1;
	GIFTag.s.PRIM = GS_PRIM(PRIM_SPRITE, 
						PRIM_IIP_FLAT, 
						PRIM_TME_ON,
						PRIM_FGE_OFF, 
						PRIM_ABE_ON, 
						PRIM_AA1_OFF, 
						PRIM_FST_UV, 
						PRIM_CTXT_CONTEXT1, 
						PRIM_FIX_NOFIXDDA);
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 5; 
	GIFTag.s.REGS = GIF_REG_RGBAQ | (GIF_REG_UV << 4) | (GIF_REG_XYZ2 << 8) | (GIF_REG_UV << 12) | (GIF_REG_XYZ2 << 16);

	TestPacket->add((uint128)GIFTag.i128);
	TestPacket->add((uint128)AddColour(0x80, 0x80, 0x80, 0x80).i128);
	TestPacket->add((uint128)AddUV(0, 0).i128);
	TestPacket->add((uint128)AddVertex(-128, -128, 100, 0).i128);
	TestPacket->add((uint128)AddUV(256, 256).i128);
	TestPacket->add((uint128)AddVertex(128,  128, 100, 0).i128);
	TestPacket->finish();
	
	

	//Setup the view port matrix
	MatrixManager->setMode(VIEW);
	MatrixManager->setView((float)sps2UScreenGetWidth(), (float)sps2UScreenGetHeight(), 2048.0f, 2048.0f, (float)(1 << 24));
	MatrixManager->dump("View.txt");
	
	//Setup the projection matrix
	MatrixManager->setMode(PROJECTION);
	MatrixManager->setProjection(60.0f, 4.0f / 3.0f, -32.0f, -1000.0f);
	MatrixManager->dump("Projection.txt");

	MatrixManager->setProjectionScale(2048.0f, 2048.0f, ((float)0xFFFFFF) / 32.0f);
	

	//Setup the camera(ie the world matrix)
	vectorf cameraEye = {0.0f, 0.0f, 0.0f, 1.0f };
	vectorf cameraLookAt = {0.0f, 0.0f, -10.0f, 1.0f};
	vectorf cameraUp = { 0.0f, 1.0f, 0.0f, 1.0f};
	MatrixManager->setMode(CAMERA);
	MatrixManager->setCamera(cameraEye, cameraLookAt, cameraUp);
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


	//Set the matrix mode to model view
	MatrixManager->setMode(MODELVIEW);
	MatrixManager->loadIdentity();



	//MatrixManager->scale(0.00005f, 0.00005f, 0.00005f);
	MatrixManager->rotate(-PI / 2,0.0f, 1.0f, 0.0f);
	MatrixManager->translate(0.0f, 0.0f, -100.0f);
	


	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);

	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);
	KeyframeEngine->buildCharacter(0,0);

	MatrixManager->scale(50.0f, 50.0f, 50.0f);
	
	MatrixManager->translate(0.0f, -60.0f, 0.0f);
	//MatrixManager->rotate(-PI / 4, 0.0f, 1.0f, 0.0f);
	KeyframeEngine->buildCharacter(1,0);

	MatrixManager->push();
	MatrixManager->loadIdentity();
	MatrixManager->translate(0.0f, -512.0f, axisz - 1000.0f);
	LevelEngine->load("data/maps/Level01.bsp");
	MatrixManager->pop();

	//Add the texture to the dma scheduler

	DmaMemoryScheduler->add(TextureEngine->getPacket(2));
	//DmaMemoryScheduler->add(LevelEngine->getPacket(0));


	DmaMemoryScheduler->add(TextureEngine->getPacket(0));

	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(0));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(0));	

	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(1));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(1));	
	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(2));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(2));	
	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(3));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(3));	
	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(4));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(4));	

	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(5));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(5));
	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(6));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(6));
	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(7));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(7));
	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(8));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(8));
	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(9));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(9));



	DmaMemoryScheduler->add(KeyframeEngine->getFirstPacket(12));
	DmaMemoryScheduler->add(KeyframeEngine->getSecondPacket(12));

	//DmaMemoryScheduler->add(TestPacket);
	DmaMemoryScheduler->add(TextureEngine->getPacket(1));
	//Add the custom model, TestModel, to the dma scheduler
	DmaMemoryScheduler->add(TestModel->getModelPacket());

	//Add the custom model, TestVuModel, to the dma scheduler
	//DmaMemoryScheduler->add(TestVuModel->getModelPacket());

	printf("done\n");
	

	printf("Drawing Started\n");


	
	//done = 1;

	while(!done)
	{
		forward = 0.0f;

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
			
			if(joypad->IsButtonPressed(PS2Pad::BUTTON_SQUARE))
			{
		
				forward += 1.0f;
			}
			else if(joypad->IsButtonPressed(PS2Pad::BUTTON_CROSS))
			{
				forward -= 1.0f;
			}

			if(joypad->IsButtonPressed(PS2Pad::BUTTON_L1))
			{
		
				angz += 0.1f;
				if (angz > PI)angz = -PI;

			}
			else if(joypad->IsButtonPressed(PS2Pad::BUTTON_R1))
			{
				angz -= 0.1f;
				if (angz < -PI)angz = PI;
			}
			if(joypad->IsButtonPressed(PS2Pad::BUTTON_L2))
			{
		
				axisz += 2.0;

			}
			else if(joypad->IsButtonPressed(PS2Pad::BUTTON_R2))
			{
				axisz -= 2.0;
			}
		}

		//Simple two triangles using gl like commands
		//See custommodel.h for more details
		TestModel->begin(TRIANGLE, VERTEX_T | COLOR_T | ST_T | lightFlg);
		{
			//Set the matrix mode to model view
			MatrixManager->setMode(MODELVIEW);
			MatrixManager->loadIdentity();
			//Try out the scale matrix function
			MatrixManager->scale(0.5f, 0.5f, 0.5f);
			//Update the position of the square
			axisx += sinf(angz) * forward;
			axisy += -cosf(angz) * forward;
			printf("X: %f Y: %f Z: %f ZROT: %f\n", axisx, axisy, axisz, angz);
			//Rotate the square along its z coordinate
			MatrixManager->rotate(angz, 1.0f, 0.0f, 0.0f);
			//Do the matrix's translations
			MatrixManager->translate(axisx, axisy, axisz);
 
			TestModel->addNormal(0.0f, 0.5f, -0.5f);
			//Must add color first then the vertex data
			TestModel->addST(0.0f, 0.0f);
			TestModel->addColor(0.0f, 1.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f,20.0f, 20.0f);

			TestModel->addST(0.0f, 1.0f);
			TestModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, -20.0f, 20.0f);

			TestModel->addST(1.0f, 0.0f);
			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, 20.0f, 20.0f);


			TestModel->addST(0.0f, 1.0f);
			TestModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, -20.0f, 20.0f);

			TestModel->addST(1.0f, 0.0f);
			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, 20.0f, 20.0f);

			TestModel->addST(1.0f, 1.0f);
			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, -20.0f, 20.0f);


		
		}
		//For every begin, there must be an end call
		TestModel->end();


		//Using vu 1 to do the transformations
		//Simple two triangles using gl like commands
		//See custommodel.h for more details
		
		MatrixManager->setMode(MODELVIEW);
		MatrixManager->loadIdentity();
		//Try out the scale matrix function
		MatrixManager->scale(0.5f, 0.5f, 0.5f);
		//Update the position of the square
		axisx += sinf(angz) * forward;
		axisy += -cosf(angz) * forward;
		printf("X: %f Y: %f Z: %f ZROT: %f\n", axisx, axisy, axisz, angz);
		//Rotate the square along its z coordinate
		MatrixManager->rotate(angz, 0.0f, 0.0f, 1.0f);
		//Do the matrix's translations

		MatrixManager->translate(axisx + 20.0f, axisy, axisz);
		
		TestVuModel->begin(TRIANGLE, VERTEX_T | COLOR_T | NORMAL_T | ST_T);
		
		{
			//Set the matrix mode to model view


			//Must add vertexs first
			TestVuModel->addVertex(-20.0f, 20.0f, 20.0f);
			TestVuModel->addVertex(-20.0f, -20.0f, 20.0f);
			TestVuModel->addVertex(20.0f, 20.0f, 20.0f);
			TestVuModel->addVertex(-20.0f, -20.0f, 20.0f);
			TestVuModel->addVertex(20.0f, 20.0f, 20.0f);
			TestVuModel->addVertex(20.0f, -20.0f, 20.0f);


			//Then the normals
			TestVuModel->addNormal(0.0f, 0.5f, -0.5f);
			TestVuModel->addNormal(0.0f, 0.5f, -0.5f);
			TestVuModel->addNormal(0.0f, 0.5f, -0.5f);
			TestVuModel->addNormal(0.0f, 0.5f, -0.5f);
			TestVuModel->addNormal(0.0f, 0.5f, -0.5f);
			TestVuModel->addNormal(0.0f, 0.5f, -0.5f);


			//Finally the colors
			TestVuModel->addColor(0.0f, 1.0f, 1.0f, 1.0f);
			TestVuModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestVuModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestVuModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestVuModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestVuModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
	

			TestVuModel->addST(0.0f, 0.0f);
			TestVuModel->addST(0.0f, 1.0f);
			TestVuModel->addST(1.0f, 0.0f);
			TestVuModel->addST(0.0f, 1.0f);
			TestVuModel->addST(1.0f, 0.0f);
			TestVuModel->addST(1.0f, 1.0f);
		}
		
		//For every begin, there must be an end call
		TestVuModel->end();
		
		
		MatrixManager->setMode(MODELVIEW);
		MatrixManager->loadIdentity();
		//Try out the scale matrix function
		//MatrixManager->scale(0.5f, 0.5f, 0.5f);
		//Update the position of the square
		axisx += sinf(angz) * forward;
		axisy += -cosf(angz) * forward;
		//printf("X: %f Y: %f Z: %f ZROT: %f\n", axisx, axisy, axisz, angz);
		//Rotate the square along its z coordinate
		MatrixManager->rotate(angz, 0.0f, 0.0f, 1.0f);
		//MatrixManager->rotate(PI / 2, 0.0f, 0.0f, 1.0f);
		MatrixManager->translate(axisx, axisy, axisz);
		

		LevelEngine->updateLevel(0);
		
		

		//Clear the screen with the default color
		sps2UScreenClear(0x00, 0x00, 0x00);

		
		//Build the list of things to transfer along
		//Dma channel 1 and 2
		/*
		DmaMemoryScheduler->buildList(CHANNEL2);
		//Start the transfer along channel 2
		DmaMemoryScheduler->transfer(CHANNEL2);
		//Wait for the transfer to finish
		DmaMemoryScheduler->waitForTransfer(CHANNEL2);
		*/
		
		DmaMemoryScheduler->buildList(CHANNEL1);
		//Start the transfer along channel 1
		DmaMemoryScheduler->transfer(CHANNEL1);
		//Wait for the transfer to finish

		
		MatrixManager->setMode(MODELVIEW);
		MatrixManager->loadIdentity();
		//Try out the scale matrix function
		//MatrixManager->scale(0.5f, 0.5f, 0.5f);
		//Update the position of the square
		axisx += sinf(angz) * forward;
		axisy += -cosf(angz) * forward;
		printf("X: %f Y: %f Z: %f ZROT: %f\n", axisx, axisy, axisz, angz);
		//Rotate the square along its z coordinate
		MatrixManager->rotate(angz, 0.0f, 0.0f, 1.0f);
		MatrixManager->rotate(-PI / 2,0.0f, 1.0f, 0.0f);
		MatrixManager->translate(axisx + 20.0f, axisy, axisz);		

		KeyframeEngine->runAnimation(0, 1);
		KeyframeEngine->flipSendPacket(0);


		//if (updateCount == 0)
		{
			MatrixManager->push();
			MatrixManager->translate(-40.0f, 0.0f, 0.0f);		
			KeyframeEngine->runAnimation(1, 2);
			KeyframeEngine->flipSendPacket(1);
			MatrixManager->pop();
		}
		
		//else if (updateCount == 1)
		{
			MatrixManager->push();
			MatrixManager->translate(-40.0f, -40.0f, 0.0f);		
			KeyframeEngine->runAnimation(2, 3);
			KeyframeEngine->flipSendPacket(2);
			MatrixManager->pop();
		}
		
		//else if (updateCount == 2)
		{
			MatrixManager->push();
			MatrixManager->translate(0.0f, -40.0f, 0.0f);		
			KeyframeEngine->runAnimation(3, 4);
			KeyframeEngine->flipSendPacket(3);
			MatrixManager->pop();
		}
		
		//else 
		{	
			MatrixManager->push();
			MatrixManager->translate(30.0f, -40.0f, 0.0f);		
			KeyframeEngine->runAnimation(4, 5);
			KeyframeEngine->flipSendPacket(4);
			MatrixManager->pop();
			
		}

		MatrixManager->translate(20.0f, -30.0f, 0.0f);		
		KeyframeEngine->runAnimation(5, 6);
		KeyframeEngine->flipSendPacket(5);

	
		//if (updateCount == 0)
		{
			MatrixManager->push();
			MatrixManager->translate(-40.0f, 0.0f, 0.0f);		
			KeyframeEngine->runAnimation(6, 7);
			KeyframeEngine->flipSendPacket(6);
			MatrixManager->pop();
		}
		
		//else if (updateCount == 1)
		{
			MatrixManager->push();
			MatrixManager->translate(-40.0f, -40.0f, 0.0f);		
			KeyframeEngine->runAnimation(7, 8);
			KeyframeEngine->flipSendPacket(7);
			MatrixManager->pop();
		}
		
		//else if (updateCount == 2)
		{
			MatrixManager->push();
			MatrixManager->translate(0.0f, -40.0f, 0.0f);		
			KeyframeEngine->runAnimation(8, 9);
			KeyframeEngine->flipSendPacket(8);
			MatrixManager->pop();
		}
		
		//else 
		{	
			MatrixManager->push();
			MatrixManager->translate(30.0f, -40.0f, 0.0f);		
			KeyframeEngine->runAnimation(9, 10);
			KeyframeEngine->flipSendPacket(9);
			MatrixManager->pop();
			
		}

		


		DmaMemoryScheduler->waitForTransfer(CHANNEL1);
		
		
		//Flip the video buffers
		sps2UScreenSwap();


	}

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

	printf("Deleting TestModel: ");
	TestModel->getModelPacket()->dump("TestModel.txt");
	TestVuModel->getModelPacket()->dump("TestVuModel.txt");
	delete TestModel;
	delete TestVuModel;
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

	return 0;

}

