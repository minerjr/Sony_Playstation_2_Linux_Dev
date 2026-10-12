#ifndef _STITCH_H_
#define _STITCH_H_

// Functions
int Stitch(void * pMem);
void EndStitch(void * pMem);
void InitStitch(void * pMem);

// Externally available variables
extern sps2Memory_t * pMemory;   	// Will store information about memory we allocate

#endif

