#include <memory.h>

#include "sps2globals.h"

#include "dma4kpage.h"

Dma_4KPage::Dma_4KPage()
{
	pPageMemory = NULL;
	pMemory = NULL;
	next = NULL;
}

Dma_4KPage::Dma_4KPage(char *TmppPageMemory)
{
	pPageMemory = TmppPageMemory;
	
	next = NULL;
	
}

unsigned long Dma_4KPage::getPhysicalAddress()
{
	if (pPageMemory != NULL)
	{
		//EngineState = DMA_MEMORY_OK;
		return sps2GetPhysicalAddress( (void *)pPageMemory, pMemory);
	
	}
	else
	{
		//EngineState = DMA_MEMORY_ERROR;
		return 0;
	}
	
}

unsigned long Dma_4KPage::getPhysicalAddress(int MemoryPart)
{
	if (pPageMemory != NULL && MemoryPart < 4096)
	{
		//EngineState = DMA_MEMORY_OK;
		return sps2GetPhysicalAddress( (void *)&pPageMemory[MemoryPart] , pMemory);
	
	}
	else
	{
		//EngineState = DMA_MEMORY_ERROR;
		return 0;
	}
	
}

char *Dma_4KPage::getpPageMemory()
{
	return pPageMemory;
}

/*
clear:

Arguements: 	none

Returns: 		none

Description:	This function zeros out the memory located in this 4k page.
*/
void Dma_4KPage::clear()
{
	memset(pPageMemory, 0, 4096);
}

void Dma_4KPage::setNext(Dma_4KPage *TmpNext)
{
	next = TmpNext;	
}

Dma_4KPage *Dma_4KPage::getNext()
{
	return next;
}
