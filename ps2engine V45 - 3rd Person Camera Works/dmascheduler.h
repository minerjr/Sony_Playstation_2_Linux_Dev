/*
Dma Scheduler - By David Froy
Last Updated: May 2, 2004

This is a simple dma scheduler that takes care of building dma chains. Each packet has a flag
that can be set to send or not to send the packet. The build packet function reads these values 
and adds the flagged packets to a list to be sent. The list is double bufferred to allow for
one list to be sent while another is being constructed.
*/

#ifndef DMASCHEDULER_H
#define DMASCHEDULER_H

#include "sps2inc.h"
#include "dmamemory.h"
#include "dmapacket.h"

//Link list node struct
struct Dma_Packet_List
{
	//Pointer to current dma packet
	Dma_Packet *current; 
	//Pointer to next packet
	Dma_Packet_List *next;
};

class Dma_Memory_Scheduler
{
private:

	Dma_Packet *c0FirstSendList;
	Dma_Packet *c0SecondSendList;
	Dma_Packet *c0CurrentSendList;
	Dma_Packet_List *c0PacketList;
	

	Dma_Packet *c1FirstSendList;
	Dma_Packet *c1SecondSendList;
	Dma_Packet *c1CurrentSendList;
	Dma_Packet_List *c1PacketList;

	Dma_Packet *c2FirstSendList;
	Dma_Packet *c2SecondSendList;
	Dma_Packet *c2CurrentSendList;
	Dma_Packet_List *c2PacketList;

	Dma_Memory_Engine *DmaMemoryEngine;

	int init();

public:

	Dma_Memory_Scheduler(Dma_Memory_Engine *tmpDmaMemoryEngine);
	~Dma_Memory_Scheduler();

	int buildList(int tmpListToBuild);
	
	int clearList(int tmpListToClear);
	int add(Dma_Packet *tmpNewPacket);
	int remove(Dma_Packet *tmpRemovePacket);
	int transfer(int tmpListToSend);
	int waitForTransfer(int tmpListToWaitFor);
	int dump(char *name);
};

#endif
