#include "dmapacket.h"
#include "sps2inc.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


#include "vumgr.h"

VU_Manager::VU_Manager(Dma_Memory_Engine *tmpDmaMemoryEngine, int tmpVUEnabled)
{
	vuEnabled = tmpVUEnabled;
	DmaMemoryEngine = tmpDmaMemoryEngine;
	enableVU();
}

VU_Manager::~VU_Manager()
{
	disableVU();
}

int VU_Manager::enableVU()
{
	if ((vuEnabled & VU0_ENABLED) == VU0_ENABLED)
	{
		vu0 = open("/dev/ps2vpu0", O_RDWR);
		if (vu0 < 0)
		{
			printf("Error opening VU0\n");
			return 0;
		}
		printf("VU0: Enabled\n");
	}
	
	if ((vuEnabled & VU1_ENABLED) == VU1_ENABLED)
	{
		vu1 = open("/dev/ps2vpu1", O_RDWR);
		if (vu1 < 0)
		{
			printf("Error opening VU1\n");
			return 0;
		}
		printf("VU1: Enabled\n");
	}

	return 1;
	
}

int VU_Manager::disableVU()
{
	if ((vuEnabled & VU0_ENABLED) == VU0_ENABLED)
	{
		close(vu0);
	}
	
	if ((vuEnabled & VU1_ENABLED) == VU1_ENABLED)
	{
		close(vu1);
	}

	return 1;
}

int VU_Manager::vuUpload(int vuNumber, char *fileName)
{
	FILE *inputFile;
	Dma_Packet *uploadPacket;
	//Max vu program can be 16kb
	//Vif code is actually 8 bytes, qword, not a dqword
	char readData[16384];
	uint32 *tmpHeader;
	int readCount;
	sps2VIFCodeMPG_t sps2VifMPG;
	sps2VIFCodeNOP_t sps2VifNOP;

	
	inputFile = fopen(fileName,"rb");
	if (inputFile == NULL)return 0;
	printf("Vu file opened properly\n");
	//Create a packet with 1 page of memory, send to path 1 
	//and set its send flag
	switch(vuNumber)
	{
	case VU0:
		uploadPacket = new Dma_Packet(1,0,0,DmaMemoryEngine);
		break;
	case VU1:
		uploadPacket = new Dma_Packet(1,1,0,DmaMemoryEngine);
		break;
	default:
		fclose(inputFile);
		return 0;
	}

	
	printf("Read data from file\n");
	readCount = fread(&readData, 1, 16384, inputFile);
	fclose(inputFile);

	//The VU0 can only store 4KB of mircro memory
	if (readCount > 4092 && vuNumber == VU0)return 0;

	printf("ReadCount = %d\n", readCount);
	printf("done\n");
	
	sps2VifNOP.i32 = 0;

	sps2VifMPG.i32 = 0;
	sps2VifMPG.s.SIZE = readCount >> 3;
	sps2VifMPG.s.CMD = VIF_CMD_MPG;

	//Add the dma header
	tmpHeader = (uint32 *)uploadPacket->getCurrent();
	tmpHeader -= 2;

	//Add a vif nop
	//tmpHeader[0] = (uint32)sps2VifNOP.i32;
	uploadPacket->add((uint32)sps2VifNOP.i32);

	//Add a vif mpg, micro program transfer
	//tmpHeader[1] = (uint32)sps2VifMPG.i32;
	uploadPacket->add((uint32)sps2VifMPG.i32);

	//Add the micro code data
	printf("Add multiple data to the uploadPacket\n");
	uploadPacket->add((uint64 *)&readData, readCount >> 3);
	printf("done\n");

	uploadPacket->finish();

	printf("Try to transfer micro program\n");
	uploadPacket->dump("vucode.txt");
	uploadPacket->transfer();
	printf("Successful\n");
	
	sps2WaitForDMA(vuNumber, DmaMemoryEngine->getSPS2Descriptor());
	printf("Finished transfering mpg\n");
	delete uploadPacket;
	printf("Deleted uploadPacket\n");
	return 1;
}
