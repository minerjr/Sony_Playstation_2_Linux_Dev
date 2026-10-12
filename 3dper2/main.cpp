#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>
#include <signal.h>
#include <assert.h>
#include <math.h>
#include "PS2Defines.h"
#include "PS2Types.h"
#include "main.h"
#include "sps2wrap.h"
#include "AddRegisters.h"
#include "ctexture.h"
#include "packet.h"
#include "pad.h"
#include "vecmaths.h"
#include "font.h"
#include "Ctimer.h"


// The SPS2 Wrapper Class
CSPS2Manager Manager;

// Signal handler - exit on all signals
void signal_handler(int signum)
{
	printf("\nReceived interrupt signal %d.\n", signum);
	Manager.SetRunningStatus(false);
}

int main(void)
{
	//Initialise SPS2 with 640 4k Blocks of memory (2.56 MBytes)
	Manager.Initialise(640);
	
	DmacControl_t Dmac;			// DMAC control structure
	PS2Sprite_t Sprite[2];		// some sprite structures to play with
	
	//initialise the pads
	if(pad_init (PAD_0, PAD_INIT_LOCK |PAD_INIT_ANALOGUE | PAD_INIT_PRESSURE) == 0)
		{
		printf("Unable to initialise the pads\n");
		pad_cleanup (PAD_0);
		exit(0);
		}
	
	// Performance timer - call after screen init.
	CTimer Timer;				
	
	// The font class Pass in the sps2manager
	CFont Font(&Manager);
	
	// Initialise font class and load the texture into SPS2 memory.
	Font.Initialise("font.bmp", "font.dat", sps2UScreenGetFirstFreeGSPage(), true);
	
	// Initialise the texture class for texture 1
	CTexture Tex1(Manager.GetTex1Base(), Manager.GetTex1DmaBase(), &Manager);
	
	// Load game bitmap into SPS2 memory
	if(!Tex1.LoadBitmap("game.bmp", true, false))
	{
		printf("Failed to load texture.");
		return -1;
	}

	//upload the game texture to the first free GS Page
	//Tex1.Upload();
	
	// make sure all data is written to SPS2 memory from the cache
	Manager.FlushCache();
	
	//Configure the Sprite Structures with some sensible data
	Sprite[0].Pos.v[0] = 0.0f;
	Sprite[0].Pos.v[1] = 0.0f;
	Sprite[0].Pos.v[2] = -100.0f;
	Sprite[0].Pos.v[3] = 1.0f;
	
	
	IdentityMatrix(&Sprite[0].World);
	//untransformed positions
	
	SetVector(&Sprite[0].Vertex[0].Vertex, -10.0f,  10.0f, 0.0f, 1.0f); // Top Left
	SetVector(&Sprite[0].Vertex[1].Vertex, -10.0f, -10.0f, 0.0f, 1.0f); // Bottom Left
	SetVector(&Sprite[0].Vertex[2].Vertex,  10.0f,  10.0f, 0.0f, 1.0f); // Top Right
	SetVector(&Sprite[0].Vertex[3].Vertex,  10.0f, -10.0f, 0.0f, 1.0f); // Bottom Right
	
	Sprite[0].Vertex[0].s = 0.125f;		// Top Left
	Sprite[0].Vertex[0].t = 0.0f;
	Sprite[0].Vertex[1].s = 0.125f; 	// Bottom Left
	Sprite[0].Vertex[1].t = 0.125f;
	Sprite[0].Vertex[2].s = 0.25f; 		// Top Right
	Sprite[0].Vertex[2].t = 0.0f;
	Sprite[0].Vertex[3].s = 0.25f; 		// Bottom Right
	Sprite[0].Vertex[3].t = 0.125f;
	
	Sprite[0].Vertex[0].r = Sprite[0].Vertex[0].g = Sprite[0].Vertex[0].b = Sprite[0].Vertex[0].a = 0x80;
	Sprite[0].Vertex[1].r = Sprite[0].Vertex[1].g = Sprite[0].Vertex[1].b = Sprite[0].Vertex[1].a = 0x80; 
	Sprite[0].Vertex[2].r = Sprite[0].Vertex[2].g = Sprite[0].Vertex[2].b = Sprite[0].Vertex[2].a = 0x80; 
	Sprite[0].Vertex[3].r = Sprite[0].Vertex[3].g = Sprite[0].Vertex[3].b = Sprite[0].Vertex[3].a = 0x80; 

	Sprite[0].RotX = 0.0f;
	Sprite[0].RotY = 0.0f;
	Sprite[0].RotZ = 0.0f;
	
	Sprite[0].Scale = 1.0f;
	
	// Configure another sprite to draw 
	// Draw this one about half way down the view volume
	Sprite[1] = Sprite[0];
	Sprite[1].Pos.v[0] = 0.0f;
	Sprite[1].Pos.v[1] = 0.0f;
	Sprite[1].Pos.v[2] = -500.0f;
		
	//Scale up this sprite to a bigger size
	Sprite[1].Scale = 20.0f;
	
	// Set to different texture coordinates
	Sprite[1].Vertex[0].s = 0.0f; 		// Top Left
	Sprite[1].Vertex[0].t = 0.0f;
	Sprite[1].Vertex[1].s = 0.0f; 		// Bottom Left
	Sprite[1].Vertex[1].t = 0.125f;
	Sprite[1].Vertex[2].s = 0.125f; 	// Top Right
	Sprite[1].Vertex[2].t = 0.0f;
	Sprite[1].Vertex[3].s = 0.125f; 	// Bottom Right
	Sprite[1].Vertex[3].t = 0.125f;
	
	// Set the required DMAC register values 
	// Clear the register to zero	
	Dmac.madr.i32 = 0;		
	// Set the address field to the physical address of the allocated mem
	Dmac.madr.s.ADDR = Manager.GetPhysicalAddress(Manager.GetPrim1Base());
	
	// The channel control register
	Dmac.chcr.i32 = 0;
	Dmac.chcr.s.MOD = CHCR_MOD_NORMAL;	// Normal mode transfer
	Dmac.chcr.s.STR = 1;					// Start the transfer
	
	//install a signal handler to capture all signals
	//printf("\n\nPress Ctrl-c to quit the program\n\n");
	for(int iSignal = 0; iSignal<128; iSignal++)
	{
		signal(iSignal, signal_handler);
	}
		
	// variable for the game loop
	float fps;	
	int iFrame = 0;
	
	// The main render loop
	do
	{
		//start the time
  		Timer.PrimeTimer();
		
		//clear the current draw buffer
		Manager.BeginScene(0x10, 0x00, 0x20);
		
		//update the pad structure
		GetUserInput();
		
		//perform the game logic
		GameLogic(Sprite);
		
		//upload the font texture
		Font.Begin();
		
		//render the font info
		Font.Render(-150, -250, \
					"Manipulation of TriStrip in 3D", \
					0x30, 0x10, 0x10, 0x80);
		Font.printf(180, -250, 0,  0x50,  0x50, 0x50, 
					"Frame: %d\nFPS: %.1f" , iFrame++, fps);
		
		//upload the game texture
		Tex1.Upload();
		
		//render the game data
		DrawFrame((char *)Manager.GetPrim1Base(), Sprite, &Dmac);
		
		//calculate the current frame fate in frames per second	
		fps = Timer.GetFPS();	
				
		//swap the screen buffers
		Manager.EndScene();
		
		// Get out methods 
		if((pad[0].buttons & PAD_START)&&(pad[0].buttons & PAD_SELECT))
								Manager.SetRunningStatus(false);
		if(!Manager.GetRunningStatus())break;
		
	}while(true);

	printf("Exiting.............\n");
	
	pad_cleanup (PAD_0);
	
	return 0;
}

//update the controller pad structures
void GetUserInput(void)
{
	pad_update (PAD_0);
	
}

void GameLogic(PS2Sprite_t * pSP)
{
	//rotate the sprite round z axis
	if(pad[0].buttons & PAD_RIGHT)pSP[0].RotZ += pad[0].pressures[PAD_PRIGHT] * 0.2f;
	if(pad[0].buttons & PAD_LEFT) pSP[0].RotZ -= pad[0].pressures[PAD_PLEFT] * 0.2f;
	
	//rotate the sprite round y axis
	if(pad[0].buttons & PAD_UP)pSP[0].RotY += pad[0].pressures[PAD_PUP] * 0.2f;
	if(pad[0].buttons & PAD_DOWN) pSP[0].RotY -= pad[0].pressures[PAD_PDOWN] * 0.2f;
	
	//rotate the sprite round x axis
	if(pad[0].buttons & PAD_L1)pSP[0].RotX += pad[0].pressures[PAD_PL1] * 0.2f;
	if(pad[0].buttons & PAD_L2) pSP[0].RotX -= pad[0].pressures[PAD_PL2] * 0.2f;

	//move along local x and animate the sprite
	if(pad[0].buttons & PAD_SQUARE)
	{
		MoveSprite(&pSP[0], -pad[0].pressures[PAD_PSQUARE] * 3.0f, XAXIS);
		
	}
	if(pad[0].buttons & PAD_CIRCLE)
	{
		MoveSprite(&pSP[0], pad[0].pressures[PAD_PCIRCLE] * 3.0f, XAXIS);
		
	}
	
	//move along local y and animate the sprite
	if(pad[0].buttons & PAD_TRI)
	{
		MoveSprite(&pSP[0], pad[0].pressures[PAD_PTRI] * 3.0f, YAXIS);
		
	}
	if(pad[0].buttons & PAD_CROSS)
	{
		MoveSprite(&pSP[0], -pad[0].pressures[PAD_PCROSS] * 3.0f, YAXIS);
		
	}
	
	//move along local z and animate the sprite
	if(pad[0].buttons & PAD_R1)
	{
		MoveSprite(&pSP[0], pad[0].pressures[PAD_PR1] * 3.0f, ZAXIS);
		
	}
	if(pad[0].buttons & PAD_R2)
	{
		MoveSprite(&pSP[0], -pad[0].pressures[PAD_PR2] * 3.0f, ZAXIS);
		
	}
	
	// Reset to default position
	if((pad[0].buttons & PAD_R3)||(pad[0].buttons & PAD_L3))
	{
		pSP[0].RotX = 0.0f;
		pSP[0].RotY = 0.0f;
		pSP[0].RotZ = 0.0f;
		
		pSP[0].Pos.v[0] = 0.0f;
		pSP[0].Pos.v[1] = 0.0f;
		pSP[0].Pos.v[2] = -100.0f;
	}
}



void DrawFrame(char * pMemStart, PS2Sprite_t * pSP, DmacControl_t * pD)
{
	char * pMemEnd;
	
	pMemEnd = pMemStart;
	
	//build the GS packet
	BuildSprite2D(&pSP[1], pMemEnd);
	BuildSprite2D(&pSP[0], pMemEnd);
		
	//calculate how many qwords have been written
	int iQWC = ((int)pMemEnd - (int)pMemStart) >> 4;

	// Assert that we haven't written more data than we allocated
	assert(iQWC < (4096 >> 4));	
	
	// Important: Make sure all the data is written to the physical memory
	Manager.FlushCache();
	
	// DMA Channel 2 is the one we want, it goes to the GS
	// Amount of data to be transmitted
	*EE_D2_QWC = iQWC;	

	// Set the memory address register
	*EE_D2_MADR = pD->madr.i32;
		
	// Set the register to start the transfer
	*EE_D2_CHCR = pD->chcr.i32;	

	// Wait for it to complete
	Manager.WaitForDMA(2);
}

//move sprite in the direction it is pointing
void MoveSprite(PS2Sprite_t * pSP, float Delta, int Axis)
{
	//this moves the sprite along its local "Axis" axis
	// by an amount Delta
	pSP->Pos.v[0] += pSP->World.m[0][Axis]* Delta;
	pSP->Pos.v[1] += pSP->World.m[1][Axis]* Delta;
	pSP->Pos.v[2] += pSP->World.m[2][Axis]* Delta;
}


