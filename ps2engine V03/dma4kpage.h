#include "sps2inc.h"

#ifndef DMA4KPAGE_H
#define DMA4KPAGE_H

class Dma_4KPage
{
private:
	char *pPageMemory;//Pointer to actual memory of the 4k page
	Dma_4KPage *next;//Link to the next page of memory

	//sps2Memory_t *pMemory;//Pointer to sps2 memory allocated
public:
	Dma_4KPage();
	Dma_4KPage(char *TmppPageMemory);
	~Dma_4KPage();
	unsigned long getPhysicalAddress();
	unsigned long getPhysicalAddress(int MemoryPart);
	char *getpPageMemory();

	void clear();

	void setNext(Dma_4KPage *TmpNext);
	Dma_4KPage *getNext();

};


#endif
