#include "sps2.h"
#include <signal.h>
#include "PS2Defines.h"
#include "AddRegisters.h"
#include "sps2wrap.h"
#include "ctexture.h"

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

	//install a signal handler to capture all signals
	for(int iSignal = 0; iSignal<128; iSignal++)
	{
		signal(iSignal, signal_handler);
	}

	// Open the VU's 
	// This prevents other applications from using them at the same time
	open("/dev/ps2vpu0", O_RDWR);
	open("/dev/ps2vpu1", O_RDWR);

	// Upload the game texture to GS Mem
	Tex1.Upload();

	// We are building a DMA chain to be sent to VIF1. The VIF is controlled
	// by what are called VIF codes. Vif codes are 32 bit numbers, each
	// having their own function. The only codes we will be using here are
	// NOP (No Operation) and DIRECT (Direct to GIF transfer i.e. Path 2)
		
	// We are transferring this DMA Chain with TTE (Transfer tag enabled).
	// This means that the 128 bits that holds the DMA tag is transferred
	// as well as the QWC of data following it. Since a DMA tag is only 64-bits
	// this means we can put 2 32-bit VIF codes straight after the DMA tag 
	// and they will be sent to the VIF.
	
	// Use Source chain mode and a single End tag to transfer the data
	sps2DMATag_t DMAEnd;
	DMAEnd.i64 = 0;
	DMAEnd.s.QWC = 6;
	DMAEnd.s.ID = DMA_ID_SOURCE_END;

	uint64 * pdwMem = (uint64 *)Manager.GetPrim1Base();

	// Add some VU codes. Each VU code is 32 bits, so we can fit 2 into
	// 64 bits by shifting one right 32 bits.
	*pdwMem++ = DMAEnd.i64;
	*pdwMem++ = (DIRECT(6) << 32) | NOP;	// A "No Operation"
		// and a DIRECT instruction. DIRECT(6) tells VIF1 to
		// transfer the following 6 quadwords of data straight
		// to the GIF. The DIRECT instruction must be in the last
		// 32 bits of a quad word so that the data that follows
		// it is quad word aligned.

	// Create a textured sprite
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pdwMem;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 1;	// Only one of them
	pGIFTag->s.PRE = 1;	// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;	// This is the last giftag in the packet
	pGIFTag->s.PRIM = GS_PRIM(PRIM_SPRITE, 
							PRIM_IIP_GOURAUD, 
							PRIM_TME_ON,
							PRIM_FGE_OFF, 
							PRIM_ABE_ON, 
							PRIM_AA1_OFF, 
							PRIM_FST_UV, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 5; 
	pGIFTag->s.REGS = GIF_REG_RGBAQ | 
						(GIF_REG_UV << 4) | 
						(GIF_REG_XYZ2 << 8) | 
		                (GIF_REG_UV << 12) | 
		                (GIF_REG_XYZ2 << 16);

	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	AddColour(pRegister, 0x80, 0x80, 0x80, 0x80);
	AddUV(pRegister, 0, 0);
	AddVertex(pRegister, -128, -128, 100, 0);
	AddUV(pRegister, 256, 256);
	AddVertex(pRegister,  128,  128, 100, 0);

	// Important: Make sure all the data is written to the physical memory
	Manager.FlushCache();

	// The DMA registers we will be using.
	Dn_CHCR_t chcr;
	chcr.i32 = 0;
	chcr.s.MOD = CHCR_MOD_CHAIN;	// Source Chain mode transfer
	chcr.s.STR = 1;					// Start the transfer
	chcr.s.TTE = 1;					// Transfer the DMA tag aswell
	chcr.s.DIR = 1;					// We are going FROM main memory
	
	Dn_TADR_t tadr;
	tadr.i32 = 0;
	tadr.s.ADDR = Manager.GetPhysicalAddress(Manager.GetPrim1Base());

	// We want to send the packet to the GS every frame
	while(true)
	{
		//clear the current draw buffer
		Manager.BeginScene(0x50, 0x00, 0x20);

		// DMA Channel 1 is the one we want, it goes to VU1
		// Set QWC to 0 to force the tag at tag address to be read
		*EE_D1_QWC = 0;
		
		*EE_D1_TADR = tadr.i32;
		
		// Set this register to start the transfer
		*EE_D1_CHCR = chcr.i32;	

		// Wait for it to complete
		Manager.WaitForDMA(1);

		// Swap the screen buffers
		Manager.EndScene();
		
		// Get out the loop on any signal
		if(!Manager.GetRunningStatus())break;
	}

	printf("Exiting.........\n");

	return 0;
}

