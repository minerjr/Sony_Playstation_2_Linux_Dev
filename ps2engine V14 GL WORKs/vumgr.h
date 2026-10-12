/*
VU Manager by David Froy

Last Modifed April 9, 2004

Simple vu manager that opens up access to the vu's that the user
specifies and also uses Dma to transfer micro programs to
a vu that the user specifies.
*/

#ifndef VUMGR_H
#define VUMGR_H


#include "dmamemory.h"

#define VU0_ENABLED 1
#define VU1_ENABLED 2

#define VU0 0
#define VU1 1

class VU_Manager
{
private:
	int vu0;
	int vu1;
	int sps2Descriptor;
	int vuEnabled;
	Dma_Memory_Engine *DmaMemoryEngine;

	int enableVU();
	int disableVU();
	
public:
	VU_Manager(Dma_Memory_Engine *tmpDmaMemoryEngine, int tmpVuEnabled);
	~VU_Manager();

	int vuUpload(int vuNumber, char *fileName);
};

#endif
