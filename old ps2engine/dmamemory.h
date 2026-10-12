#include <stdio.h>
#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>

#ifndef DMA_MEMORY_H
#define DMA_MEMORY_H

#define DMA_MEMORY_OK 1
#define DMA_MEMORY_ERROR 2
#define DMA_MEMORY_NOTLOADED 3
#define DMA_MEMORY_ACCESSPASTMEMORY 4
#define DMA_MEMORY_ACCESSBEFOREMEMORY 5

class Dma_Memory_Engine
{
private:
	int NumberOfPages;
	int SPS2Descriptor;
	int EngineState;
	int CurrentPageNumber;
	sps2Memory_t *pMemory;
	char *pActualMemory;
	char *pCurrent4KPage;
public:	
	Dma_Memory_Engine();
	Dma_Memory_Engine(int TmpNumberOfPages);
	~Dma_Memory_Engine();

	int Init();
	int Init(int TmpNumberOfPages);

	int GetEngineState();
	void *GetMemoryStart();
	void *GetNext4KPage();
	void *GetPrev4KPage();
	void *GetCurrentPage();
	int GetCurrentPageNumber();
	int SetCurrentPageNumber(int PageNumber);
	void *GetPage(int PageNumber);
	unsigned long GetPhysicalAddress(int PageNumber);
	unsigned long GetPhysicalAddress(int PageNumber, int MemoryPart);

};


#endif
