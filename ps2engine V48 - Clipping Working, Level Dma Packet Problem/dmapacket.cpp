#include <stdio.h>

#include "dmamemory.h"
#include "structs.h"
#include "dmapacket.h"


inline static uint128 * align128(uint32* pPtr)
{
	while((uint32)pPtr & 0xc)
	{
		*pPtr++ = 0;
	}
	return (uint128*)pPtr;
}

Dma_Packet::Dma_Packet()
{
	callCntFlag = 0;
	init(1);
	path2Send = PATH2DISABLED;
	path2Enabled = PATH2DISABLED;
}

Dma_Packet::Dma_Packet(int numberOfPages, int tmpchannelNumber, int tmpCallCntFlag, Dma_Memory_Engine *tmpDmaMemoryEngine)
{
	channelNumber = tmpchannelNumber;
	callCntFlag = tmpCallCntFlag;
	DmaMemoryEngine = tmpDmaMemoryEngine;
	init(numberOfPages);
	path2Send = PATH2DISABLED;
	path2Enabled = PATH2DISABLED;
}

Dma_Packet::~Dma_Packet()
{
	DmaMemoryEngine->free4KPages(packetMemory);
}


int Dma_Packet::init(int numberOfPages)
{
	packetMemory = DmaMemoryEngine->getFree4KPage(numberOfPages);

	if (packetMemory == NULL)return 0;

	send = 1;

	currentPage = packetMemory;	
	pCurrent = (uint32 *)currentPage->getpPageMemory();	
	iQWC = 0;
	iQWCpart = 0;
	

	// Get the physical address of our data
	madr.i32 = 0;
	madr.s.ADDR = currentPage->getPhysicalAddress();

	// chcr is used to control (start, stop) DMAC transfers.
	// Since we need the same chcr value each time we initialize it once.
	chcr.i32=0;                  	// Blank everything out
	chcr.s.TTE=1;
	chcr.s.DIR=1;
	chcr.s.MOD=CHCR_MOD_CHAIN;  	// We'll do a source chain dma transfer
	chcr.s.STR=1;                	// STR=1 = start transfer
	if (callCntFlag == 0)
	{
		pDMATag = (sps2DMATag_t *)currentPage->getpPageMemory();
		pDMATag->i64 = 0;
		pDMATag->s.ID = DMA_ID_SOURCE_CNT;

		//Inialize the stitching function first
		//stitch();
		pCurrent+=4;
	}

	return 1;
}

int Dma_Packet::transfer()
{

	//Flush the cache just in case.
	sps2FlushCache(DmaMemoryEngine->getSPS2Descriptor());
	//Switch on the channel number
	switch(channelNumber)
	{
		//Case channel 0
		case 0:
		{
		    *EE_D0_QWC = 0;
		    // Set the physical address of our data to the tag address register
		   	*EE_D0_TADR = madr.i32;

		    // Start the DMA transfer
		    *EE_D0_CHCR = chcr.i32;

	
			break;
	
		}
		//Case channel 1
		case 1:
		{
	    	*EE_D1_QWC = 0;

	    	// Set the physical address of our data to the tag address register
		   	*EE_D1_TADR = madr.i32;

	    	// Start the DMA transfer
			*EE_D1_CHCR = chcr.i32;

			break;
		}
		//Case channel 2
		case 2:
		{
	    	*EE_D2_QWC = 0;

    		// Set the physical address of our data to the tag address register
		   	*EE_D2_TADR = madr.i32;

	    	// Start the DMA transfer
	    	*EE_D2_CHCR = chcr.i32;

			break;
		}
		default:
		{
			return 0;
		}
	}

	

	return 1;
}

int Dma_Packet::finish()
{
	//FIX
	//If the last part of the packet is not full, IE only the first part of the qwc is full
	//then increase the iQWC count by 1
/*
	if (iQWCpart != 0) 
	{
		iQWC++;
		printf("iQWCpart = %d\n", iQWCpart);
		if (path2Send == PATH2ENABLED)directCount++;
	}
*/		

	if (callCntFlag == 0)	
		pDMATag->s.QWC = iQWC;
	
	if (path2Enabled == PATH2ENABLED)finishDirect();

	//Allign the last data to be 128 bit aligned
	pDMATag = (sps2DMATag_t *)align128(pCurrent);

	//Add the proper end tag to the packet
	pDMATag->i64 = 0;
	pDMATag->s.QWC = 0;
	if (callCntFlag == 0)
		pDMATag->s.ID = DMA_ID_SOURCE_RET;
	else 
		pDMATag->s.ID = DMA_ID_SOURCE_END;
	return 1;
}

// Call this function once EVERYTIME (very important) before you write
// 1 quad word to the DMA chain
int Dma_Packet::stitch(int tmpiQWCpart)
{
	iQWCpart += tmpiQWCpart;

	// Do the if statement only when the pointer points to the last qword of a 4k page
	if((((int)pCurrent) & 0xFFF) == 0xFF0 || (((int)pCurrent + tmpiQWCpart) & 0xFFF) > 0xFF0)
	{
		if (path2Enabled == PATH2ENABLED)finishDirect();
		pCurrent = (uint32 *)align128(pCurrent);
		pDMATag->s.QWC = iQWC;			//set QWC of the DMA tag at the start of the page
		iQWC = 1;						//reset iQWC to zero
		iQWCpart = 0;
										//we know we are about to write one qword
		
		//now write the dmatag next tag at the end of the page
		pDMATag = (sps2DMATag_t *)pCurrent;		//point to the end of the page
		pDMATag->i64 = 0;					//zero the tag
		pDMATag->s.QWC = 0;					//set the QWC to zero (no data will be sent)
		pDMATag->s.ID = DMA_ID_SOURCE_NEXT;	//this is a next tag
		
		//ADDR points to the location of the next tag 
		//which is a qword ahead, just over the 4k boundary
		
		if (currentPage->getNext() == NULL)
		{
			currentPage->setNext(DmaMemoryEngine->getFree4KPage());
		}
		currentPage = currentPage->getNext();

		pDMATag->s.ADDR = currentPage->getPhysicalAddress();
		pCurrent = (uint32 *)currentPage->getpPageMemory();

		//now point to the tag at the start of the next page
		pDMATag = (sps2DMATag_t *)pCurrent;
		//If this is a regular data packet then add a cnt dma tag
		if (callCntFlag == 0)
		{
			pDMATag->i64 = 0;					//clear it
			pDMATag->s.QWC = 0;					//set its QWC to zero
			pDMATag->s.ID = DMA_ID_SOURCE_CNT;	//set the type
		
			//return 2 to say we have written 2 128bit tags
			//this is used to move the memory pointer on by 2
			pCurrent+=4;
			
			if (path2Enabled == PATH2ENABLED)startDirect();
			if (path2Send == PATH2ENABLED)directCount++;
		}
		return 2;
	}
	
	//increment the global qword count
	if (iQWCpart > 3)
	{
		iQWC++;
		if (path2Send == PATH2ENABLED)directCount++;
		iQWCpart -= 4;
	}
	
	//return 0 so - we don't want to increment the memory pointer
	return 0;
}

int Dma_Packet::add(uint32 tmpword)
{
	stitch(1);//Stitch the memory together
	*pCurrent = tmpword;//Copy the data into the 
	pCurrent++;//Increase the pointer to the next location
	return 1;
}

int Dma_Packet::add(uint32 *tmpword, int count)
{
	for (int x = 0; x < count; x++)
	{
		stitch(1);//Stitch the memory together
		*pCurrent = *tmpword;//Copy the data into the 
		pCurrent++;//Increase the pointer to the next location
		tmpword++;

	}
	return 1;
}

int Dma_Packet::add(uint64 tmpQword)
{
	uint32 *tmpData = (uint32 *)&tmpQword;

	stitch(2);//Stitch the memory together

	*pCurrent = tmpData[0];//Copy the data into the 
	pCurrent++;//Increase the pointer to the next location

	*pCurrent = tmpData[1];//Copy the data into the 
	pCurrent++;//Increase the pointer to the next location

	return 1;
}

int Dma_Packet::add(uint64 *tmpQword, int count)
{
	uint32 *tmpData;
	for (int x = 0; x < count; x++)
	{
		tmpData = (uint32 *)tmpQword;

		stitch(2);//Stitch the memory together

		*pCurrent = tmpData[0];//Copy the data into the 
		pCurrent++;//Increase the pointer to the next location

		*pCurrent = tmpData[1];//Copy the data into the 
		pCurrent++;//Increase the pointer to the next location

		tmpQword++;

	}
	return 1;
}

int Dma_Packet::add(uint128 tmp2Qword)
{
	uint32 *tmpData = (uint32 *)&tmp2Qword;

	stitch(4);//Stitch the memory together

	*pCurrent = tmpData[0];//Copy the data into the 
	pCurrent++;//Increase the pointer to the next location
	//tmpData++;

	*pCurrent = tmpData[1];//Copy the data into the 
	pCurrent++;//Increase the pointer to the next location
	//tmpData++;

	*pCurrent = tmpData[2];//Copy the data into the 
	pCurrent++;//Increase the pointer to the next location
	//tmpData++;

	*pCurrent = tmpData[3];//Copy the data into the 
	pCurrent++;//Increase the pointer to the next location

	return 1;
}

int Dma_Packet::add(uint128 *tmp2Qword, int count)
{
	uint32 *tmpData;
	for (int x = 0; x < count; x++)
	{
		tmpData = (uint32 *)tmp2Qword;

		stitch(4);//Stitch the memory together

		*pCurrent = tmpData[0];//Copy the data into the 
		pCurrent++;//Increase the pointer to the next location

		*pCurrent = tmpData[1];//Copy the data into the 
		pCurrent++;//Increase the pointer to the next location

		*pCurrent = tmpData[2];//Copy the data into the 
		pCurrent++;//Increase the pointer to the next location

		*pCurrent = tmpData[3];//Copy the data into the 
		pCurrent++;//Increase the pointer to the next location

		tmp2Qword++;

	}
	return 1;
}


uint32 *Dma_Packet::getCurrent()
{
	return pCurrent;

}

uint32 *Dma_Packet::getUpdateCurrent()
{
	return pUpdateCurrent;

}

uint128 Dma_Packet::getStartPhy()
{
	return packetMemory->getPhysicalAddress();
}

void Dma_Packet::dump(char *name)
{
	FILE *dumpFile;
	
	Dma_4KPage *tmpPage = packetMemory;
	uint *tmpData;

	dumpFile = fopen(name, "w+");
	//fprintf(dumpFile, "");
	fflush(dumpFile);
	fclose(dumpFile);

	dumpFile = fopen(name, "a+");

	fprintf(dumpFile, "PS2 - Engine Dump packet\n");

	while (tmpPage != NULL)
	{
		tmpData = (uint32 *)tmpPage->getpPageMemory();
		//fwrite(tmpData, sizeof(uint32), 1024,dumpFile);
		for (int x = 0; x < 4096; x+=16)
		{	
			fprintf(dumpFile,"%d:\t 0x%.8x 0x%.8x 0x%.8x 0x%.8x\n",x, 
				tmpData[0], tmpData[1], tmpData[2], tmpData[3]);
			tmpData+=4; 
		}
		
		tmpPage = tmpPage->getNext();
		fprintf(dumpFile, "\n\n");
	}

	fclose(dumpFile);	
}

int Dma_Packet::setChannel(int tmpchannelNumber)
{
	if (tmpchannelNumber >= 0 && tmpchannelNumber <=9)
	{
		channelNumber = tmpchannelNumber;
		return 1;
	}
	else return 0;
}

int Dma_Packet::getChannel()
{
	return channelNumber;
}


int Dma_Packet::setSend(int tmpSend)
{
	send = tmpSend;
	return 1;
}

int Dma_Packet::getSend()
{
	return send;
}

int Dma_Packet::clear()
{
	Dma_4KPage *tmpPage;

	currentPage = packetMemory;
	pCurrent = (uint32 *)currentPage->getpPageMemory();	
	iQWC = 0;
	iQWCpart = 0;

	tmpPage = packetMemory;
	if (tmpPage == NULL)return 0;

	while(tmpPage != NULL)
	{
		tmpPage->clear();
		tmpPage = tmpPage->getNext();
	}

	DmaMemoryEngine->free4KPages(currentPage->getNext());
	currentPage->setNext(NULL);

	return 1;
}

int Dma_Packet::reset()
{

	clear();

	if (callCntFlag == 0)
	{
		pDMATag = (sps2DMATag_t *)currentPage->getpPageMemory();
		pDMATag->i64 = 0;
		pDMATag->s.ID = DMA_ID_SOURCE_CNT;

		//Inialize the stitching function first
		//stitch();
		pCurrent+=4;
	}

	return 1;
}


//HACK
int Dma_Packet::setTTE(int newTTE)
{
	chcr.s.TTE=newTTE;
	return 1;
}

int Dma_Packet::startDirect()
{
	path2Send = PATH2DISABLED;
	add((uint64)0);
	pDirectTag = (uint64 *)pCurrent;
	add((uint64)0);
	path2Send = PATH2ENABLED;
	directCount = 0;

	return 1;
}

int Dma_Packet::finishDirect()
{
	sps2VIFCodeDIRECT_t VIFDirect;
	sps2VIFCodeFLUSH_t	VIFFlush;
	
	VIFDirect.i32 = 0;
	VIFDirect.s.CMD = VIF_CMD_DIRECT;
	VIFDirect.s.SIZE = directCount;//Subtract one for the first DIRECT VIF Tag
	VIFFlush.i32 = 0;
	VIFFlush.s.CMD = VIF_CMD_FLUSH;
	*pDirectTag = ( ((uint64)VIFDirect.i32) << 32 ) | 0;//VIFFlush.i32;
	
	path2Send = PATH2DISABLED;
	directCount = 0;
	
	return 1;
}

int Dma_Packet::usePath2()
{
	path2Enabled = PATH2ENABLED;
	startDirect();
	return 1;
}



int Dma_Packet::update(uint32 tmpword)
{
	updateStitch(1);//Stitch the memory together
	*pUpdateCurrent = tmpword;//Copy the data into the 
	pUpdateCurrent++;//Increase the pointer to the next location
	return 1;
}

int Dma_Packet::update(uint32 *tmpword, int count)
{
	for (int x = 0; x < count; x++)
	{
		updateStitch(1);//Stitch the memory together
		*pUpdateCurrent = *tmpword;//Copy the data into the 
		pUpdateCurrent++;//Increase the pointer to the next location
		tmpword++;

	}
	return 1;
}

int Dma_Packet::update(uint64 tmpQword)
{
	uint32 *tmpData = (uint32 *)&tmpQword;

	updateStitch(2);//Stitch the memory together

	*pUpdateCurrent = tmpData[0];//Copy the data into the 
	pUpdateCurrent++;//Increase the pointer to the next location

	*pUpdateCurrent = tmpData[1];//Copy the data into the 
	pUpdateCurrent++;//Increase the pointer to the next location

	return 1;
}

int Dma_Packet::update(uint64 *tmpQword, int count)
{
	uint32 *tmpData;
	for (int x = 0; x < count; x++)
	{
		tmpData = (uint32 *)tmpQword;

		updateStitch(2);//Stitch the memory together

		*pUpdateCurrent = tmpData[0];//Copy the data into the 
		pUpdateCurrent++;//Increase the pointer to the next location

		*pUpdateCurrent = tmpData[1];//Copy the data into the 
		pUpdateCurrent++;//Increase the pointer to the next location

		tmpQword++;

	}
	return 1;
}

int Dma_Packet::update(uint128 tmp2Qword)
{
	uint32 *tmpData = (uint32 *)&tmp2Qword;

	updateStitch(4);//Stitch the memory together

	*pUpdateCurrent = tmpData[0];//Copy the data into the 
	pUpdateCurrent++;//Increase the pointer to the next location
	//tmpData++;

	*pUpdateCurrent = tmpData[1];//Copy the data into the 
	pUpdateCurrent++;//Increase the pointer to the next location
	//tmpData++;

	*pUpdateCurrent = tmpData[2];//Copy the data into the 
	pUpdateCurrent++;//Increase the pointer to the next location
	//tmpData++;

	*pUpdateCurrent = tmpData[3];//Copy the data into the 
	pUpdateCurrent++;//Increase the pointer to the next location

	return 1;
}

int Dma_Packet::update(uint128 *tmp2Qword, int count)
{
	uint32 *tmpData;
	for (int x = 0; x < count; x++)
	{
		tmpData = (uint32 *)tmp2Qword;

		updateStitch(4);//Stitch the memory together

		*pUpdateCurrent = tmpData[0];//Copy the data into the 
		pUpdateCurrent++;//Increase the pointer to the next location

		*pUpdateCurrent = tmpData[1];//Copy the data into the 
		pUpdateCurrent++;//Increase the pointer to the next location

		*pUpdateCurrent = tmpData[2];//Copy the data into the 
		pUpdateCurrent++;//Increase the pointer to the next location

		*pUpdateCurrent = tmpData[3];//Copy the data into the 
		pUpdateCurrent++;//Increase the pointer to the next location

		tmp2Qword++;

	}
	return 1;
}

int Dma_Packet::updateSetup()
{
	pUpdateStart = pCurrent;
	pUpdateCurrent = pCurrent;
	currentUpdatePage = packetMemory;
	return 1;
}

int Dma_Packet::updateReset()
{
	pUpdateCurrent = pUpdateStart;
	currentUpdatePage = packetMemory;
	return 1;
}


// Call this function once EVERYTIME (very important) before you write
// 1 quad word to the DMA chain
int Dma_Packet::updateStitch(int tmpiQWCpart)
{
	
	// Do the if statement only when the pointer points to the last qword of a 4k page
	if((((int)pUpdateCurrent) & 0xFFF) == 0xFF0 || (((int)pUpdateCurrent + tmpiQWCpart) & 0xFFF) > 0xFF0)
	{
		
		//Set the currentUpdatePage to the next one
		currentUpdatePage = currentUpdatePage->getNext();

		//Set the UpdateCurrent pointer to the first of the update page
		pUpdateCurrent = (uint32 *)currentUpdatePage->getpPageMemory();

		//If using the CNT flag then update to skip the dma header
		if (callCntFlag == 0)
		{
			pUpdateCurrent+=4;
			//Skip the next 4 dwords for the direct tag
			if (path2Enabled == PATH2ENABLED)pUpdateCurrent+= 4;
			
		}
		return 2;
	}
	
	//return 0 so - we don't want to increment the memory pointer
	return 0;
}

//Used to skip over amounts of memory uses 32bit incriments
int Dma_Packet::updateMove(int moveAmount)
{
	//0xFFF == 4095 in hex
	//0xFF0 == 4080 in hex, ie the last possible 16 bytes
	int currentLocation = ((int)pUpdateCurrent) & 0xFFF;
	//printf("moveAmount 1: %d %d\n", moveAmount, currentLocation);
	
	if (currentLocation + moveAmount * 4 >= 0xFF0)
	{
		//Find the left over amount to move by
		moveAmount = currentLocation / 4 + moveAmount -  0xFF0 / 4;
		//printf("moveAmount 2: %d\n", moveAmount);
		//Set the currentUpdatePage to the next one
		currentUpdatePage = currentUpdatePage->getNext();

		//Set the UpdateCurrent pointer to the first of the update page
		pUpdateCurrent = (uint32 *)currentUpdatePage->getpPageMemory();

		//If using the CNT flag then update to skip the dma header
		if (callCntFlag == 0)
		{
			pUpdateCurrent += 4;
			//Skip the next 4 dwords for the direct tag
			if (path2Enabled == PATH2ENABLED)pUpdateCurrent += 4;
		}
		//return 2;
	}
	pUpdateCurrent += moveAmount;
	currentLocation = ((int)pUpdateCurrent) & 0xFFF;
	//printf("CurrentLocation 3: %d\n", currentLocation);
	return 1;
}

