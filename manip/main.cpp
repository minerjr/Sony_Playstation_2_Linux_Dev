#include "sps2.h"
#include <signal.h>
#include "PS2Defines.h"
#include "sps2wrap.h"
#include "AddRegisters.h"
#include "ctexture.h"
#include "pad.h"
#include "vecmaths.h"
#include "font.h"
#include "Ctimer.h"
#include "cpipe3d.h"
#include "ms3dmodel.h"
#include "caxes.h"

// Function Prototypes
void GetUserInput(void);
void GameLogic(CMs3dModel * pModel);

// The SPS2 Wrapper Class
CSPS2Manager Manager;

// The Pipeline Class
CPipe3d Pipe3d;

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
	
	// The model Class
	CMs3dModel  Model;
	
	// The Axes Class
	CAxes LocalAxes, WorldAxes;
	
	// Load the Model
	Model.Load("spawn.txt");
		
	// Set up the view port transformation Matrix to match the screen
	Pipe3d.SetupVT((float)Manager.GetScreenWidth(), (float)Manager.GetScreenHeight());
	
	//initialise the pads
	if(pad_init (PAD_0, PAD_INIT_LOCK |PAD_INIT_ANALOGUE | PAD_INIT_PRESSURE) == 0)
		{
		printf("Unable to initialise the pads\n");
		pad_cleanup (PAD_0);
		exit(0);
		}
	
	// Performance timer - call after manager initialisation.
	CTimer Timer;				
	
	// The font class Pass in the sps2manager
	CFont Font(&Manager);
	
	// Initialise font class and load the texture into SPS2 memory.
	Font.Initialise("font.bmp", "font.dat", Manager.GetFirstFreeGSPage());
	
	// Initialise the texture class for texture 1
	CTexture Tex1(Manager.GetTex1Base(), Manager.GetTex1DmaBase(), &Manager);
	
	// Load game bitmap into SPS2 memory
	if(!Tex1.LoadBitmap("skin.bmp", false, false))
	{
		printf("Failed to load texture.");
		return -1;
	}

	// Make sure all data is written to SPS2 memory from the cache
	Manager.FlushCache();

	//install a signal handler to capture all signals
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
		GameLogic(&Model);
			
		//upload the font texture
		Font.Begin();
		
		//render the font info
		Font.Render(-150, -240, \
					"Textured MilkShape 3D Model", \
					0x30, 0x10, 0x10, 0x80);
		Font.printf(180, -240, 0,  0x50,  0x50, 0x50, 
					"Frame: %d\nFPS: %.1f" , iFrame++, fps);
		
		//upload the game texture
		Tex1.Upload();
		
		// Build the GS Packet
		void * pMemEnd = Manager.GetPrim1Base();
		Model.BuildGSPacket(pMemEnd, &Pipe3d);
		WorldAxes.BuildGSPacket(pMemEnd, &Pipe3d);
		LocalAxes.SetWorldMatrix(Model.GetWorldMatix());
		LocalAxes.BuildGSPacket(pMemEnd, &Pipe3d);
		
		// Render the GS Packet
		Manager.FirePacketToGS(Manager.GetPrim1Base(), pMemEnd, Manager.GetPrim1DmaBase());
		 		
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

// Update the controller pad structures
void GetUserInput(void)
{
	pad_update (PAD_0);
	
}


void GameLogic(CMs3dModel * pSP)
{
	enum Mode_e {T_MOD_WORLD, T_MOD_LOCAL, R_MOD, T_CAMERA_LOCAL, R_CAMERA};
	static enum Mode_e Mode;
	
	if(pad[0].buttons & PAD_CROSS) Mode = T_MOD_WORLD;
	if(pad[0].buttons & PAD_TRI) Mode = T_MOD_LOCAL;
	if(pad[0].buttons & PAD_CIRCLE) Mode = R_MOD;
	if(pad[0].buttons & PAD_SQUARE) Mode = T_CAMERA_LOCAL;
	if(pad[0].buttons & PAD_R1) Mode = R_CAMERA;
		
	// Reset to default position
	if((pad[0].buttons & PAD_R3)||(pad[0].buttons & PAD_L3))
		{
			pSP[0].Reset();
			FVector Position, LookAt, Up;
			SetVector(&Position, 0.0f, 0.0f, 150.0f, 1.0f);
			SetVector(&LookAt, 0.0f, 0.0f, 0.0f, 1.0f);
			SetVector(&Up, 0.0f, 1.0f, 0.0f, 1.0f);
			Pipe3d.CalcCameraMatrix(Position, LookAt, Up);
		}
	
	switch(Mode)
	{
		case T_MOD_WORLD:
			//move along World x 
			if(pad[0].buttons & PAD_RIGHT)	pSP[0].MoveWX(pad[0].pressures[PAD_PRIGHT] * 3.0f);
			if(pad[0].buttons & PAD_LEFT)	pSP[0].MoveWX(-pad[0].pressures[PAD_PLEFT] * 3.0f);
			
			//move along World y 
			if(pad[0].buttons & PAD_UP) pSP[0].MoveWY(pad[0].pressures[PAD_PUP] * 3.0f);
			if(pad[0].buttons & PAD_DOWN) pSP[0].MoveWY(-pad[0].pressures[PAD_PDOWN] * 3.0f);
				
			//move along World z 
			if(pad[0].buttons & PAD_L1) pSP[0].MoveWZ(pad[0].pressures[PAD_PL1] * 3.0f);
			if(pad[0].buttons & PAD_L2) pSP[0].MoveWZ(-pad[0].pressures[PAD_PL2] * 3.0f);
			break;
			
		case T_MOD_LOCAL:
			//move along local x 
			if(pad[0].buttons & PAD_RIGHT)	pSP[0].MoveLX(pad[0].pressures[PAD_PRIGHT] * 3.0f);
			if(pad[0].buttons & PAD_LEFT)	pSP[0].MoveLX(-pad[0].pressures[PAD_PLEFT] * 3.0f);
			
			//move along local y 
			if(pad[0].buttons & PAD_UP) pSP[0].MoveLY(pad[0].pressures[PAD_PUP] * 3.0f);
			if(pad[0].buttons & PAD_DOWN) pSP[0].MoveLY(-pad[0].pressures[PAD_PDOWN] * 3.0f);
				
			//move along local z 
			if(pad[0].buttons & PAD_L1) pSP[0].MoveLZ(pad[0].pressures[PAD_PL1] * 3.0f);
			if(pad[0].buttons & PAD_L2) pSP[0].MoveLZ(-pad[0].pressures[PAD_PL2] * 3.0f);
			break;
			
		case R_MOD:
			//rotate the sprite round x axis
			if(pad[0].buttons & PAD_RIGHT) pSP[0].m_RotX += pad[0].pressures[PAD_PRIGHT] * 0.2f;
			if(pad[0].buttons & PAD_LEFT) pSP[0].m_RotX -= pad[0].pressures[PAD_PLEFT] * 0.2f;
			
			//rotate the sprite round y axis
			if(pad[0].buttons & PAD_UP) pSP[0].m_RotY += pad[0].pressures[PAD_PUP] * 0.2f;
			if(pad[0].buttons & PAD_DOWN) pSP[0].m_RotY -= pad[0].pressures[PAD_PDOWN] * 0.2f;
			
			//rotate the sprite round z axis
			if(pad[0].buttons & PAD_L1) pSP[0].m_RotZ += pad[0].pressures[PAD_PL1] * 0.2f;
			if(pad[0].buttons & PAD_L2) pSP[0].m_RotZ -= pad[0].pressures[PAD_PL2] * 0.2f;
			break;
			
		case T_CAMERA_LOCAL:
			// Translate the Camera Position
			//move along local x 
			if(pad[0].buttons & PAD_RIGHT)	Pipe3d.MoveLX(pad[0].pressures[PAD_PRIGHT] * 3.0f);
			if(pad[0].buttons & PAD_LEFT)	Pipe3d.MoveLX(-pad[0].pressures[PAD_PLEFT] * 3.0f);
			
			//move along local y 
			if(pad[0].buttons & PAD_UP) Pipe3d.MoveLY(pad[0].pressures[PAD_PUP] * 3.0f);
			if(pad[0].buttons & PAD_DOWN) Pipe3d.MoveLY(-pad[0].pressures[PAD_PDOWN] * 3.0f);
				
			//move along local z 
			if(pad[0].buttons & PAD_L1) Pipe3d.MoveLZ(pad[0].pressures[PAD_PL1] * 3.0f);
			if(pad[0].buttons & PAD_L2) Pipe3d.MoveLZ(-pad[0].pressures[PAD_PL2] * 3.0f);
			break;
			
		case R_CAMERA:
			// Simple rotation of the Camera
			//Rotate Round z 
			if(pad[0].buttons & PAD_L1)	Pipe3d.RotZ(pad[0].pressures[PAD_PL1] * 0.01f);
			if(pad[0].buttons & PAD_L2)	Pipe3d.RotZ(-pad[0].pressures[PAD_PL2] * 0.01f);
			
			//Rotate Round x 
			if(pad[0].buttons & PAD_UP) Pipe3d.RotX(pad[0].pressures[PAD_PUP] * 0.01f);
			if(pad[0].buttons & PAD_DOWN) Pipe3d.RotX(-pad[0].pressures[PAD_PDOWN] * 0.01f);
				
			//Rotate Round y
			if(pad[0].buttons & PAD_RIGHT) Pipe3d.RotY(pad[0].pressures[PAD_PRIGHT] * 0.01f);
			if(pad[0].buttons & PAD_LEFT) Pipe3d.RotY(-pad[0].pressures[PAD_PLEFT] * 0.01f);
			break;
	}
}

