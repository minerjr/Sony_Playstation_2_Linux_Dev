#include <stdio.h>
#include "dmascheduler.h"
#include "dmapacket.h"
#include "dmamemory.h"
#include "addregisters.h"
#include "modelloader.h"
#include "vumgr.h"
#include "matrix.h"
#include "custommodel.h"

extern "C" {
#include "joypadlib.h"
}

Dma_Memory_Engine *DmaMemoryEngine;
Dma_Memory_Scheduler *DmaMemoryScheduler;
VU_Manager *VUManager;
Matrix_Manager *MatrixManager;

int main()
{
	bool done = false;
	int active_joypad = 0;
	int buttons;
	float button_pressure[PS2PAD_NUM_PRESSURE_BUTTONS];
	//float joysticks1[ 2 ][ 2 ];
	//float joysticks2[ 2 ][ 2 ];
	int x,y;
	int posx, posy;
	float axisz = -100.0f;
	float forward = 0.0f;
	float axisx = 0.0f;
	float axisy = 0.0f;
	float angz = 0.0f;

	Dma_Packet *testPacket;
	Dma_Packet *testPacket1;
	Dma_Packet *testPacket2;
	Dma_Packet *testPacket3;
	//Dma_Packet *sendPacket;

	sps2GIFTag_t GifTag;

	sps2GIFPackedRegister_t Register;

	Custom_Model *TestModel;
	//sps2DMATag_t pDMATag;

	joypadlib_initialize();

	printf("\nPS2 engine - test program\n");
	
	printf("Create Memory Engine: ");
	DmaMemoryEngine = new Dma_Memory_Engine(3072);
	DmaMemoryScheduler = new Dma_Memory_Scheduler(DmaMemoryEngine);
	printf("done\n");

	printf("Create VU Manager: ");
	VUManager = new VU_Manager(DmaMemoryEngine, VU0_ENABLED | VU1_ENABLED);
	printf("done\n");

	printf("Create Matrix Manager: ");
	MatrixManager = new Matrix_Manager();
	printf("done\n");

	printf("Upload VU1 micro program: ");
	VUManager->vuUpload(VU1,"libexec/vu1/trans.vu");
	printf("done\n");

	printf("Create custom model TestModel: ");
	TestModel = new Custom_Model(MatrixManager, CHANNEL2, DmaMemoryEngine);
	printf("done\n");

	printf("Create Dma Packet: ");
	//sendPacket = new Dma_Packet(1,2,1, DmaMemoryEngine);
	testPacket1 = new Dma_Packet(1,2,0,DmaMemoryEngine);
	testPacket2 = new Dma_Packet(1,2,0,DmaMemoryEngine);
	testPacket3 = new Dma_Packet(1,2,0,DmaMemoryEngine);
	
	DmaMemoryScheduler->add(testPacket1);

	DmaMemoryScheduler->add(testPacket2);

	DmaMemoryScheduler->add(testPacket3);

	DmaMemoryScheduler->add(TestModel->getModelPacket());
	
	printf("done\n");



	loadModel(testPacket3, "data/models/demon.ms3d");
	//testPacket3->dump("TestPacket3.txt");
	
	sps2UScreenInit(0);	

	MatrixManager->setMode(VIEW);
	MatrixManager->setView((float)sps2UScreenGetWidth(), (float)sps2UScreenGetHeight(), 2048.0f, 2048.0f, (float)(1 << 24));
	MatrixManager->dump("View.txt");
	
	//MatrixManager->setProjection(512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, (float)((1 << 24) -1), 1.0f, 65536.0f); 
	//MatrixManager->setProjection((float)sps2UScreenGetWidth(), (float)sps2UScreenGetHeight(), 1.0f, 65536.0f, 16777215.0f, 0.0f);

	MatrixManager->setMode(PROJECTION);
	MatrixManager->setProjection(45.0f, 4.0f / 3.0f, -5.0f, -1000.0f);
	MatrixManager->dump("Projection.txt");
	

	vectorf cameraEye = {0.0f, 0.0f, 10.0f, 1.0f };
	vectorf cameraLookAt = {0.0f, 0.0f, 0.0f, 1.0f};
	vectorf cameraUp = { 0.0f, 1.0f, 0.0f, 1.0f};
	//vectorf cameraRot = {0.0f, 0.0f, 0.0f, 0.0f};
	MatrixManager->setMode(CAMERA);
	MatrixManager->setCamera(cameraEye, cameraLookAt, cameraUp);
	MatrixManager->dump("Camera.txt");

	//TestModel->getModelPacket()->dump("TestModel.txt");
	//TestModel->getModelPacket()->setSend(1);
	printf("done\n");
	

	for (y = 0; y < 2; y++)
	{
	printf("Select Dma Packet...");
	if (y == 0)testPacket = testPacket1;
	if (y == 1)testPacket = testPacket2;
	if (y == 2)testPacket = testPacket3;
	printf("done\n");
	//Draw 200 triangles per screen;
	GifTag = AddGifTag(3 * 800, 1, 1, 11, GIF_FLG_PACKED, 0);
	
	printf("Add data to gif tag...");
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
	// Add the XYZ2 register to the giftag. 
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
	printf("done\n");

	printf("Add gif tag...");
	testPacket->add((uint128)GifTag.i128);
	printf("done\n");
	for (x = 0; x < 800; x++)
	{
		Register = AddColour(0,0,255,0x80);
	
		testPacket->add((uint128)Register.i128);

		posx = -320 + (rand() % 640);
		posy = -240 + (rand() % 480);
		/*
		printf("Enter the first Vertex: X ");
		scanf("%d",&posx);
		printf("Enter the first Vertex: Y ");
		scanf("%d",&posy);
		*/

		
		Register = AddVertex( posx, posy, 102,0);
	
		testPacket->add((uint128)Register.i128);

		Register = AddColour(255,0,0,0xff);
	
		testPacket->add((uint128)Register.i128);


		posx += 64;//-320 + (rand() % 640);
		posy += 64;//-240 + (rand() % 480);
		/*
		printf("Enter the second Vertex: X: ");
		scanf("%d",&posx);
		printf("Enter the second Vertex: Y: ");
		scanf("%d",&posy);
		*/
		Register = AddVertex( posx, posy, 102,0);
	
		testPacket->add((uint128)Register.i128);


		Register = AddColour(0,255,0,0x80);
	
		testPacket->add((uint128)Register.i128);


		//posx = //-320 + (rand() % 640);
		posy -=64; //-240 + (rand() % 480);

		/*
		printf("Enter the third Vertex: X: ");
		scanf("%d",&posx);
		printf("Enter the third Vertex: Y: ");
		scanf("%d",&posy);
		*/
		Register = AddVertex( posx, posy, 102,0);
	
		testPacket->add((uint128)Register.i128);

	}
	printf("Call Finish...\n");
	testPacket->finish();
	printf("done\n");

	}
	printf("Drawing Started\n");

	//testPacket1->dump("packet1.txt");
	//testPacket2->dump("packet2.txt");
	//testPacket3->dump("packet3.txt");


	
	y = 0;

				testPacket1->setSend(0);
				testPacket2->setSend(0);
				testPacket3->setSend(0);

	while(!done)
	{
		forward = 0.0f;

		joypadlib_update();
		buttons = joypadlib_get_joypad_buttons( active_joypad, button_pressure, PRESSURE_TYPE_FLOAT );
		if( button_pressure[ PS2PAD_PRESSURE_TRIANGLE ] > 0.0f )
			done = true;
		else if( button_pressure[ PS2PAD_PRESSURE_CIRCLE ] > 0.0f )
		{
			switch(y)
			{

			case 0:
			{
				testPacket1->setSend(1);
				testPacket2->setSend(0);
				testPacket3->setSend(0);
				break;
			}

			case 1:
			{
				testPacket1->setSend(0);
				testPacket2->setSend(1);
				testPacket3->setSend(0);
				break;
			}

			case 2:
			{
				testPacket1->setSend(0);
				testPacket2->setSend(0);
				testPacket3->setSend(1);
				break;
			}

			case 3:
			{
				testPacket1->setSend(0);
				testPacket2->setSend(0);
				testPacket3->setSend(0);
				break;
			}

			}
			y++;
			if (y >= 4) y = 0;
			
		}
		
		if( button_pressure[ PS2PAD_PRESSURE_SQUARE ] > 0.0f )
		{
	
			forward += 1.0f;
		}
		else if( button_pressure[ PS2PAD_PRESSURE_CROSS ] > 0.0f )
		{
			forward -= 1.0f;
		}

		if( button_pressure[ PS2PAD_PRESSURE_L1 ] > 0.0f )
		{
	
			angz += 0.1f;
			if (angz > PI)angz = -PI;

		}
		else if( button_pressure[ PS2PAD_PRESSURE_R1 ] > 0.0f )
		{
			angz -= 0.1f;
			if (angz < -PI)angz = PI;
		}
		if( button_pressure[ PS2PAD_PRESSURE_L2 ] > 0.0f )
		{
	
			axisz += 1.0;

		}
		else if( button_pressure[ PS2PAD_PRESSURE_R2 ] > 0.0f )
		{
			axisz -= 1.0;
		}


		TestModel->begin(TRIANGLE, VERTEX_T |COLOR_T);
		{
			MatrixManager->setMode(MODELVIEW);
			MatrixManager->loadIdentity();
			MatrixManager->scale(0.5f, 0.5f, 0.5f);
			axisx += sinf(angz) * forward;
			axisy += -cosf(angz) * forward;
			printf("X: %f Y: %f Z: %f ZROT: %f\n", axisx, axisy, axisz, angz);
			MatrixManager->rotate(angz, 0.0f, 0.0f, 1.0f);

			MatrixManager->translate(axisx, axisy, axisz);

		
			TestModel->addColor(0.0f, 1.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, 20.0f, 20.0f, 1.0f);

			TestModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, -20.0f, 20.0f, 1.0f);

			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, 20.0f, 20.0f, 1.0f);


			TestModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, -20.0f, 20.0f, 1.0f);

			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, 20.0f, 20.0f, 1.0f);

			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, -20.0f, 20.0f, 1.0f);


		
		}
		TestModel->end();




		sps2UScreenClear(0x80, 0x80, 0x80);

		DmaMemoryScheduler->buildList(2);
		DmaMemoryScheduler->transfer(2);
		DmaMemoryScheduler->waitForTransfer(2);

		sps2UScreenSwap();


	}

	
	joypadlib_shutdown();

	DmaMemoryScheduler->dump("DmaSche.txt");

	printf("Drawing Finished\n");

	sps2UScreenShutdown();

	printf("Deleting Dma Packet: ");
	testPacket1->dump("TestPacket3.txt");

	delete testPacket1;
	delete testPacket2;
	delete testPacket3;
	printf("done\n");

	printf("Deleting TestModel: ");
	TestModel->getModelPacket()->dump("TestModel2.txt");
	delete TestModel;
	printf("done\n");

	printf("Deleting VU Manager: ");
	delete VUManager;
	printf("done\n");

	printf("Deleting Matrix Manager: ");
	delete MatrixManager;
	printf("done\n");

	printf("Deleting Memory Engine: ");
	delete DmaMemoryScheduler;
	delete DmaMemoryEngine;
	printf("done\n");

	return 0;

}

