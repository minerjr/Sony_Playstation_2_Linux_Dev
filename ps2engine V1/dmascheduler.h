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
	

	Dma_Packet *c1FirstPacketList;
	Dma_Packet *c1SecondPacketList;
	Dma_Packet *c1CurrentPacketList;
	Dma_Packet_List *c1PacketList;

	Dma_Packet *c2FirstPacketList;
	Dma_Packet *c2SecondPacketList;
	Dma_Packet *c2CurrentPacketList;
	Dma_Packet_List *c2PacketList;

	int init();

public:

	Dma_Memory_Scheduler();
	~Dma_Memory_Scheduler();



	int BuildList(int tmpListToBuild);
	
	int add(Dma_Packet *tmpNewPacket);
	int remove(Dma_Packet *tmpRemovePacket);
	int transfer(int tmpListToSend);
	

};