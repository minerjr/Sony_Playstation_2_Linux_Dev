#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <assert.h>
#include "PS2Defines.h"
#include "PS2Types.h"
#include "AddRegisters.h"
#include "texture.h"
#include "packet.h"

// Work out how we are going to allocate SPS2 to each use:
// Here we have 4K allocated for primitives, that is right at the start of the memory
// Next, 256K allocated for the texture.
// Finally 4K allocated for the texture upload function to use for DMAing
// (The KILOBYTES macro just multiplies the number by 1024)
#define PRIMITIVEMEM_4K		0
#define TEXTUREMEM_256K		(KILOBYTES(4))
#define TEXTUREDMAMEM_4K	(KILOBYTES(260))
#define TOTALMEMNEEDED		(KILOBYTES(264))

//function prototypes in this file
void AnimateSprite(PS2Sprite_t * pSP);
void MoveSprite(PS2Sprite_t * pSP);

//globals
sps2Memory_t * pMemCached;	//pointer to allocated memory structure
int iSPS2Desc;				//the SPS2 handle

int main(void)
{
	int Frame;					// Frame counter
	char * pcMemory;			// pointer to start of allocated memory
	char * pPrimitiveMem;		// pointer to the start of primitive memory
	Dn_CHCR_t chcr;				// The DMA registers we will be using. 
	Dn_MADR_t madr;
	PS2Sprite_t Sprite[2];		//some sprite structures to play with
	
	//initialise the SPS2 module
	iSPS2Desc = sps2Init();
	
	// Initialise the screen
	sps2UScreenInit(0);

	// Allocate the SPS2 memory
	pMemCached = sps2Allocate(TOTALMEMNEEDED, 
							SPS2_MAP_BLOCK_4K |	SPS2_MAP_CACHED, 
							iSPS2Desc);

	// Get a char pointer, we can use this to access memory addresses
	// relative to the start of SPS2 memory
	pcMemory = (char *)pMemCached->pvStart;
	
	//pointer to the start of primitive memory
	pPrimitiveMem = &pcMemory[PRIMITIVEMEM_4K];

	// Load the 256*256 bitmap into SPS2 memory
	// The final parameter says whether or not to make RGB(0,0,0)
	// transparent
	if(!LoadBitmap("game.bmp", &pcMemory[TEXTUREMEM_256K], true))
	{
		printf("Failed to load texture.");
		return -1;
	}

	// Upload the texture from SPS2 memory into the first free
	// page of GS Memory. 
	// The second parameter is the address of the texture in SPS2 memory
	// The final parameter is 4K of memory that the Upload function can use
	// for creating its own DMA chain.
	Upload(sps2UScreenGetFirstFreeGSPage(), 
			&pcMemory[TEXTUREMEM_256K], 
			&pcMemory[TEXTUREDMAMEM_4K]);

	
	// Configure the first Sprite Structures
	// Could write a consructor function for this!
	Sprite[0].x = -100;
	Sprite[0].y = -100;
	Sprite[0].z = 1000;
	Sprite[0].w = 32;
	Sprite[0].h = 32;
	Sprite[0].u = 32;
	Sprite[0].v = 0;
	Sprite[0].r = 0x80;
	Sprite[0].g = 0x80;
	Sprite[0].b = 0x80;
	Sprite[0].a = 0x80;
	Sprite[0].dx = 0;
	Sprite[0].FCountMax = 5;	
	Sprite[0].FCount = 0;
	Sprite[0].NFrames = 8;		

	// Configure the second Sprite Structures
	// Could write a consructor function for this!
	Sprite[1].x = -100;
	Sprite[1].y = -80;
	Sprite[1].z = 100;
	Sprite[1].w = 32;
	Sprite[1].h = 32;
	Sprite[1].u = 0;
	Sprite[1].v = 0;
	Sprite[1].r = 0x80;
	Sprite[1].g = 0x80;
	Sprite[1].b = 0x80;
	Sprite[1].a = 0x80;
	Sprite[1].dx = 0;
	
	//get the required register values
	// Clear the register to zero	
	madr.i32 = 0;		
	// Set the address field to the physical address of the primitive memory
	madr.s.ADDR = sps2GetPhysicalAddress(pPrimitiveMem, pMemCached);
	
	// The channel control register
	chcr.i32 = 0;
	chcr.s.MOD = CHCR_MOD_NORMAL;	// Normal mode transfer
	chcr.s.STR = 1;					// Start the transfer

	// We want to send the packet to the GS every frame
	while(Frame++)
	{	
		// The following line of code is a mess and is not critical
		// to the functionality of the program.
		// It changes the animation sequece being used every 10 seconds or so.
		// Comment it out and/or delete it if you want.
		if(!(Frame%500)){Sprite[0].u += 32; if(Sprite[0].u > 96)Sprite[0].u = 32;}
		
		// Set/reset memory pointer to the start of primitive memory
		pPrimitiveMem = &pcMemory[PRIMITIVEMEM_4K]; 
		
		//clear the screen to the specified colour
		sps2UScreenClear(0x30, 0x00, 0x10);
		
		//do some sprite animation animation
		AnimateSprite(&Sprite[0]);
		
		//do some movement
		MoveSprite(&Sprite[0]);
		
		//build the GS packet for all objects to be drawn
		BuildSprite2D(&Sprite[1], pPrimitiveMem);
		BuildSprite2D(&Sprite[0], pPrimitiveMem);
		
		//calculate how many qwords have been written
		int iQWC = ((int)pPrimitiveMem - (int)&pcMemory[PRIMITIVEMEM_4K]) >> 4;
	
		// Assert that we haven't written more data than we allocated
		assert(iQWC < (4096 >> 4));	
	
		// Important: Make sure all the data is written to the physical memory
		sps2FlushCache(iSPS2Desc);
				
		// DMA Channel 2 is the one we want, it goes to the GS
		// Amount of data to be transmitted
		*EE_D2_QWC = iQWC;	

		// Set the memory address register
		*EE_D2_MADR = madr.i32;
		
		// Set the register to start the transfer
		*EE_D2_CHCR = chcr.i32;	

		// Wait for it to complete
		sps2WaitForDMA(2, iSPS2Desc);

		// Display the buffer we were just drawing to
		sps2UScreenSwap();
	}
	
	// Shutdown the screen
	sps2UScreenShutdown();

	// Free the memory
  	sps2Free(pMemCached);

	// Release SPS2
	sps2Release(iSPS2Desc);

	return 0;
}


// some very simple animation
// Change the frame every time the counter reaches FCount Max
// Frames go in the sequence 0123456701234567 etc. etc.
void AnimateSprite(PS2Sprite_t * pSP)
{
	if((pSP->FCount++)%pSP->FCountMax == 0)
		{
			pSP->v += pSP->h;
			if(pSP->v > (pSP->NFrames - 1) * pSP->h)pSP->v = 0;
		}
}

// some very simple movement
// Move sptite back and forward across screen
// dx=0 move left to right
// dx = 1 move right to left
void MoveSprite(PS2Sprite_t * pSP)
{
	if(pSP->dx == 0)
		{
			if(pSP->x < (320 - pSP->w))pSP->x++;
			else pSP->dx = 1;
		}
	else
		{
			if(pSP->x > - 320)pSP->x--;
			else pSP->dx = 0;
		}	
}