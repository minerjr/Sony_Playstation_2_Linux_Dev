/*
Dma 4K Page - By David Froy
Last Updated: May 2, 2004

This is a link list of the individual 4k pages used by the memory manager and
the dma packet class. This allows for easy getting and returning of memory pages
and keeping track of what is allocated and what is free
*/

#include <memory.h>
#include "dma4kpage.h"

/*
Name:
	Dma_4KPage
Type:
	Constructor
Arguements:
	sps2Memory_t *TmppMemory - pointer to the sps2 memory object
Returns:
	None
Description
	This is the default constructor for the Dma 4K page. 
	It sets the pMemory pointer to the one passed in and sets the class's var to NULL
*/
Dma_4KPage::Dma_4KPage(sps2Memory_t *TmppMemory)
{
	//Set the pointer to the actual 4k page to NULL
	pPageMemory = NULL;
	//Set the pointer to the sps2 memory object to TmppMemory
	pMemory = TmppMemory;
	//Set the next pointer to NULL
	next = NULL;
}

/*
Name:
	Dma_4KPage
Type:
	Constructor
Arguements:
	char *TmppPageMemory - pointer to the actual 4K page to get
	sps2Memory_t *TmppMemory - pointer to the sps2 memory object
Returns:
	None
Description
	This is the default constructor for the Dma 4K page. 
	It sets the pMemory pointer to the one passed in and also the pointer
	to the acutal 4K page.
*/
Dma_4KPage::Dma_4KPage(char *TmppPageMemory, sps2Memory_t *TmppMemory)
{
	//Set the pointer to the acutal 4k page to TmppPageMemory
	pPageMemory = TmppPageMemory;
	//Set the pointer to the sps2 memory object to TmppMemory
	pMemory = TmppMemory;
	next = NULL;
	
}


/*
Name:
	getPhysicalAddress
Type:
	Accessor
Arguements:
	None
Returns:
	unsigned long - physical address of the 4k page, 0 on error
Description
	This function returns the physical address of the 4k page
*/
unsigned long Dma_4KPage::getPhysicalAddress()
{
	//If there is actually a pointer to the 4k page then
	if (pPageMemory != NULL)
	{
		//Return the phisical address of the memory page
		return sps2GetPhysicalAddress( (void *)pPageMemory, pMemory);
	
	}
	else
	{
		//Else return 0
		return 0;
	}
	
}

/*
Name:
	getPhysicalAddress
Type:
	Accessor
Arguements:
	int MemoryPart - a specified point in the 4k page, from 0 to 4095
Returns:
	unsigned long - physical address of the 4k page, 0 on error
Description
	This function returns the physical address of the 4k page at the specifed part in the
	4k page.
*/
unsigned long Dma_4KPage::getPhysicalAddress(int MemoryPart)
{
	//If there is actually a pointer to the 4k page and MemoryPart is less then 4096 then
	if (pPageMemory != NULL && MemoryPart < 4096)
	{
		//Return the phisical address of the memory page
		return sps2GetPhysicalAddress( (void *)&pPageMemory[MemoryPart] , pMemory);
	
	}
	else
	{
		//Else return 0
		return 0;
	}
	
}

/*
Name:
	getpPageMemory
Type:
	Accessor
Arguements:
	None
Returns:
	char * - pointer to the actual 4k page
Description
	This function returns a pointer to the actual 4k page of memory
*/
char *Dma_4KPage::getpPageMemory()
{
	return pPageMemory;
}

/*
Name:
	clear
Type:
	Accessor
Arguements:
	None
Returns:
	None
Description
	This function zeros out the 4k page
*/
void Dma_4KPage::clear()
{
	memset(pPageMemory, 0, 4096);
}

/*
Name:
	setNext
Type:
	Accessor
Arguements:
	Dma_4KPage *TmpNext - pointer the next node to be set into the list
Returns:
	None
Description
	This function sets the next pointer to the value passed in
*/
void Dma_4KPage::setNext(Dma_4KPage *TmpNext)
{
	next = TmpNext;	
}

/*
Name:
	getNext
Type:
	Accessor
Arguements:
	None
Returns:
	Dma_4KPage * - pointer to the next node on the list
Description
	This function gets the next pointer
*/
Dma_4KPage *Dma_4KPage::getNext()
{
	return next;
}
