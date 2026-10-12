#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <assert.h>
#include "defines.h"
#include "types.h"
#include "stitch.h"
#include "packet.h"



/**
 * This function prepare the data to be displayed.
 * pvBase points to the start of the  data buffer.
 * Returns the number of qwords put into pvBase.
*/
int LoadPacketData(void * pvBase)
{
	int iWC;							// word count written
	int iCount;							// a loop counter
	int TriSize = 40;					// Size of the Triangles!
	int NumTriangles = 200;				// number of triangles to draw
	Position_t Position;				// coordinate of the triangle
	int ScreenW, ScreenH;				//width and height of screen
	sps2GIFTag_t * pGIFTag;				//GifTag pointer (128bits)	
	sps2GIFPackedRegister_t * pRegister;//packed register pointer (128bits)
	
	//get the screen width and height
 	ScreenW = sps2UScreenGetWidth();
	ScreenH = sps2UScreenGetHeight();
	printf("Screen Width(%d) Screen Height(%d)\n", ScreenW, ScreenH);	
	
	// initialise the stitching process and
	// write the first dma tag
	InitStitch(pvBase);
		
	//the giftag is the next qword after the dma tag
	pGIFTag = (sps2GIFTag_t *)pvBase + 1;
	
	//call before adding any data to the dma chain
	Stitch(pGIFTag);

  	// Prepare the GIF tag  
  	pGIFTag->i128=0;       			// Blank it out
  	pGIFTag->s.NLOOP=0;    			// 0 entries.  We'll increment as we add entries
  	pGIFTag->s.PRE=1;      			// We are providing a valid PRIM value
	pGIFTag->s.EOP=1;      			// End of packet -- draw me please!
 	pGIFTag->s.PRIM=11;   		 	// 3 = triangle | 8 = gourard shading
 	pGIFTag->s.FLG=GIF_FLG_PACKED;	// We're using the packed format for data
 	pGIFTag->s.NREG=0;    			// Start off with zero registers, increment as we
                         			// populate the descriptions

  
	// Add the RGBAQ register to the giftag.
	SPS2_SET_GIF_REG(*pGIFTag, pGIFTag->s.NREG++, GIF_REG_RGBAQ);
	// Add the XYZ2 register to the giftag. 
	SPS2_SET_GIF_REG(*pGIFTag, pGIFTag->s.NREG++, GIF_REG_XYZ2);
 
 	//get a pointer to the packed register data which the next qword
 	pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1); 
  
	// Loop through all the triangles to draw
	// set the RGBAQ and XYZ2 register data
  	for (iCount=0;iCount<NumTriangles;iCount++)
	{
		//get a random position on screen
		Position.x = rand()%ScreenW - (ScreenW/2);
		Position.y = rand()%ScreenH - (ScreenH/2);
		
		// Increment NLOOP for each vertex used
		pGIFTag->s.NLOOP++;  
    
    	//call before adding any data to the dma chain
    	pRegister += Stitch(pRegister);
    
    	//Set the  first vertex colour
    	pRegister->i128=0;   // Blank out this value
    	pRegister->RGBAQ.s.A=0x80; // Alpha = 128
    	// make the vertex colour red
    	pRegister->RGBAQ.s.R = 0x80;
    	pRegister->RGBAQ.s.G = 0;
    	pRegister->RGBAQ.s.B = 0;
    
    	// Increment register data pointer
    	pRegister++;
    	
		//call before adding any data to the dma chain
    	pRegister += Stitch(pRegister);
    	
    	//set the first vertex position
    	pRegister->i128=0;  // Blank out this value
    	pRegister->XYZ2.s.X=(2048 + Position.x - TriSize) << 4;
    	pRegister->XYZ2.s.Y=(2048 + Position.y + TriSize) << 4;
    	pRegister->XYZ2.s.Z=128;
    
    	// Increment register data pointer
    	pRegister++;
    	
    	//call before adding any data to the dma chain
    	pRegister += Stitch(pRegister);
    	
    	// Increment for each vertex
 		pGIFTag->s.NLOOP++;  
    
    	//Set the  second vertex colour
    	pRegister->i128=0;   // Blank out this value
    	pRegister->RGBAQ.s.A=0x80; // Alpha = 128
    	// make the vertex colour red
    	pRegister->RGBAQ.s.R = 0;
    	pRegister->RGBAQ.s.G = 0x80;
    	pRegister->RGBAQ.s.B = 0;
    
    	// Increment register data pointer
    	pRegister++;
    	
    	//call before adding any data to the dma chain
    	pRegister += Stitch(pRegister);
    
    	//Set the second vertex position
    	pRegister->i128=0;  // Blank out this value
    	pRegister->XYZ2.s.X=(2048 + Position.x + TriSize) << 4;
    	pRegister->XYZ2.s.Y=(2048 + Position.y + TriSize) << 4;
    	pRegister->XYZ2.s.Z=128;
    
    	// Increment register data pointer
    	pRegister++;
    	
    	//call before adding any data to the dma chain
    	pRegister += Stitch(pRegister);
    	
    	// Increment for each vertex
    	pGIFTag->s.NLOOP++;  
    
    	//Set the  third vertex position
    	pRegister->i128=0;   // Blank out this value
    	pRegister->RGBAQ.s.A=0x80; // Alpha = 128
    	// make the vertex colour red
    	pRegister->RGBAQ.s.R = 0;
    	pRegister->RGBAQ.s.G = 0;
    	pRegister->RGBAQ.s.B = 0x80;
    
    	// Increment register data pointer
    	pRegister++;
    	
    	//call before adding any data to the dma chain
    	pRegister += Stitch(pRegister);
    
    	//Set the  third vertex position
    	pRegister->i128=0;  // Blank out this value
    	pRegister->XYZ2.s.X=(2048 + Position.x) << 4;
    	pRegister->XYZ2.s.Y=(2048 + Position.y - TriSize) << 4;
    	pRegister->XYZ2.s.Z=128;
    
    	// Increment register data pointer
    	//this will be pointing to the next register data or,
    	//if finshed the loop, it will point to the final DMAC tag
    	//to be written
    	pRegister++;   
    	
    	
    	// Compute the difference between our register data pointer and our initial
		// pointer.  This is the byte count of data written so far.
		// Assert that we haven't written more data than memory allocated
    	iWC = (int) pRegister - (int) pvBase;
		assert(iWC < GSMEMSIZE );
	}
 
 	//end the dma chain
 	EndStitch(pRegister);
 	
 	//print some statistics
	printf("Bytes written to memory = %d \n", iWC);
    
	// iWC Shifted right fourtimes gives us the qword count
	return (iWC >> 4);
}