#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include "stitch.h"


int iQWC = 0;				// number of qwords written per 4k page
sps2DMATag_t * pDMATag;		// pointer to a DMAtag

// Call this function once EVERYTIME (very important) before you write
// 1 quad word to the DMA chain
int Stitch(void * pMem)
{
	// Do the if statement only when the pointer points to the last qword of a 4k page
	if((((int)pMem) & 0xFFF) == 0xFF0)
	{
		pDMATag->s.QWC = iQWC;			//set QWC of the DMA tag at the start of the page
		iQWC = 1;						//reset iQWC to zero
										//we know we are about to write one qword

		//now write the dmatag next tag at the end of the page
		pDMATag = (sps2DMATag_t *)pMem;		//point to the end of the page
		pDMATag->i64 = 0;					//zero the tag
		pDMATag->s.QWC = 0;					//set the QWC to zero (no data will be sent)
		pDMATag->s.ID = DMA_ID_SOURCE_NEXT;	//this is a next tag
		
		//ADDR points to the location of the next tag 
		//which is a qword ahead, just over the 4k boundary
		pDMATag->s.ADDR = sps2GetPhysicalAddress((void *)(((int)pMem) + 0x10), pMemory);

		//now point to the tag at the start of the next page
		++pDMATag;
		pDMATag->i64 = 0;					//clear it
		pDMATag->s.QWC = 0;					//set its QWC to zero
		pDMATag->s.ID = DMA_ID_SOURCE_CNT;	//set the type

		//return 2 to say we have written 2 128bit tags
		//this is used to move the memory pointer on by 2
		return 2;
	}
	
	//increment the global qword count
	iQWC++;
	
	//return 0 so - we don't want to inccrement the memory pointer
	return 0;
}



void InitStitch(void * pMem)
{
	iQWC = 0;		// Set the QWC to zero
 	//put the first DMA tag in the chain at the start of allocated memory
 	//this is a cnt tag. The qword count of the tag will be set later.
	pDMATag = (sps2DMATag_t *)pMem;
	pDMATag->i64 = 0;
	pDMATag->s.ID = DMA_ID_SOURCE_CNT;
}




void EndStitch(void * pMem)
{
 	// Set the QWC of the last DMATag that we added (If there
	// was no stitching done this will be the first cnt tag
	// otherwise it will be the most recently added cnt tag)
	pDMATag->s.QWC = iQWC;

	//now add the final dma end tag. This will transfer no data but is
	//used to end the transfer.
	pDMATag = (sps2DMATag_t *)pMem;
	pDMATag->i64 = 0;
	pDMATag->s.QWC = 0;
	pDMATag->s.ID = DMA_ID_SOURCE_END;
	
}