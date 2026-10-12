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
	float axisz = -100.0f;
	float forward = 0.0f;
	float axisx = 0.0f;
	float axisy = 0.0f;
	float angz = 0.0f;
	int lightFlg = NORMAL_T;

	Custom_Model *TestModel;

	Custom_Model *TestVuModel;

	joypadlib_initialize();

	printf("\nPS2 engine - test program\n");
	
	printf("Create Memory Engine: ");
	//Allocate 12 megs for dma memory, IE textures and models ( 3072 * 4K)
	DmaMemoryEngine = new Dma_Memory_Engine(3072);
	//Setup the dma scheduler, takes care of the complicated transfers
	DmaMemoryScheduler = new Dma_Memory_Scheduler(DmaMemoryEngine);
	printf("done\n");

	printf("Create VU Manager: ");
	//Setup the vu manager used to upload programs and gain accesss the
	//uu's.
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


	printf("Create custom model TestVuModel: ");
	TestVuModel = new Custom_Model(MatrixManager, CHANNEL1, DmaMemoryEngine);
	printf("done\n");



	//Add the custom model, TestModel, to the dma scheduler
	DmaMemoryScheduler->add(TestModel->getModelPacket());

	//Add the custom model, TestVuModel, to the dma scheduler
	DmaMemoryScheduler->add(TestVuModel->getModelPacket());




	//Setup the video
	sps2UScreenInit(0);	

	//Setup the view port matrix
	MatrixManager->setMode(VIEW);
	MatrixManager->setView((float)sps2UScreenGetWidth(), (float)sps2UScreenGetHeight(), 2048.0f, 2048.0f, (float)(1 << 24));
	MatrixManager->dump("View.txt");
	
	//Setup the projection matrix
	MatrixManager->setMode(PROJECTION);
	MatrixManager->setProjection(45.0f, 4.0f / 3.0f, -5.0f, -1000.0f);
	MatrixManager->dump("Projection.txt");
	

	//Setup the camera(ie the world matrix)
	vectorf cameraEye = {0.0f, 0.0f, 10.0f, 1.0f };
	vectorf cameraLookAt = {0.0f, 0.0f, 0.0f, 1.0f};
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


	printf("done\n");
	

	printf("Drawing Started\n");

	
	while(!done)
	{
		forward = 0.0f;

		joypadlib_update();
		buttons = joypadlib_get_joypad_buttons( active_joypad, button_pressure, PRESSURE_TYPE_FLOAT );
		if( button_pressure[ PS2PAD_PRESSURE_TRIANGLE ] > 0.0f )
			done = true;
		else if( button_pressure[ PS2PAD_PRESSURE_CIRCLE ] > 0.0f )
		{
			if(lightFlg)
				lightFlg = 0;
			else
				lightFlg = NORMAL_T;
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

		//Simple two triangles using gl like commands
		//See custommodel.h for more details
		TestModel->begin(TRIANGLE, VERTEX_T | COLOR_T | lightFlg);
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
			MatrixManager->rotate(angz, 0.0f, 0.0f, 1.0f);
			//Do the matrix's translations
			MatrixManager->translate(axisx, axisy, axisz);
 
			TestModel->addNormal(0.0f, 0.5f, -0.5f);
			//Must add color first then the vertex data
			TestModel->addColor(0.0f, 1.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, 20.0f, 20.0f);

			TestModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, -20.0f, 20.0f);

			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, 20.0f, 20.0f);


			TestModel->addColor(0.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(-20.0f, -20.0f, 20.0f);

			TestModel->addColor(1.0f, 0.0f, 1.0f, 1.0f);
			TestModel->addVertex(20.0f, 20.0f, 20.0f);

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
		
		TestVuModel->begin(TRIANGLE, VERTEX_T | COLOR_T | NORMAL_T);
		
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
			


		
		}
		//For every begin, there must be an end call
		TestVuModel->end();
		
		//TestVuModel->createDummy();

		//Clear the screen with the default color
		sps2UScreenClear(0x00, 0x00, 0x00);

		//Build the list of things to transfer along
		//Dma channel 1 and 2
		DmaMemoryScheduler->buildList(CHANNEL2);
		

		//Start the transfer along channel 2
		DmaMemoryScheduler->transfer(CHANNEL2);
		//Wait for the transfer to finish
		DmaMemoryScheduler->waitForTransfer(CHANNEL2);

		DmaMemoryScheduler->buildList(CHANNEL1);
		//Start the transfer along channel 1
		DmaMemoryScheduler->transfer(CHANNEL1);
		//Wait for the transfer to finish
		DmaMemoryScheduler->waitForTransfer(CHANNEL1);
		
		//Flip the video buffers
		sps2UScreenSwap();


	}

	//Shut down the joypad library
	joypadlib_shutdown();

	DmaMemoryScheduler->dump("DmaSche.txt");

	printf("Drawing Finished\n");

	//Shutdown the video
	sps2UScreenShutdown();


	printf("Deleting TestModel: ");
	TestModel->getModelPacket()->dump("TestModel.txt");
	TestVuModel->getModelPacket()->dump("TestVuModel.txt");
	delete TestModel;
	delete TestVuModel;
	printf("done\n");

	printf("Deleting VU Manager: ");
	delete VUManager;
	printf("done\n");

	printf("Deleting Matrix Manager: ");
	delete MatrixManager;
	printf("done\n");

	printf("Deleting Memory Engine: ");
	delete DmaMemoryScheduler;

	//Must delete the memory manager last.
	delete DmaMemoryEngine;
	printf("done\n");

	return 0;

}

