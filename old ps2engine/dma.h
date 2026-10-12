#ifndef DMA_H
#define DMA_H

#include <stdio.h>

#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>

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
	sps2DMATag_t *pDMATag;
	Dn_CHCR_t	chcr;    
	Dn_MADR_t	madr;
	Dn_QWC_t	qwc;
	
public:
	Dma_Packet();
	~Dma_Packet();
	


};







#endif
