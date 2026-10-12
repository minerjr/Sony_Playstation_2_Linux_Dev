#include "main.h"

int main()
{

	printf("Hello, World!\n");
	Dma_Memory_Engine *DmaMemoryEngine;
	DmaMemoryEngine = new Dma_Memory_Engine(3072);

	printf("Dma Starting Physical Address 5 = %lu\n",DmaMemoryEngine->GetPhysicalAddress(5));
	printf("Dma Starting Physical Address 5 Part 0 = %lu\n",DmaMemoryEngine->GetPhysicalAddress(5,0));
	printf("Dma Starting Physical Address 5 Part 1 = %lu\n",DmaMemoryEngine->GetPhysicalAddress(5,1));

	printf("Dma Starting Physical Address 5 Part 4095 = %lu\n",DmaMemoryEngine->GetPhysicalAddress(5,4095));
	printf("Dma Starting Physical Address 5 Part 4096 = %lu\n",DmaMemoryEngine->GetPhysicalAddress(5,4096));

	printf("Dma Starting Physical Address 3071 Part 4095 = %lu\n",DmaMemoryEngine->GetPhysicalAddress(3071,4095));
	printf("Dma Starting Physical Address 3071 Part 4096 = %lu\n",DmaMemoryEngine->GetPhysicalAddress(3071,4096));

	printf("Dma Engine State = %d\n", DmaMemoryEngine->GetEngineState());
	delete DmaMemoryEngine;

	return 0;

}
