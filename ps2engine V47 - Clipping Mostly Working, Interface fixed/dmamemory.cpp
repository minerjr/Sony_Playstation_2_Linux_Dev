/*
Dma Memory Engine - By David Froy
Last Updated: May 1, 2004

This is a simple dma memory manager that takes care of all the hard parts of using
the DMA memory. Allows the user to allocate up to 16 megs of dma memory and 
breaks the memory into a list of 4k pages that can be give to and freed by use of
accessor functions
*/

#include "dmamemory.h"

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
	//sps2Free(pMemory);
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

/*
Name:
	Init
Type:
	Initalizer
Arguements:
	None
Returns:
	int - the current state of the engine
Description
	This function sets all of the memory engine variables to their default values
*/
int Dma_Memory_Engine::Init()
{
	//Set all the vars to thier default values
	NumberOfPages = 0;
	CurrentPageNumber = 0;
	SPS2Descriptor = -1;
	//Set the pMemory and freeMemory pointers to NULL
	pMemory = NULL;
	freeMemory = NULL;
	EngineState = DMA_MEMORY_NOTLOADED;
	return EngineState;
}

/*
Name:
	Init
Type:
	Initalizer
Arguements:
	int TmpNumberOfPages - the number of 4k pages to allocate
Returns:
	int - the current state of the engine
Description
	This function sets up sps2 and also allocates the number of 4k pages specified
*/
int Dma_Memory_Engine::Init(int TmpNumberOfPages)
{
	//Set the number of pages to the value passed in
	NumberOfPages = TmpNumberOfPages;
	//Inialize sps2
	SPS2Descriptor = sps2Init();
	//Allocate the number of specified 4k packets
	pMemory = sps2Allocate(NumberOfPages * 4096, SPS2_MAP_BLOCK_4K | SPS2_MAP_UNCACHED, SPS2Descriptor);
	//Set the pActualMemory pointer to the start of the allocated memeory
	pActualMemory = (char *)pMemory->pvStart;
	CurrentPageNumber = 0;

	init4KPages();//Initalize all of the memory allocated

	//Set the engine stat to ok
	EngineState = DMA_MEMORY_OK;
	return EngineState;	
}

/*
Name:
	Init4KPages
Type:
	Initalizer
Arguements:
	None
Returns:
	int - 1 for success, 0 for error
Description
	This function sets up all of the links to each of the 4k pages and clears out the memory
*/
int Dma_Memory_Engine::init4KPages()
{
	//Link list worker variable
	Dma_4KPage *tmpPage = NULL;

	//Create the head of the list from the start of the 4k memory
	freeMemory = new Dma_4KPage((char *)GetMemoryStart(), pMemory);
	//Set the worker variable to the head of the list
	tmpPage = freeMemory;
	//If the list is empty then return 0
	if (tmpPage == NULL)return 0;
	//Loop from 1 to the number of 4k pages -1
	for (int x = 1; x < NumberOfPages; x++)
	{
		//Set the next link of the current 4k page to a the next free 4k page
		tmpPage->setNext(new Dma_4KPage((char *)GetNext4KPage(), pMemory) );
		//Clear the memory of the current link
		tmpPage->clear();
		//Step through to the next link and repeat
		tmpPage = tmpPage->getNext();		

	}

	return 1;
}

/*
Name:
	GetEngineState
Type:
	Accessor
Arguements:
	None
Returns:
	int -  the current engine state
Description
	Returns the current engine state
*/
int Dma_Memory_Engine::GetEngineState()
{
	return EngineState;
}


/*
Name:
	GetMemoryStart
Type:
	Accessor
Arguements:
	None
Returns:
	void * - pointer to the start of the pMemory object
Description
	Returns the pointer to the start of the pMemory object
*/
void *Dma_Memory_Engine::GetMemoryStart()
{
	return (void *)pMemory->pvStart;
}

/*
Name:
	GetNext4KPage
Type:
	Accessor
Arguements:
	None
Returns:
	void * - pointer to the next free 4k page, NULL on error
Description
	Returns the pointer to the next free 4k page
*/
void *Dma_Memory_Engine::GetNext4KPage()
{
	//If the current number is at the end of the total number of pages then
	if ((CurrentPageNumber + 1) > NumberOfPages)
	{
		//Set the engine state to access past memory
		EngineState = DMA_MEMORY_ACCESSPASTMEMORY;
		//and return null
		return NULL;
	}
	else
	{	
		//Incease the current page number
		CurrentPageNumber++;
		//Get the pointer to the actual 4k page to return
		pCurrent4KPage = &pActualMemory[CurrentPageNumber * 4096];
		//Set the engine state to ok
		EngineState = DMA_MEMORY_OK;
		//and return the pointer to the memory page
		return (void *)pCurrent4KPage;
	}
}


/*
Name:
	getFree4KPage
Type:
	Accessor
Arguements:
	None
Returns:
	Dma_4KPage - returns a free 4k page from the list of memory pages availble.
Description
	This function gets the first page from the list of pages and removes it from 
	the list and returns the newly removed page.
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
Name:
	getFree4KPage
Type:
	Accessor
Arguements:
	int numberOfPages - the number of pages to get
Returns:
	Dma_4KPage - returns a number of free 4k pages from the list of memory pages availble.
				NULL on error
Description
	This function gets the first page from the list of pages and removes it 
	from the list and returns the newly removed pages.
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

	//Get a free 4k page
	tmpReturnPage = getFree4KPage();

	//Set the work var to the freed 4k page
	tmpWorkingPage = tmpReturnPage;

	//Loop from 1 to the number of pages -1
	for (int x = 1; x < numberOfPages; x++)
	{
		//Set tmpNewPage a free 4k page
		tmpNewPage = getFree4KPage();
		//If the new page is NULL return NULL
		if (tmpNewPage == NULL) return NULL;
		//Set the current pages next the newly found 4k page
		tmpWorkingPage->setNext(tmpNewPage);
		//Set the current page to the newly found page and repeat
		tmpWorkingPage = tmpWorkingPage->getNext();
	}

	//Finally return the newly used page.
	return tmpReturnPage;
}


/*
Name:
	free4KPages
Type:
	Accessor
Arguements:
	Dma_4KPage * TmpPageList - the list of memory pages to be re added to the list of free pages
Returns:
	int - Return 1 if the function works, else return 0 on error
Description
	This function reads pages of memory back to the list of free memory.
	Also clears the memory to make sure the memory is ok when it is requested again.
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

/*
Name:
	getSPS2Descriptor
Type:
	Accessor
Arguements:
	None
Returns:
	int - returns the sps2 descriptor
Description
	This function returns the sps2 descriptor
*/
int Dma_Memory_Engine::getSPS2Descriptor()
{
	return SPS2Descriptor;
}

