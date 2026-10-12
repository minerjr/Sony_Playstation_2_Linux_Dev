/*
Dma 4K Page - By David Froy
Last Updated: May 2, 2004

This is a link list of the individual 4k pages used by the memory manager and
the dma packet class. This allows for easy getting and returning of memory pages
and keeping track of what is allocated and what is free
*/

#include "sps2inc.h"

#ifndef DMA4KPAGE_H
#define DMA4KPAGE_H

class Dma_4KPage
{
private:
	char *pPageMemory;//Pointer to actual memory of the 4k page
	Dma_4KPage *next;//Link to the next page of memory
	sps2Memory_t *pMemory;//Link to ps2 memory object, needed by getPhysicalAdress

public:

	Dma_4KPage(sps2Memory_t *TmppMemory);//Default constructor
	Dma_4KPage(char *TmppPageMemory, sps2Memory_t *TmppMemory);//Constructor with specifed page
																//to use
	~Dma_4KPage();//Default deconstructor

	//These are needed when sending dma chains and dma stiching
	unsigned long getPhysicalAddress();//get phyisical address of memory
	unsigned long getPhysicalAddress(int MemoryPart);//Get physical address of specifed memory
	
	char *getpPageMemory();//Get the pionter to the actual 4k page

	void clear();//Erase all the data in the 4k page

	void setNext(Dma_4KPage *TmpNext);//Set the next pointer to a the specified node
	Dma_4KPage *getNext();//Returns the next pointer

};


#endif
