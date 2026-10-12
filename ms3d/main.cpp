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
	
	// Load the Model
	Model.Load("cube_m6.txt");
		
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
	if(!Tex1.LoadBitmap("max.bmp", true, false))
	{
		printf("Failed to load texture.");
		return -1;
	}

	// Make sure all data is written to SPS2 memory from the cache
	Manager.FlushCache();

	// Set the model position
	Model.SetPosition(0.0f, 0.0f, -100.0f);	

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

// Perform some game logic
void GameLogic(CMs3dModel * pModel)
{
	pModel->m_RotX += 0.025;
	pModel->m_RotY += 0.012;
	pModel->m_RotZ += 0.023;
}

