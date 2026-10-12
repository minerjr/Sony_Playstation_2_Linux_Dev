#include "dmapacket.h"
#include "sps2inc.h"
#include <stdio.h>
#include <stdlib.h>

int vuUpload(int vuNumber, char *fileName, int sps2Descriptor)
{
	FILE *inputFile;
	Dma_Packet *uploadPacket;
	//Max vu program can be 16kb
	//Vif code is actually 8 bytes, qword, not a dqword
	char readData[16384];
	
	int readCount;
	sps2DMATag_t header;
	sps2VIFCodeMPG_t sps2VifMPG;
	sps2VIFCodeNOP_t sps2VifNOP;

	
	inputFile = fopen(fileName,"rb");
	if (inputFile == NULL)return 0;
	printf("Vu file opened properly\n");
	//Create a packet with 1 page of memory, send to path 1 
	//and set its send flag
	switch(vuNumber)
	{
	case 0:
		uploadPacket = new Dma_Packet(1,0,1);
		break;
	case 1:
		uploadPacket = new Dma_Packet(1,1,1);
		break;
	default:
		fclose(inputFile);
		return 0;
	}

	
	printf("Read data from file\n");
	readCount = fread(&readData, 1, 16384, inputFile);
	fclose(inputFile);

	printf("ReadCount = %d\n", readCount);
	printf("done\n");
	header.i64 = 0;
	header.s.ID = DMA_ID_SOURCE_END;
	header.s.QWC = (readCount + 15) >> 4;
	
	sps2VifNOP.i32 = 0;

	sps2VifMPG.i32 = 0;
	sps2VifMPG.s.SIZE = readCount >> 3;
	sps2VifMPG.s.CMD = VIF_CMD_MPG;

	//header.ul32[3] = PS2_DMA_SET_VIFCODE(0, readCount >> 3, VIF_CMD_MPG);

	//Add the dma header
	uploadPacket->add((uint64)header.i64);
	//Add a vif nop
	uploadPacket->add((uint32)sps2VifNOP.i32);
	//Add a vif mpg, micro program transfer
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
	
	//sps2WaitForDMA(1, sps2Descriptor);

	//delete uploadPacket;
	return 1;
}
