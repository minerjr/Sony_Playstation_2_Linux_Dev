#include "dmamemory.h"

Dma_Memory_Engine::Dma_Memory_Engine()
{
	Init();
}

Dma_Memory_Engine::~Dma_Memory_Engine()
{
	sps2Free(pMemory);
	sps2Release(SPS2Descriptor);
}

Dma_Memory_Engine::Dma_Memory_Engine(int TmpNumberOfPages)
{
	Init(TmpNumberOfPages);
}

int Dma_Memory_Engine::Init()
{
	NumberOfPages = 0;
	CurrentPageNumber = 0;
	SPS2Descriptor = -1;
	pMemory = NULL;
	EngineState = DMA_MEMORY_NOTLOADED;
	return EngineState;
}

int Dma_Memory_Engine::Init(int TmpNumberOfPages)
{
	NumberOfPages = TmpNumberOfPages;
	SPS2Descriptor = sps2Init();
	pMemory = sps2Allocate(NumberOfPages * 4096, SPS2_MAP_BLOCK_4K | SPS2_MAP_UNCACHED, SPS2Descriptor);
	pActualMemory = (char *)pMemory->pvStart;
	CurrentPageNumber = 0;
	EngineState = DMA_MEMORY_OK;
	return EngineState;	
}

int Dma_Memory_Engine::GetEngineState()
{
	return EngineState;
}

void *Dma_Memory_Engine::GetMemoryStart()
{
	return (void *)pMemory->pvStart;
}

void *Dma_Memory_Engine::GetNext4KPage()
{
	if ((CurrentPageNumber + 1) > NumberOfPages)
	{
		EngineState = DMA_MEMORY_ACCESSPASTMEMORY;
		return NULL;
	}
	else
	{	
		CurrentPageNumber++;
		pCurrent4KPage = &pActualMemory[CurrentPageNumber * 4096];
		EngineState = DMA_MEMORY_OK;
		return (void *)pCurrent4KPage;
	}
}

void *Dma_Memory_Engine::GetPrev4KPage()
{
	if ((CurrentPageNumber - 1) < 0)
	{
		EngineState = DMA_MEMORY_ACCESSBEFOREMEMORY;
		return NULL;
	}
	else
	{	
		CurrentPageNumber--;
		pCurrent4KPage = &pActualMemory[CurrentPageNumber * 4096];
		EngineState = DMA_MEMORY_OK;
		return (void *)pCurrent4KPage;
	}
}

void *Dma_Memory_Engine::GetCurrentPage()
{
	return (void *)pCurrent4KPage;
}


int Dma_Memory_Engine::GetCurrentPageNumber()
{
	return CurrentPageNumber;
}

int Dma_Memory_Engine::SetCurrentPageNumber(int PageNumber)
{
	if (PageNumber > 0 && PageNumber <= NumberOfPages)
	{
		CurrentPageNumber = PageNumber;
		pCurrent4KPage = &pActualMemory[CurrentPageNumber * 4096];
		EngineState = DMA_MEMORY_OK;
		return EngineState;
	}
	else
	{
		EngineState = DMA_MEMORY_ERROR;
		return EngineState;
	}
}

void *Dma_Memory_Engine::GetPage(int PageNumber)
{
	if (PageNumber > 0 && PageNumber <= NumberOfPages)
	{
		EngineState = DMA_MEMORY_OK;
		return (void *)&pActualMemory[PageNumber * 4096];
	
	}
	else
	{
		EngineState = DMA_MEMORY_ERROR;
		return NULL;
	}
	
}

unsigned long Dma_Memory_Engine::GetPhysicalAddress(int PageNumber)
{
	if (PageNumber >= 0 && PageNumber < NumberOfPages)
	{
		EngineState = DMA_MEMORY_OK;
		return sps2GetPhysicalAddress( (void *)&pActualMemory[PageNumber * 4096], pMemory);
	
	}
	else
	{
		EngineState = DMA_MEMORY_ERROR;
		return 0;
	}
	
}

unsigned long Dma_Memory_Engine::GetPhysicalAddress(int PageNumber, int MemoryPart)
{
	if (PageNumber >= 0 && PageNumber < NumberOfPages && MemoryPart < 4096)
	{
		EngineState = DMA_MEMORY_OK;
		return sps2GetPhysicalAddress( (void *)&pActualMemory[(PageNumber * 4096) + MemoryPart] , pMemory);
	
	}
	else
	{
		EngineState = DMA_MEMORY_ERROR;
		return 0;
	}
	
}

