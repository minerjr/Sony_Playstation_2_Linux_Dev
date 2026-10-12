#include "sps2inc.h"
#include "dmapacket.h"

struct Dma_Packet_List
{
	Dma_Packet *current;
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

	int init();

public:

	Dma_Memory_Scheduler();
	~Dma_Memory_Scheduler();

	int buildList(int tmpListToBuild);
	
	int add(Dma_Packet *tmpNewPacket);
	int remove(Dma_Packet *tmpRemovePacket);
	int transfer(int tmpListToSend);
	int waitForTransfer(int tmpListToWaitFor);
	int dump(char *name);
};
