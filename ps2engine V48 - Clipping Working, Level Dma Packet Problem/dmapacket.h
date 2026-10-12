#include "sps2inc.h"
#include "dmamemory.h"
//#include "globals.h"
#include "dma4kpage.h"
#include "structs.h"

#ifndef DMAPACKET_H
#define DMAPACKET_H

#define PATH2ENABLED 1
#define PATH2DISABLED 0

/*
/Class Dma_Packet
/This class is used all dma packets. Used for schedaling of packets and also
/for storing of the packets.
*/

class Dma_Packet
{
private:
	int IDNumber;//Packet ID number given when a new packet is created
	int IsInUse;
	int send;

	int callCntFlag;

	int channelNumber;
	int priority;

	int path2Send;
	int path2Enabled;
	uint64 *pDirectTag;
	uint32 directCount;
	

	sps2DMATag_t *pDMATag;//Link to the dma tag
	Dn_CHCR_t	chcr;// Channel control register used to control the DMA transfer
	Dn_MADR_t	madr;//Address of the first packet's psycial address
	int iQWC;	//Number of qwords
	int iQWCpart;//Number of uint32's to be added. when it hits 4 or greater iQWC increases

	Dma_Memory_Engine *DmaMemoryEngine;

	Dma_4KPage *packetMemory;//Link list of 4k paged memory
	Dma_4KPage *currentPage;//Current working 4K page
	Dma_4KPage *currentUpdatePage;

	uint32 *pCurrent;//Used for stepping through the memory
	uint32 *pUpdateCurrent;
	uint32 *pUpdateStart;
	
	int stitch(int tmpiQWCpart);

	int init(int numberOfPages);


public:
	Dma_Packet();
	
	Dma_Packet(int numberOfPages, int tmpchannelNumber, int tmpCallCntFlag, Dma_Memory_Engine *tmpDmaMemoryEngine);
	~Dma_Packet();
	
	int finish();

	int transfer();

	int clear();

	int reset();

	int add(uint32 tmpword);
	int add(uint64 tmpQword);
	int add(uint128 tmp2Qword);

	int add(uint32 *tmpword, int count);
	int add(uint64 *tmpQword, int count);
	int add(uint128 *tmp2Qword, int count);

	//used for updating data in the dma packet
	int update(uint32 tmpword);
	int update(uint64 tmpQword);
	int update(uint128 tmp2Qword);

	int update(uint32 *tmpword, int count);
	int update(uint64 *tmpQword, int count);
	int update(uint128 *tmp2Qword, int count);

	int updateStitch(int tmpiQWCpart);
	int updateSetup();
	int updateMove(int moveAmount);
	int updateReset();
	uint32 *Dma_Packet::getUpdateCurrent();
	

	int setChannel(int tmpchannelNumber);
	int getChannel();

	uint32 *getCurrent();
	uint128 getStartPhy();
	void dump(char *name);

	int setSend(int tmpSend);//Set the send flag
	int getSend();//Get the send flag

	int usePath2();
	int setTTE(int newTTE);
	int startDirect();
	int finishDirect();
};

#endif
