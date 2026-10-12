#include "sps2inc.h"
#include "dmamemory.h"
#include "dmapacket.h"
#include "structs.h"

#ifndef TEXTURE_H
#define TEXTURE_H
//#include "tga.h"
//There can be up to four megs of textures in memory at one time
//Plus 12 256 by 256 8bit textures in VRAM


#ifndef CHANNEL2
#define CHANNEL2 2
#endif

struct Texture
{

	
	Dma_Packet *texturePacket;//pointer to dma packet that contains dma memory for the texture
	int textureID;//Id number for texture
	int textureLocationID;//Number from 0 to 11, ie what memory location should it be loaded
	bool isInMemory;//Flag to tell if the texture is in VRAM or not

	/*
	//Transfer tags needed for resending the 
	sps2GIFRegTEXCLUT_t sps2TexClut;
	sps2GIFRegBITBLTBUF_t sps2BitBltBuf;
	sps2GIFRegTRXDIR_t sps2TrxDir;
	sps2GIFRegTRXPOS_t sps2TrxPos;
	sps2GIFRegTRXREG_t sps2TrxReg;
	*/

	Texture *next;
};




class Texture_Engine
{

private:

	//Head of the link list of textures
	Texture *head;
	bool textureSlotOpen[64];//Used to determine which texture slot is open
	int currentTextureID;//Used to determine which texture is the current texture
	int currentLocationID;//Used to determine which texture location is currently used
	Dma_Memory_Engine *DmaMemoryEngine;
	//Dma_Packet transferPackets[11];//pointer to dma packet that contains upload cmd

public:

	Texture_Engine(Dma_Memory_Engine *tmpDmaMemoryEngine);
	~Texture_Engine();

	int loadTexture(char *fileName, int texLocationID = 0);
	int loadBitmap(char *fileName, Texture *textureStruct);
	int loadCIF(char *fileName, Texture *textureStruct);
	int deleteTexture(int textureID);

	int setLocationID(int texID, int texLocationID);

	int checkCurrentTexture(int textureID);

	Dma_Packet *getPacket(int textureID);
	Dma_Packet *getTransferPacket(int textureID);
	Texture *getTexture(int textureID);


};

#endif