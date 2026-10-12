/*
Dma Memory Engine - By David Froy
Last Updated: May 1, 2004

This is a simple dma memory manager that takes care of all the hard parts of using
the DMA memory. Allows the user to allocate up to 16 megs of dma memory and 
breaks the memory into a list of 4k pages that can be give to and freed by use of
accessor functions. Also sets up sps2.
*/

#include <stdio.h>
#include "dma4kpage.h"


#ifndef DMA_MEMORY_H
#define DMA_MEMORY_H

//Error messages for Dma memory engine
#define DMA_MEMORY_OK 1
#define DMA_MEMORY_ERROR 2
#define DMA_MEMORY_NOTLOADED 3
#define DMA_MEMORY_ACCESSPASTMEMORY 4
#define DMA_MEMORY_ACCESSBEFOREMEMORY 5


class Dma_Memory_Engine
{
private:
	int NumberOfPages;//Number of pages to be allocated
	int SPS2Descriptor;//The sps2 descriptor, used for interfacing with sps2
	int EngineState;//Engine state of the memory manager
	int CurrentPageNumber;//Current memory page that the manager is working with
	char *pActualMemory;//Pointer to the start of the actual memory
	char *pCurrent4KPage;//The current page the memory manager is working with

	Dma_4KPage *freeMemory;//Link list of all the memory currently free

	//Internal memory manager functions not to be used directly
	int Init();//Does the actual initalizing of the manager
	int Init(int TmpNumberOfPages);//Does the actual initalizing of the manger and allocates the memory

	int init4KPages();//Initalizes the 4k Link list page

	//Functions for working with the engine and also the memory array.
	void *GetMemoryStart();
	void *GetNext4KPage();

	//Pointer to the global sps2memory struct. Required by more many dma components
	sps2Memory_t *pMemory;

public:	
	Dma_Memory_Engine();//Default constructor
	Dma_Memory_Engine(int TmpNumberOfPages);//Constructor that calls init
	~Dma_Memory_Engine();//Default deconstructor

	int GetEngineState();//Get the current state of the engine

	Dma_4KPage *getFree4KPage();//Gets the next free 4k page from the list
	Dma_4KPage *getFree4KPage(int numberOfPages);//Gets a number of chunks of memory pages
	int free4KPages(Dma_4KPage *TmpPageList);//Adds a list of 4k pages back to the list of free pages

	int getSPS2Descriptor();//Get the sps2 descriptor, needed by other objects that required the sps2descriptor
};


#endif
