#include "dmamemory.h"

//Pointer to the global sps2memory struct. Required by more many dma components
sps2Memory_t *pMemory;

/*
Name:
	Dma_Memory_Engine
Type:
	Constructor
Arguements:
	None
Returns:
	None
Description
	This is the default constructor for the Dma memory engine. 
	It calls init() to setup the various variables used by the engine.
*/
Dma_Memory_Engine::Dma_Memory_Engine()
{
	//Initalize the variables for the engine
	Init();
}

/*
Name:
	~Dma_Memory_Engine
Type:
	Deconstructor
Arguements:
	None
Returns:
	None
Description
	This is the default deconstructor for the Dma memory engine. 
	It frees the allocated sps2 memory and also releases the sps2 devices
*/
Dma_Memory_Engine::~Dma_Memory_Engine()
{
	//Free the sps2 memory
	sps2Free(pMemory);
	//Release the sps2 devices
	sps2Release(SPS2Descriptor);
}

/*
Name:
	Dma_Memory_Engine
Type:
	Constructor
Arguements:
	int TmpNumberOfPages - the number of 4K pages to allocate
Returns:
	None
Description
	This is another constructor for the Dma memory engine. 
	It calls init(TmpNumberOfPages) to setup the various variables used by the engine
	and also to allocate the number of 4k pages specified
*/
Dma_Memory_Engine::Dma_Memory_Engine(int TmpNumberOfPages)
{
	//Initalize the variables and allocate the specified number of pages
	Init(TmpNumberOfPages);
}

int Dma_Memory_Engine::Init()
{
	NumberOfPages = 0;
	CurrentPageNumber = 0;
	SPS2Descriptor = -1;
	pMemory = NULL;
	freeMemory = NULL;
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

	Init4KPages();//Initalize all of the memory allocated

	EngineState = DMA_MEMORY_OK;
	return EngineState;	
}

int Dma_Memory_Engine::Init4KPages()
{
	Dma_4KPage *tmpPage = NULL;

	freeMemory = new Dma_4KPage((char *)GetMemoryStart());
	tmpPage = freeMemory;
	if (tmpPage == NULL)return 0;
	for (int x = 1; x < NumberOfPages; x++)
	{

		tmpPage->setNext(new Dma_4KPage((char *)GetNext4KPage()) );
		tmpPage->clear();
		tmpPage = tmpPage->getNext();		

	}

	return 1;
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


/*
getFree4KPage:

Arguements: 	none

Returns: 		Dma_4KPage - returns a free 4k page from the list of memory pages availble.

Description:	This function gets the first page from the list of pages and removes it from the list and returns the
			newly removed page.

*/
Dma_4KPage *Dma_Memory_Engine::getFree4KPage()
{
	//Tempary holder of the page to be removed
	Dma_4KPage *tmpReturnPage;

	//Set the tmp page holder to the first free memory list
	tmpReturnPage = freeMemory;
	//If the first page is empty return null
	if (tmpReturnPage == NULL)return NULL;
	//Else set the first free memory page to the second one on the list
	freeMemory = tmpReturnPage->getNext();
	
	//Set the current memory page to return's next to NULL.
	tmpReturnPage->setNext(NULL);

	//Finally return the newly used page.
	return tmpReturnPage;
}


/*
getFree4KPage:

Arguements: 	int numberOfPages - the number of pages to get.

Returns: 		Dma_4KPage - returns a free 4k page from the list of memory pages availble.

Description:	This function gets the first page from the list of pages and removes it from the list and returns the
			newly removed page.

*/
Dma_4KPage *Dma_Memory_Engine::getFree4KPage(int numberOfPages)
{
	//Tempary holder of the page to be removed
	Dma_4KPage *tmpReturnPage;
	//Used to hold the newly allocated page
	Dma_4KPage *tmpNewPage;
	//The working page to step through the new list of pages.
	Dma_4KPage *tmpWorkingPage;

	if (numberOfPages == 0)return NULL;

	tmpReturnPage = getFree4KPage();

	tmpWorkingPage = tmpReturnPage;

	for (int x = 1; x < numberOfPages; x++)
	{
		tmpNewPage = getFree4KPage();
		if (tmpNewPage == NULL) return NULL;
		tmpWorkingPage->setNext(tmpNewPage);
		tmpWorkingPage = tmpWorkingPage->getNext();
	}

	//Finally return the newly used page.
	return tmpReturnPage;
}


/*
free4KPages:

Arguements: 	Dma_4KPage * TmpPageList - the list of memory pages to be re added to the list of free pages

Returns: 		int - Return 1 if the function works, else return 0 on error

Description:	This function readds pages of memory back to the list of free memory. Also clears the memory to 
			make sure the memory is ok when it is requested again.
*/
int Dma_Memory_Engine::free4KPages(Dma_4KPage *TmpPageList)
{
	//Tmp holder used to step through the list of free 4k pages
	Dma_4KPage *tmpPage;

	//If the list sent in is empty return 0;
	if (TmpPageList == NULL)return 0;

	//Set the tmp page to first of the list
	tmpPage = freeMemory;

	//If the first of the list is empty
	if (tmpPage == NULL)
	{
		freeMemory = TmpPageList;
	}

	//Get to the end of list
	while(tmpPage->getNext() != NULL)
		tmpPage = tmpPage->getNext();

	//Add the list of freed pages to the end of the list.
	tmpPage->setNext(TmpPageList);
	
	//Step to the first of the newly freed pages
	tmpPage = tmpPage->getNext();

	//Loop through all of the freed pages and clear thier memory
	while(tmpPage != NULL)
	{
		tmpPage->clear();
		tmpPage = tmpPage->getNext();
	}

	//return 1 for the function was successful
	return 1;
}


int Dma_Memory_Engine::getSPS2Descriptor()
{
	return SPS2Descriptor;
}

