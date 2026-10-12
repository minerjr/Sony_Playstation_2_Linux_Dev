#include <stdio.h>
#include <assert.h>
#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>
#include "sps2wrap.h"

// Function prototypes
int BuildGSPacket(void * pMem);

// Define triangle vertices relative to centre of screen
int VertexData[][3] =
{
	{-200, -220, 100}, {-200,  -20, 100}, { -50, -120, 100}, // Triangle 1
	{ 200, -220, 100}, { 200,  -20, 100}, {  50, -120, 100}, // Triangle 2
	{-200,  220, 100}, {-200,   20, 100}, { -50,  120, 100}, // Triangle 3
	{ 200,  220, 100}, { 200,   20, 100}, {  50,  120, 100}, // Triangle 4
};

// Define vertex  colours
int VertexColors[][3] =
{
	{255,   0,   0}, {  0, 255,   0}, {  0,   0, 255},  // Triangle 1
	{255,   0,   0}, {  0, 255,   0}, {  0,   0, 255},  // Triangle 2
	{255,   0,   0}, {  0, 255,   0}, {  0,   0, 255},  // Triangle 3
	{255,   0,   0}, {  0, 255,   0}, {  0,   0, 255},  // Triangle 4
}; 

// The number of vertices and colours
int iVertexCount=sizeof(VertexData)/(3*sizeof(int));
int iColourCount=sizeof(VertexColors)/(3*sizeof(int));

CSPS2Manager Manager;

// The main function
int main(void)
{
	Manager.Initialise(640);
  	
  	int iQWC;		// Qword count
  		  	
  	Dn_CHCR_t chcr;	// Channel control register used to control the DMA transfer
	
	// assert that these are the same
	assert(iVertexCount == iColourCount);
	
  	// Set the channel control register values.
  	// These will not change.
  	chcr.i32 = 0;                  // Clear everything
  	chcr.s.MOD = CHCR_MOD_NORMAL;  // Normal mode transfer
  	chcr.s.STR = 1;                // STR=1 to start transfer

  	// Render loop
  	while (true) 
  	{
    	// Clear the screen to blue.
    	Manager.BeginScene(0x00, 0x00, 0x40);

    	// Build the GSPacket & return qwords written
    	iQWC=BuildGSPacket(Manager.GetPrim1Base());
    	
    	Manager.FlushCache();

    	// Set the QWC register
    	*EE_D2_QWC=iQWC;

    	// Set the memory address register with the start of allocated memroy
    	*EE_D2_MADR=Manager.GetPhysicalAddress(Manager.GetPrim1Base());

    	// Start the DMA transfer
    	*EE_D2_CHCR=chcr.i32;

    	// Wait for the DMA transfer to finish
    	Manager.WaitForDMA(2);

    	// Swap display buffers
    	Manager.EndScene();
  	}
  
  	return 0;
}

// Build the GS packet. Function takes a pointer to the allocated memory
// and returns the qwords written to this memory
int BuildGSPacket(void * pMem) 
{
	int iQWC;					//qword count
	// GIFTag pointer
  	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *) pMem;
  	// GIF Register pointer
  	sps2GIFPackedRegister_t * pReg; 
  	
  	// Prepare the GIF tag  
  	pGIFTag->i128 = 0;     			// Clear it
  	pGIFTag->s.NLOOP = 0;  			// Set to 0 and increment as required
  	pGIFTag->s.PRE =1 ;      		// provide a valid PRIM value
  	pGIFTag->s.EOP = 1;      		// End of packet
  	pGIFTag->s.PRIM = 11;    		// 3 = triangle | 8 = gourard shading
  	pGIFTag->s.FLG = GIF_FLG_PACKED;// Packed format 
  	pGIFTag->s.NREG = 0;     		// Start at zero and inc as required
  
  	// Add RGBAQ register.
  	SPS2_SET_GIF_REG(*pGIFTag, pGIFTag->s.NREG++, GIF_REG_RGBAQ);
  	// Add XYZ2 register.
  	SPS2_SET_GIF_REG(*pGIFTag, pGIFTag->s.NREG++, GIF_REG_XYZ2);
	
	// Set pReg to qword after the GIFTag
  	pReg = (sps2GIFPackedRegister_t *) (pGIFTag + 1 );
  
  	// Loop through all vertices
  	for (int i=0;i<iVertexCount;i++)
	{
    	pGIFTag->s.NLOOP++;  					// Increment NLOOP in GIFTag
    
    	pReg->i128=0;   						// Blank out this value
    	// Set RGBA values
    	pReg->RGBAQ.s.R = VertexColors[i][0];
    	pReg->RGBAQ.s.G = VertexColors[i][1];
    	pReg->RGBAQ.s.B = VertexColors[i][2];
    	pReg->RGBAQ.s.A = 0x80; 				// Alpha = 128
    	
    	pReg++;		// Increment register data pointer
    
    	pReg->i128=0;  							// Blank out this value
    	// Set XYZ values
    	pReg->XYZ2.s.X = (2048 + VertexData[i][0])<< 4;
    	pReg->XYZ2.s.Y = (2048 + VertexData[i][1])<< 4;
    	pReg->XYZ2.s.Z = VertexData[i][2];
    
    	pReg++;		// Increment register data pointer
    }

  	// Determine how many bytes have been written.
  	// Shift right by 4 to give qwords written
  	iQWC = ((int) pReg - (int) pMem) >> 4;
  	
  	// assert that qwords written is less than allocated memory
  	assert(iQWC < (4096 >> 4));
  	
  	//return qword count
  	return iQWC;
}