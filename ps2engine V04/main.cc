#include <stdio.h>
#include "dmascheduler.h"
#include "dmapacket.h"
#include "dmamemory.h"
#include "addregisters.h"
extern "C" {
#include "joypadlib.h"
}

Dma_Memory_Engine *DmaMemoryEngine;
Dma_Memory_Scheduler *DmaMemoryScheduler;

int main()
{
	bool done = false;
	int active_joypad = 0;
	int buttons;
	float button_pressure[PS2PAD_NUM_PRESSURE_BUTTONS];
	float joysticks1[ 2 ][ 2 ];
	float joysticks2[ 2 ][ 2 ];
	int x,y;
	int posx, posy;

	Dma_Packet *testPacket;
	Dma_Packet *testPacket1;
	Dma_Packet *testPacket2;
	Dma_Packet *testPacket3;
	//Dma_Packet *sendPacket;

	sps2GIFTag_t GifTag;

	sps2GIFPackedRegister_t Register;
	//sps2DMATag_t pDMATag;

	joypadlib_initialize();

	printf("\nPS2 engine - test program\n");
	
	printf("Create Memory Engine: ");
	DmaMemoryEngine = new Dma_Memory_Engine(3072);
	DmaMemoryScheduler = new Dma_Memory_Scheduler();
	printf("done\n");

	printf("Create Dma Packet: ");
	//sendPacket = new Dma_Packet(1,2,1);
	testPacket1 = new Dma_Packet(1,2,0);
	testPacket2 = new Dma_Packet(1,2,0);
	testPacket3 = new Dma_Packet(1,2,0);
	
	DmaMemoryScheduler->add(testPacket1);

	DmaMemoryScheduler->add(testPacket2);

	DmaMemoryScheduler->add(testPacket3);

	printf("done\n");

	for (y = 0; y < 3; y++)
	{
	printf("Select Dma Packet...");
	if (y == 0)testPacket = testPacket1;
	if (y == 1)testPacket = testPacket2;
	if (y == 2)testPacket = testPacket3;
	printf("done\n");
	//Draw 200 triangles per screen;
	GifTag = AddGifTag(3 * 8000, 1, 1, 11, GIF_FLG_PACKED, 0);
	
	printf("Add data to gif tag...");
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
	// Add the XYZ2 register to the giftag. 
	SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
	printf("done\n");

	printf("Add gif tag...");
	testPacket->add((uint128)GifTag.i128);
	printf("done\n");
	for (x = 0; x < 8000; x++)
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


	sps2UScreenInit(0);
	y = 0;

	while(!done)
	{

		joypadlib_update();
		buttons = joypadlib_get_joypad_buttons( active_joypad, button_pressure, PRESSURE_TYPE_FLOAT );
		if( button_pressure[ PS2PAD_PRESSURE_TRIANGLE ] > 0.0f )
			done = true;
		else if( button_pressure[ PS2PAD_PRESSURE_SQUARE ] > 0.0f )
			printf("Square Pressed\n");
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
				testPacket1->setSend(1);
				testPacket2->setSend(1);
				testPacket3->setSend(1);
				break;
			}

			}
			y++;
			if (y >= 4) y = 0;
			
		}

		sps2UScreenClear(0x80, 0x80, 0x80);

		//DmaMemoryScheduler->dump("Scheduler.txt");
		DmaMemoryScheduler->buildList(2);
		DmaMemoryScheduler->transfer(2);
		DmaMemoryScheduler->waitForTransfer(2);
		sps2UScreenSwap();





	}

	
	joypadlib_shutdown();

	//DmaMemoryScheduler->dump();

	printf("Drawing Finished\n");

	sps2UScreenShutdown();

	printf("Deleting Dma Packet: ");
	delete testPacket1;
	delete testPacket2;
	delete testPacket3;
	printf("done\n");


	printf("Deleting Memory Engine: ");
	delete DmaMemoryScheduler;
	delete DmaMemoryEngine;
	printf("done\n");

	return 0;

}

