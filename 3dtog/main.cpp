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

// Transformation matrices
Camera_t Cam;
FMatrix ViewScreen, ViewPortTrans;

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
	PS2Sprite_t Sprite[10];		// some sprite structures to play with
	
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
	Sprite[0].Pos.v[2] = -400.0f;
	Sprite[0].Pos.v[3] = 1.0f;
	Sprite[0].w = 32;
	Sprite[0].h = 32;
	
	IdentityMatrix(&Sprite[0].World);
	//untransformed positions
	Sprite[0].TL.v[0] = -(float)(Sprite[0].w/2);;
	Sprite[0].TL.v[1] = -(float)(Sprite[0].h/2);
	Sprite[0].TL.v[2] = 0.0f;
	Sprite[0].TL.v[3] = 1.0f;
	Sprite[0].BL.v[0] = -(float)(Sprite[0].w/2);
	Sprite[0].BL.v[1] =  (float)(Sprite[0].h/2);
	Sprite[0].BL.v[2] = 0.0f;
	Sprite[0].BL.v[3] = 1.0f;
	Sprite[0].TR.v[0] =  (float)(Sprite[0].w/2);
	Sprite[0].TR.v[1] = -(float)(Sprite[0].h/2);
	Sprite[0].TR.v[2] = 0.0f;
	Sprite[0].TR.v[3] = 1.0f;
	Sprite[0].BR.v[0] = (float)(Sprite[0].w/2);
	Sprite[0].BR.v[1] = (float)(Sprite[0].h/2);
	Sprite[0].BR.v[2] = 0.0f;
	Sprite[0].BR.v[3] = 1.0f;
	
	Sprite[0].u = 32;
	Sprite[0].v = 0;
	Sprite[0].r = 0x80;
	Sprite[0].g = 0x80;
	Sprite[0].b = 0x80;
	Sprite[0].a = 0x80;
	Sprite[0].FCountMax = 5;	
	Sprite[0].FCount = 0;
	Sprite[0].NFrames = 8;
	Sprite[0].RotX = 0.0f;
	Sprite[0].RotY = 0.0f;
	Sprite[0].RotZ = 0.0f;
	
	// Configure another sprite to draw 
	// Draw this one about half way down the view volume
	Sprite[1] = Sprite[0];
	Sprite[1].Pos.v[0] = 0.0f;
	Sprite[1].Pos.v[1] = 0.0f;
	Sprite[1].Pos.v[2] = -500.0f;
	Sprite[1].u = 0;
	Sprite[1].v = 32;
	
	//Scale up this sprite to a bigger size
	FMatrix * pScale;
	pScale = new FMatrix;
	IdentityMatrix(pScale);
	ScaleMatrix(pScale, pScale, 10.0f);
	ApplyMatrix(&Sprite[1].TL, pScale, &Sprite[1].TL);
	ApplyMatrix(&Sprite[1].TR, pScale, &Sprite[1].TR);
	ApplyMatrix(&Sprite[1].BR, pScale, &Sprite[1].BR);
	ApplyMatrix(&Sprite[1].BL, pScale, &Sprite[1].BL);
	delete pScale;
	
	//setup Camera matrix
	Cam.Position.v[0] = 0.0f;
	Cam.Position.v[1] = 0.0f;
	Cam.Position.v[2] = 10.0f;
	Cam.Position.v[3] = 1.0f;

	Cam.LookAt.v[0] = 0.0f;
	Cam.LookAt.v[1] = 0.0f;
	Cam.LookAt.v[2] = 0.0f;
	Cam.LookAt.v[3] = 1.0f;

	Cam.Up.v[0] = 0.0f;
	Cam.Up.v[1] = 1.0f;
	Cam.Up.v[2] = 0.0f;
	Cam.Up.v[3] = 1.0f;

	//set WC matrix
	CalcCameraMatrix(&Cam);
	
	// Configure the View to Screen Matrix
	SetupVS(&ViewScreen, -5.0f, -1000.0f, 4.0f/3.0f, 60.0f);
	
	// Configure the View Port Transformation Matrix
	SetupVT(&ViewPortTrans, (float)Manager.GetScreenWidth(), 
				(float)Manager.GetScreenHeight(), 2048.0f, 2048.0f, (float)(1<<24));
				
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
		AnimateSprite(&pSP[0], pad[0].pressures[PAD_PSQUARE]);
	}
		
	if(pad[0].buttons & PAD_CIRCLE)
	{
		MoveSprite(&pSP[0], pad[0].pressures[PAD_PCIRCLE] * 3.0f, XAXIS);
		AnimateSprite(&pSP[0], pad[0].pressures[PAD_PCIRCLE]);
	}
		
	//move along local y and animate the sprite
	if(pad[0].buttons & PAD_TRI)
	{
		MoveSprite(&pSP[0], pad[0].pressures[PAD_PTRI] * 3.0f, YAXIS);
		AnimateSprite(&pSP[0], pad[0].pressures[PAD_PTRI]);
	}
		
	if(pad[0].buttons & PAD_CROSS)
	{
		MoveSprite(&pSP[0], -pad[0].pressures[PAD_PCROSS] * 3.0f, YAXIS);
		AnimateSprite(&pSP[0], pad[0].pressures[PAD_PCROSS]);
	}
	
	//move along local z and animate the sprite
	if(pad[0].buttons & PAD_R1)
	{
		MoveSprite(&pSP[0], pad[0].pressures[PAD_PR1] * 3.0f, ZAXIS);
		AnimateSprite(&pSP[0], pad[0].pressures[PAD_PR1]);
	}
	
	if(pad[0].buttons & PAD_R2)
	{
		MoveSprite(&pSP[0], -pad[0].pressures[PAD_PR2] * 3.0f, ZAXIS);
		AnimateSprite(&pSP[0], pad[0].pressures[PAD_PR2]);
	}
	
	if((pad[0].buttons & PAD_R3)||(pad[0].buttons & PAD_L3))
	{
		pSP[0].RotX = 0.0f;
		pSP[0].RotY = 0.0f;
		pSP[0].RotZ = 0.0f;
		
		pSP[0].Pos.v[0] = 0.0f;
		pSP[0].Pos.v[1] = 0.0f;
		pSP[0].Pos.v[2] = -400.0f;
			
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


//some very simple animation
void AnimateSprite(PS2Sprite_t * pSP, float Count)
{
	int FCountMax = 8 - (int)(6.0f * Count);
	if((pSP->FCount++)%FCountMax == 0)
		{
			pSP->v += pSP->h;
			if(pSP->v > (pSP->NFrames - 1) * pSP->h)pSP->v = 0;
		}
}
