/*
Dma Scheduler - By David Froy
Last Updated: May 5, 2004

This is a simple dma scheduler that takes care of building dma chains. Each packet has a flag
that can be set to send or not to send the packet. The build packet function reads these values 
and adds the flagged packets to a list to be sent. The list is double bufferred to allow for
one list to be sent while another is being constructed.
*/

#include "dmascheduler.h"

/*
Name:
	Dma_Memory_Scheduler
Type:
	Constructor
Arguements:
	Dma_Memory_Engine *tmpDmaMemoryEngine - pointer to the memory engine.
Returns:
	None
Description
	This is the default constructor for the Dma memory scheduler
	The dma scheduler takes a pointer to a dma memory engine to allow
	for gathering of more memory if needed for the scheduler packets
	It calls init() to setup the various variables used by the scheduler
*/
Dma_Memory_Scheduler::Dma_Memory_Scheduler(Dma_Memory_Engine *tmpDmaMemoryEngine)
{
	//Set the schedulers pointer to the dma memory engine.
	DmaMemoryEngine = tmpDmaMemoryEngine;
	//Call the init function to initialize the dma scheduler
	init();

}


/*
Name:
	~Dma_Memory_Scheduler
Type:
	Deconstructor
Arguements:
	None
Returns:
	None
Description
	This is the default deconstructor. It deletes all of the allocated packets for
	the dma send lists and also list of dma packets
*/
Dma_Memory_Scheduler::~Dma_Memory_Scheduler()
{
	//Var used to step through the list of dma packets
	Dma_Packet_List *tmpList;

	if (c0FirstSendList)
		delete c0FirstSendList;
	if (c0SecondSendList)
		delete c0SecondSendList;
	

	if (c1FirstSendList)
		delete c1FirstSendList;
	if (c1SecondSendList)
		delete c1SecondSendList;
	
	if (c2FirstSendList)
		delete c2FirstSendList;
	if (c2SecondSendList)
		delete c2SecondSendList;
	
	//Clear all the links to the dma packets
	tmpList = c0PacketList;
	while(tmpList != NULL)
	{
		tmpList->current = NULL;
		tmpList = tmpList->next;
	}

	//Clear all the links to the dma packets
	tmpList = c1PacketList;
	while(tmpList != NULL)
	{
		tmpList->current = NULL;
		tmpList = tmpList->next;
	}

	//Clear all the links to the dma packets
	tmpList = c2PacketList;
	while(tmpList != NULL)
	{
		tmpList->current = NULL;
		tmpList = tmpList->next;
	}

	if (c0PacketList)
		delete c0PacketList;
	if (c1PacketList)
		delete c1PacketList;
	if (c2PacketList)
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
	c1FirstSendList->setTTE(0);
	c1SecondSendList->setTTE(0);
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

int Dma_Memory_Scheduler::remove(Dma_Packet *tmpRemovePacket)
{
	Dma_Packet_List *workPacketList;
	Dma_Packet_List *deletePacketList;
	int tmpDmaChannel = 0;

	if (tmpRemovePacket == NULL)
		return 0;
	tmpDmaChannel = tmpRemovePacket->getChannel();

	switch(tmpDmaChannel)
	{
	case 0:
		{
			workPacketList = c0PacketList;
			if (workPacketList == NULL)
			{
				return 0;
				
			}
			else
			{
				if (workPacketList->current == tmpRemovePacket)
				{
					deletePacketList = workPacketList;
					c0PacketList = c0PacketList->next;
					delete deletePacketList;
					return 1;
				}
			}
		}
	case 1:
		{
			workPacketList = c1PacketList;
			if (workPacketList == NULL)
			{
				return 0;
				
			}
			else
			{
				if (workPacketList->current == tmpRemovePacket)
				{
					deletePacketList = workPacketList;
					c1PacketList = c1PacketList->next;
					delete deletePacketList;
					return 1;
				}
			}
		}
	case 2:
		{
			workPacketList = c2PacketList;
			if (workPacketList == NULL)
			{
				return 0;
				
			}
			else
			{
				if (workPacketList->current == tmpRemovePacket)
				{
					deletePacketList = workPacketList;
					c2PacketList = c2PacketList->next;
					delete deletePacketList;
					return 1;
				}
			}
		}
	}

	while (workPacketList != NULL)
	{
		if (workPacketList->next->current == tmpRemovePacket)
		{
			deletePacketList = workPacketList->next;
			workPacketList->next = deletePacketList->next;
			delete deletePacketList;
			break;
		}
	}

	return 1;
}

int Dma_Memory_Scheduler::clearList(int tmpListToClear)
{
	Dma_Packet_List *workPacketList;
	Dma_Packet_List *deletePacketList;

	switch(tmpListToClear)
	{
	case 0:
		workPacketList = c0PacketList;
		c0PacketList = NULL;
		break;
	case 1:
		workPacketList = c1PacketList;
		c1PacketList = NULL;
		break;
	case 2:
		workPacketList = c2PacketList;
		c2PacketList = NULL;
		break;
	}

	while(workPacketList != NULL)
	{
		deletePacketList = workPacketList;
		workPacketList = workPacketList->next;
		delete deletePacketList;
	}
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
