#include "sps2.h"
#include <signal.h>
#include "PS2Defines.h"
#include "AddRegisters.h"
#include "sps2wrap.h"
#include "ctexture.h"

// Function Prototypes
int BuildVIFPacket(void *& pMem);

CSPS2Manager Manager;

// These two external function pointers are exported by the VU code
// and we can now use them as pointers to the compiled VU data
extern "C" void VU_vu1code_start(void);
extern "C" void VU_vu1code_end(void);

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
	
	// Open the VU's (This could go in Manager.Initialise())
	if(open("/dev/ps2vpu0", O_RDWR) < 0)
	{
		printf("Can't open VU0\n");
		exit(-1);
	}
	if(open("/dev/ps2vpu1", O_RDWR) < 0)
	{
		printf("Can't open VU1\n");
		exit(-1);
	}
	
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
	
	// Upload the game texture to GS Mem
	Tex1.Upload();

	//install a signal handler to capture all signals
	for(int iSignal = 0; iSignal<128; iSignal++)
	{
		signal(iSignal, signal_handler);
	}

	// Build the VIF Packet
	void * pMemEnd = Manager.GetPrim1Base();
	printf("Packet size = %d qwords\n", BuildVIFPacket(pMemEnd) >> 4);

	// Find out the length of the code in double words
	// We use double words since code is made up of many
	// pairs of two (1 upper, and 1 lower) VU instructions. 
	// Each instruction is 32 bits, so VU code must be a
	// multiple of 64 bits.
	uint32 iCodeLengthInDWORDs = (((uint32)&VU_vu1code_end - (uint32)&VU_vu1code_start) + 7) >> 3;
	// Copy the code to VU1_MICRO_MEM
	memcpy(VU1_MICRO_MEM, &VU_vu1code_start, iCodeLengthInDWORDs * 8);
	
	// We want to send the packet to the VIF every frame
	while(true)
	{
		//clear the current draw buffer
		Manager.BeginScene(0x20, 0x30, 0x20);
		
		Manager.FirePacketToVIF(Manager.GetPrim1Base(), pMemEnd, Manager.GetPrim1DmaBase());

		// Swap the screen buffers
		Manager.EndScene();
		
		// Get out the loop on any signal
		if(!Manager.GetRunningStatus())break;
	}
	
	printf("Exiting.........\n");

	return 0;
}

int BuildVIFPacket(void *& pMem)
{
	uint64 * pdwMem = (uint64 *)pMem;

	// Add some VU codes. Each VU code is 32 bits, so we can fit 2 into
	// 64 bits by shifting one right 32 bits.
	// Put the UPACK VIF code in the last work of the qword
	*pdwMem++ = (NOP << 32) | NOP;
	*pdwMem++ = (UNPACK(V4_32, 13, 0) << 32) | NOP;	
		// UNPACK(V4_32, 13, 0) says to send each quad word
		// of data to VU mem as it is (an array of 4 * 32 bit elements)
		// 6 is the number of quadwords to send, 0 is how far into VU mem
		// we want to put the data. 	
	
	// Create a textured tri-strip
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pdwMem;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 4;	// 4 vertices
	pGIFTag->s.PRE = 1;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;		// This is the last giftag in the packet
	pGIFTag->s.PRIM = GS_PRIM(GS_TRISTRIP, 
							PRIM_IIP_GOURAUD, 
							PRIM_TME_ON,
							PRIM_FGE_OFF, 
							PRIM_ABE_ON, 
							PRIM_AA1_OFF, 
							PRIM_FST_UV, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 3; 
	pGIFTag->s.REGS = GIF_REG_RGBAQ | 
						(GIF_REG_UV << 4) | 
						(GIF_REG_XYZ2 << 8); 

	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	AddColour(pRegister, 0x80, 0x80, 0x80, 0x80);
	AddUV(pRegister, 0, 0);
	AddFloatVert(pRegister, -128.0f, -128.0f, 100, 0);
	AddColour(pRegister, 0x80, 0x80, 0x80, 0x80);
	AddUV(pRegister, 0, 256);
	AddFloatVert(pRegister, -128.0f, 128.0f, 100, 0);
	AddColour(pRegister, 0x80, 0x80, 0x80, 0x80);
	AddUV(pRegister, 256, 0);
	AddFloatVert(pRegister, 128.0f, -128.0f, 100, 0);
	AddColour(pRegister, 0x80, 0x80, 0x80, 0x80);
	AddUV(pRegister, 256, 256);
	AddFloatVert(pRegister, 128.0f, 128.0f, 100, 0);
	
	// Now for some more VIF codes
	// First the FLUSH VIF code makes sure that the VU1 has finished what
	// it was doing and that path1 and path2 are idle.
	// Next MSCALL(0) starts the microprogram at address 0 in
	// VU1 micro memory
	pdwMem = (uint64 *)pRegister;
	*pdwMem++ = (MSCALL(0) << 32) | FLUSH;
	*pdwMem++ = (NOP << 32) | NOP;
		
	// Important: Make sure all the data is written to the physical memory
	Manager.FlushCache();
		
	// Work out how much we just wrote
	int iWC = (int)pdwMem - (int)pMem;
	
	//Set the memory pointer we used to the next free location
	pMem = (char *)pdwMem;
	
	//return the number of bytes written to memory
	return iWC;	
}