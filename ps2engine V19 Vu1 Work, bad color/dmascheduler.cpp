#include "dmascheduler.h"

Dma_Memory_Scheduler::Dma_Memory_Scheduler(Dma_Memory_Engine *tmpDmaMemoryEngine)
{
	DmaMemoryEngine = tmpDmaMemoryEngine;
	init();

}

Dma_Memory_Scheduler::~Dma_Memory_Scheduler()
{

	delete c0FirstSendList;
	delete c0SecondSendList;
	delete c0PacketList;

	delete c1FirstSendList;
	delete c1SecondSendList;
	delete c1PacketList;

	delete c2FirstSendList;
	delete c2SecondSendList;
	delete c2PacketList;

}


int Dma_Memory_Scheduler::init()
{
	c0FirstSendList = new Dma_Packet(1,0,1,DmaMemoryEngine);
	c0SecondSendList = new Dma_Packet(1,0,1,DmaMemoryEngine);
	c0CurrentSendList = c0FirstSendList;

	c0PacketList = NULL;

	c1FirstSendList = new Dma_Packet(1,1,1,DmaMemoryEngine);
	c1SecondSendList = new Dma_Packet(1,1,1,DmaMemoryEngine);
	c1CurrentSendList = c1FirstSendList;

	c1PacketList = NULL;

	c2FirstSendList = new Dma_Packet(1,2,1,DmaMemoryEngine);
	c2SecondSendList = new Dma_Packet(1,2,1,DmaMemoryEngine);
	c2CurrentSendList = c2FirstSendList;

	c2PacketList = NULL;

	return 1;	

}

int Dma_Memory_Scheduler::add(Dma_Packet *tmpNewPacket)
{
	int tmpDmaChannel;
	Dma_Packet_List *tmpPacketList;

	if (tmpNewPacket == NULL)
		return 0;
	tmpDmaChannel = tmpNewPacket->getChannel();

	switch(tmpDmaChannel)
	{
	case 0:
	{
		tmpPacketList = c0PacketList;
		if (tmpPacketList == NULL)
		{
			c0PacketList = new Dma_Packet_List;
			c0PacketList->next = NULL;
			c0PacketList->current = tmpNewPacket;
			return 1;
			
		}
		break;
		
	}
	case 1:
	{
		tmpPacketList = c1PacketList;
		if (tmpPacketList == NULL)
		{
			c1PacketList = new Dma_Packet_List;
			c1PacketList->next = NULL;
			c1PacketList->current = tmpNewPacket;
			return 1;			
		}
		break;
		
	}
	case 2:
	{
		tmpPacketList = c2PacketList;
		if (tmpPacketList == NULL)
		{
			c2PacketList = new Dma_Packet_List;
			c2PacketList->next = NULL;
			c2PacketList->current = tmpNewPacket;
			return 1;
		}
		break;
	}
	default:
	{
		return 0;
		break;
	}
	}

	
	while(tmpPacketList->next != NULL)
		tmpPacketList = tmpPacketList->next;

	tmpPacketList->next = new Dma_Packet_List;
	tmpPacketList->next->next = NULL;
	tmpPacketList->next->current = tmpNewPacket;

	return 1;

}

int Dma_Memory_Scheduler::buildList(int tmpListToBuild)
{
	Dma_Packet_List *tmpPacketList;
	Dma_Packet *tmpCurrent;
	sps2DMATag_t DmaTag;

	switch(tmpListToBuild)
	{
	case 0:
	{
		tmpPacketList = c0PacketList;
		tmpCurrent = c0CurrentSendList;
		break;
	}
	case 1:
	{
		tmpPacketList = c1PacketList;
		tmpCurrent = c1CurrentSendList;
		break;
	}
	case 2:
	{
		tmpPacketList = c2PacketList;
		tmpCurrent = c2CurrentSendList;
		break;
	}
	default:
	{
		return 0;
	}

	}
	if (tmpCurrent == NULL) return 0;
	tmpCurrent->clear();
	
	while(tmpPacketList != NULL)
	{
		if (tmpPacketList->current->getSend() == 1)
		{
			DmaTag.i64 = 0;
			DmaTag.s.QWC = 0;
			DmaTag.s.ID = DMA_ID_SOURCE_CALL;
			DmaTag.s.ADDR = tmpPacketList->current->getStartPhy();
			tmpCurrent->add((uint128)DmaTag.i64);
		}
		tmpPacketList = tmpPacketList->next;
		
	}
	tmpCurrent->finish();
	return 1;

}


int Dma_Memory_Scheduler::transfer(int tmpListToSend)
{
	Dma_Packet *tmpCurrent;

	//Get the current packet to send and swap the double buffers around
	switch(tmpListToSend)
	{
	case 0:
	{
		tmpCurrent = c0CurrentSendList;
		if (c0CurrentSendList == c0FirstSendList)
		{
			c0CurrentSendList = c0SecondSendList;
		}
		else
		{
			c0CurrentSendList = c0FirstSendList;
		}
		break;
	}
	case 1:
	{
		tmpCurrent = c1CurrentSendList;
		if (c1CurrentSendList == c1FirstSendList)
		{
			c1CurrentSendList = c1SecondSendList;
		}
		else
		{
			c1CurrentSendList = c1FirstSendList;
		}
		break;

	}
	case 2:
	{
		tmpCurrent = c2CurrentSendList;
		if (c2CurrentSendList == c2FirstSendList)
		{
			c2CurrentSendList = c2SecondSendList;
		}
		else
		{
			c2CurrentSendList = c2FirstSendList;
		}
		break;
	}

	}
	
	tmpCurrent->transfer();

	return 1;
}

int Dma_Memory_Scheduler::waitForTransfer(int tmpListToWaitFor)
{
	sps2WaitForDMA(tmpListToWaitFor, DmaMemoryEngine->getSPS2Descriptor());
	return 1;
}

int Dma_Memory_Scheduler::dump(char *name)
{
	c2CurrentSendList->dump(name);
	return 1;
}
