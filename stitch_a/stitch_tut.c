/*
Henry S Fortuna Sept 2003

Illustration of the use of memory stitching using 
the DMAC in source chain mode.
*/

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>
#include "defines.h"
#include "packet.h"


// Information about SPS2 memory allocate
sps2Memory_t * pMemory;   	

/**
 * The main function of the tutorial.  It will perform some 
 * initialisation and then set up a loop to repeatedly render the screen
 */
int main(void)
{
	
	int iSPS2Descriptor;      	// The handle we use for sps2
	Dn_CHCR_t chcrValue;      	// Channel address register
	Dn_TADR_t tadr;				// Tag address register

  	//seed the random number generator
  	srand(time(NULL));

	// Initialize sps2
	iSPS2Descriptor=sps2Init();      

	// Allocate memory. Use this memory uncached so that writes to the memory are
	// immediately committed. GSMEMSIZE defined in defines.h
	// pMemory defined in stitch.c (possibly not the best idea!)
	pMemory = sps2Allocate(GSMEMSIZE, 
						SPS2_MAP_BLOCK_4K | SPS2_MAP_UNCACHED,
		       			iSPS2Descriptor);
	
	
	// Initialize the screen.  Zero tells sps2UScreenInit to exit gracefully
	// when it receives a signal.
	sps2UScreenInit(0);

	// Load the packet data.  This function returns the number of
    // qwords. The memory is loaded up once since none of the GS data 
    // changes in this example. See packet.c
    LoadPacketData(pMemory->pvStart);
	
	// chcr is used to control (start, stop) DMAC transfers.
	// Since we need the same chcr value each time we initialize it once.
	chcrValue.i32=0;                  	// Blank everything out
	chcrValue.s.MOD=CHCR_MOD_CHAIN;  	// We'll do a source chain dma transfer
	chcrValue.s.STR=1;                	// STR=1 = start transfer

	// Get the physical address of our data
	tadr.i32 = 0;
	tadr.s.ADDR = sps2GetPhysicalAddress(pMemory->pvStart, pMemory);


	// Do the render loop 
	do
	{
  		
    	// Clear the new screen.  Let's make it a dark purple background.
    	sps2UScreenClear(0x10, 0, 0x20);
		
    	// Set the QWC to zero, this will cause the DMAC to immediately load and execute
		// the tag pointed to by the TADR register when the transfer is started
    	*EE_D2_QWC = 0;

    	// Set the physical address of our data to the tag address register
	   	*EE_D2_TADR = tadr.i32;

    	// Start the DMA transfer
    	*EE_D2_CHCR = chcrValue.i32;

    	// Wait for the DMA transfer to finish
    	sps2WaitForDMA(2, iSPS2Descriptor);
  
    	// Swap displays now that we're done
		sps2UScreenSwap();
  
  	}while(1);
	
	 
  	// Shut down the screen
  	sps2UScreenShutdown();

  	// Free the memory
  	sps2Free(pMemory);

  	// and close sps2
  	sps2Release(iSPS2Descriptor);
  
  	return 0;
}
