#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>
#include <assert.h>
#include "PS2Defines.h"
#include "AddRegisters.h"
#include "ctexture.h"


// Function Prototypes
int BuildGSPacket(void * pMem);

// These are made global so we can access them from anywhere
sps2Memory_t * pMemCached;
int iSPS2Desc;

// Work out how we are going to allocate SPS2 to each use:
// Here we have 4K allocated for primitives, that is right at the start of the memory
// Next, 256K allocated for the texture.
// Finally 4K allocated for the texture upload function to use for DMAing
// (The KILOBYTES macro just multiplies the number by 1024)
#define PRIMITIVEMEM_4K		0
#define TEXTUREMEM_256K		(KILOBYTES(4))
#define TEXTUREDMAMEM_4K	(KILOBYTES(260))
#define TOTALMEMNEEDED		(KILOBYTES(264))

int main(void)
{
	//initialise sps2
	iSPS2Desc = sps2Init();

	// Initialise the screen because we are drawing graphics
	sps2UScreenInit(0);

	// The DMA registers we will be using. We are doing a normal mode transfer
	Dn_CHCR_t chcr;
	Dn_MADR_t madr;

	// Allocate the SPS2 memory
	pMemCached = sps2Allocate(TOTALMEMNEEDED, 
							SPS2_MAP_BLOCK_4K |	SPS2_MAP_CACHED, 
							iSPS2Desc);

	// Get a char pointer, we can use this to access memory addresses
	// relative to the start of SPS2 memory
	char * pcMemory = (char *)pMemCached->pvStart;

	// The texture class for the game bitmap
	CTexture Tex(&pcMemory[TEXTUREMEM_256K]);
	
	// Load game bitmap into SPS2 memory
	if(!Tex.LoadBitmap("max8bit256.bmp", true, false))
	{
		printf("Failed to load texture.");
		return -1;
	}
	
	//upload the game texture
	Tex.Upload(sps2UScreenGetFirstFreeGSPage(), &pcMemory[TEXTUREDMAMEM_4K]);

	// Build the GS Packet and return the number of qwords written
	int iQWC = BuildGSPacket((void *)&pcMemory[PRIMITIVEMEM_4K]);

	// Configure the required register values
	// Set the address field to the physical address of the GS Packet
	madr.i32 = 0;		
	madr.s.ADDR = sps2GetPhysicalAddress((void *)&pcMemory[PRIMITIVEMEM_4K], pMemCached);
	
	// The channel control register
	chcr.i32 = 0;
	chcr.s.MOD = CHCR_MOD_NORMAL;	// Normal mode transfer
	chcr.s.STR = 1;					// Start the transfer
	
	// We want to send the packet to the GS every frame
	while(true)
	{
		//clear the screen to the specified colour
		sps2UScreenClear(0x40, 0x00, 0x00);
		
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

	// Release SPS2
	sps2Release(iSPS2Desc);

	return 0;
}

int BuildGSPacket(void * pMem)
{
	// Declare a GIFTag pointer
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pMem;
	
	// Create a textured sprite
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 1;	// Only one of them
	pGIFTag->s.PRE = 1;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;		// This is the last giftag in the packet
	// Set up the PRIM register with texture mapping on and UV coordinates
	pGIFTag->s.PRIM = GS_PRIM(PRIM_SPRITE, 
							PRIM_IIP_FLAT, 
							PRIM_TME_ON,
							PRIM_FGE_OFF, 
							PRIM_ABE_ON, 
							PRIM_AA1_OFF, 
							PRIM_FST_UV, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	// This is 5 now, as we are sending one colout two positions 
	// and two UV coordinates
	pGIFTag->s.NREG = 5; 
	pGIFTag->s.REGS = GIF_REG_RGBAQ 		| \
						(GIF_REG_UV << 4) 	| \
						(GIF_REG_XYZ2 << 8) | \
						(GIF_REG_UV << 12) 	| \
						(GIF_REG_XYZ2 << 16);

	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	// Add the register data to the GS Packet
	// RGBA
	AddColour(pRegister, 0x80, 0x80, 0x80, 0x80);
	// UV
	AddUV(pRegister, 0, 0);
	// XYZ
	AddVertex(pRegister, -128, -128, 100);
	// UV
	AddUV(pRegister, 256, 256);
	// XYZ
	AddVertex(pRegister,  128,  128, 100);

	// Work out how many quad words we just wrote
	int iQWC = ((int)pRegister - (int)pMem) >> 4;

	// Assert that we haven't written more data than we allocated
	assert(iQWC < (4096 >> 4));

	// Important: Make sure all the data is written to the physical memory
	sps2FlushCache(iSPS2Desc);	

	//return the number of qwords written
	return iQWC;	
}