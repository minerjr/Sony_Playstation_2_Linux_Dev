#include "texture.h"
#include "bitmap.h"
#include "customimage.h"
#include <string.h>
#include <stdio.h>
#include "dmapacket.h"
#include "sps2inc.h"
#include "PS2Defines.h"

#ifndef CHANNEL1
#define CHANNEL1 1
#endif

Texture_Engine::Texture_Engine(Dma_Memory_Engine *tmpDmaMemoryEngine)
{
	head = NULL;
	DmaMemoryEngine = tmpDmaMemoryEngine;

	for (int x = 0; x < 60; x++)
	{
		textureSlotOpen[x] = true;
	}
}
	
Texture_Engine::~Texture_Engine()
{
	Texture *workTexture = head;
	Texture *deleteTexture = head;


	while (workTexture != NULL)
	{
		workTexture = workTexture->next;
		delete deleteTexture->texturePacket;
		delete deleteTexture;
		deleteTexture = workTexture;
	}

	if (deleteTexture)
	{
		delete deleteTexture->texturePacket;
		delete deleteTexture;		
	}

}

int Texture_Engine::loadTexture(char *fileName, int texLocationID)
{

	Texture *newTexture = new Texture;
	newTexture->next = NULL;

	newTexture->textureLocationID = texLocationID;

	if (strstr(fileName, ".bmp") != NULL)
	{
		if(loadBitmap(fileName, newTexture) == -1)
		{
			printf("Failed to load BMP ");
			delete newTexture->texturePacket;
			delete newTexture;
			return -1;
		}
	}
	else if (strstr(fileName, ".cif") != NULL)
	{
		if(loadCIF(fileName, newTexture) == -1)
		{
			printf("Failed to load CIF ");
			delete newTexture->texturePacket;
			delete newTexture;
			return -1;
		}
	}
	else
	{
		printf("File type not supported ");
		delete newTexture->texturePacket;
		delete newTexture;
		return -1;
	}
	

	if (head == NULL)
	{
		head = newTexture;
	}
	else
	{
		Texture *workTexture = head;
		while (workTexture->next != NULL)
		{
			workTexture = workTexture->next;
		}
		workTexture->next = newTexture;
	}

	return newTexture->textureID;
}

int Texture_Engine::loadBitmap(char *fileName, Texture *textureStruct)
{
	Bitmap_Loader *bitmapFile = NULL;
	int x = 0;
	
	int clutData[256];
	int swizzledClutData[256];
	char imageData[256 * 256 * 1];
	unsigned char *pPtr;
	char red;

	//Convert the PAGE to BLOCKS. IE there are 32 blocks in 1 page. So multiply by 32. (<< 5)
	uint32 firstPage = sps2UScreenGetFirstFreeGSPage() << 5;
	printf("First Page = %d\n", firstPage);
	printf("First Page Orginal Value = %d\n", sps2UScreenGetFirstFreeGSPage());
	uint32 clutDst = firstPage;

		//Calculate the number of blocks the clut takes and set
		//the next texture address to it.
		//Cacluation works in this formula.
		//((texture width + block width - 1) / block width) * 
		//	((texture height + block height - 1) / block height)
		//32 bit textures uses 8 for the block height and width.
		//The clut is a 16 by 16 32bit image.
	uint32 textureDst = firstPage + ((16 + 8 - 1) / 8) * ((16 + 8 - 1) / 8);
	
	
	sps2GIFTag_t GIFTag;
	sps2GIFPackedRegister_t Register;

	//Calculate  vram locations for the clut and the texture data
	for (x = 0; x < textureStruct->textureLocationID; x++)
	{
		//256 Color clut calculation
		//8bit textures use 16 for the block height and width
		clutDst = textureDst + ((256 + 16 - 1) / 16) * ((256 + 16 - 1) / 16);
		textureDst = clutDst + ((16 + 8 - 1) / 8) * ((16 + 8 -1) / 8);
	}

	

	printf("clutDst = %d\n", clutDst);
	printf("textureDst = %d\n", textureDst);

	printf("clutDst << 5= %d\n", clutDst);
	printf("textureDst << 5 = %d\n", textureDst);


	GIFTag.i128 = 0;
	Register.i128 = 0;

	bitmapFile = new Bitmap_Loader(fileName);

	//Check to make sure the file is 256x256 by 8 bit.
	if (bitmapFile->loaded == false ||
		bitmapFile->bpp != 8 ||
		bitmapFile->width != 256 ||
		bitmapFile->height != 256)
	{
		printf("Failed to load BMP: Check header ");
		delete bitmapFile;
		return -1;
	}

	
	for (x = 0 ;x < 64; x++)
	{
		if (textureSlotOpen[x] == true)
		{
			textureStruct->textureID = x;
			textureSlotOpen[x] = false;
			break;
		}
	}


	if (x == 64)
	{
		printf("Failed to find open slot ");
		delete bitmapFile;
		return -1;	
	}

	textureStruct->isInMemory = false;
	
	textureStruct->texturePacket = new Dma_Packet(1,CHANNEL1, 0, DmaMemoryEngine);
	textureStruct->texturePacket->usePath2();
	//textureStruct->texturePacket->reset();

	//CLUT DATA

	//Add gif tag for the clut data regs
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 4;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);


	//Add BITBLTBUF info
	Register.i128 = 0;
	Register.A_D.s.DATA = BITBLTBUF_SET(0, 0, 0, clutDst, 1, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_BITBLTBUF;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TRXPOS_SET(0, 0, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXPOS;
	textureStruct->texturePacket->add((uint128)Register.i128);


	//Add reg for texture height/width. 
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXREG_SET(16, 16);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXREG;
	textureStruct->texturePacket->add((uint128)Register.i128);


	//Add reg for direction to send data, IE ram to vram
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXDIR_SET(0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXDIR;
	textureStruct->texturePacket->add((uint128)Register.i128);

	//Setup transfer tag needed to send the clut data to the vram
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = (256 * 4) >> 4; // CLUT size in QWORDs
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG  = GIF_FLG_IMAGE;
	GIFTag.s.NREG = 0;
	GIFTag.s.REGS = 0;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);

	//Add actual clut data.
	//Copy the pallette out of the bitmap file and into memory to be converted.
	memcpy(&clutData, bitmapFile->colors, sizeof(int) * 256);

	// If you look at page 32 of GSUSER_E.pdf you will see
	// a table called "Arrangement of CLUT in IDTEX8" if you
	// look closely you will see that the CLUT is not laid out
	// contiguously. Therefore we must "swizzle" the position so that
	// the clut is placed into memory correctly.
	// (basically what this does is swap bits 3 & 4, see the tutorial
	// doc for more info)
	for (x = 0; x < 256; x++)
	{
		pPtr = (unsigned char *)&clutData[x];
		
		//Swap the red and blue, the bmp pallete is set to BGR
		red = pPtr[2];
		pPtr[2] = pPtr[0];
		pPtr[0] = red;
		//Set the alpha channel to full
		pPtr[3] = 128;
		((uint32 *) swizzledClutData)[(x & 0xE7) + ((x & 0x08) << 1) + ((x & 0x10) >> 1)] = clutData[x];

	}

	textureStruct->texturePacket->add((uint32 *)&swizzledClutData, 256);


	//TEXTURE DATA

	//Add gif tag for the texture data regs
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 4;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);


	//Add BITBLTBUF info
	Register.i128 = 0;
	Register.A_D.s.DATA = BITBLTBUF_SET(0, 0, 0, textureDst, 4, PSM_PSMT8);
	Register.A_D.s.ADDR = GIF_A_D_REG_BITBLTBUF;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TRXPOS_SET(0, 0, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXPOS;
	textureStruct->texturePacket->add((uint128)Register.i128);


	//Add reg for texture height/width. 
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXREG_SET(256, 256);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXREG;
	textureStruct->texturePacket->add((uint128)Register.i128);



	//Add reg for send direction. IE ram to vram
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXDIR_SET(0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXDIR;
	textureStruct->texturePacket->add((uint128)Register.i128);


	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = (256 * 256 * 1) >> 4;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_IMAGE;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);

	
	//Invert the image, stupid, STUPID micro$oft and being ass backwards
	for (x = 0; x < 256; x++)
	{
		memcpy(&imageData[x * 256], &bitmapFile->data[256 * 255 - (256 * x)], 256);
	}
	

	//Add the image data to the packet
	textureStruct->texturePacket->add((uint128 *)&imageData, (256 * 256 * 1) >> 4);


	
	//Gif tag for setting the actual use registers
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 5;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 1;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = 0;
	Register.A_D.s.ADDR = GIF_A_D_REG_TEXFLUSH;
	textureStruct->texturePacket->add((uint128)Register.i128);

	
	Register.i128 = 0;
	Register.A_D.s.DATA = ALPHA_SET(0, 1, 0, 1, 128);
	Register.A_D.s.ADDR = GIF_A_D_REG_ALPHA_1;
	textureStruct->texturePacket->add((uint128)Register.i128);

	Register.i128 = 0;
	Register.A_D.s.DATA = CLAMP_SET(0, 0, 0, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_CLAMP_1;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TEX1_SET(1, 0, 1, 1, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TEX1_1;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TEX0_SET(textureDst, 4, PSM_PSMT8, 8, 8, 1, 0, clutDst, 0, 0, 0, 1);
	Register.A_D.s.ADDR = GIF_A_D_REG_TEX0_1;
	textureStruct->texturePacket->add((uint128)Register.i128);


	textureStruct->texturePacket->finish();

	delete bitmapFile;
	return 1;
}

int Texture_Engine::loadCIF(char *fileName, Texture *textureStruct)
{
	Custom_Image_Loader *cifFile = NULL;
	int x = 0;
	
	//Convert the PAGE to BLOCKS. IE there are 32 blocks in 1 page. So multiply by 32. (<< 5)
	uint32 firstPage = sps2UScreenGetFirstFreeGSPage() << 5;
	printf("First Page = %d\n", firstPage);
	printf("First Page Orginal Value = %d\n", sps2UScreenGetFirstFreeGSPage());
	uint32 clutDst = firstPage;

		//Calculate the number of blocks the clut takes and set
		//the next texture address to it.
		//Cacluation works in this formula.
		//((texture width + block width - 1) / block width) * 
		//	((texture height + block height - 1) / block height)
		//32 bit textures uses 8 for the block height and width.
		//The clut is a 16 by 16 32bit image.
	uint32 textureDst = firstPage + ((16 + 8 - 1) / 8) * ((16 + 8 - 1) / 8);
	
	
	sps2GIFTag_t GIFTag;
	sps2GIFPackedRegister_t Register;

	//Calculate  vram locations for the clut and the texture data
	for (x = 0; x < textureStruct->textureLocationID; x++)
	{
		//256 Color clut calculation
		//8bit textures use 16 for the block height and width
		clutDst = textureDst + ((256 + 16 - 1) / 16) * ((256 + 16 - 1) / 16);
		textureDst = clutDst + ((16 + 8 - 1) / 8) * ((16 + 8 -1) / 8);
	}

	

	printf("clutDst = %d\n", clutDst);
	printf("textureDst = %d\n", textureDst);

	printf("clutDst << 5= %d\n", clutDst);
	printf("textureDst << 5 = %d\n", textureDst);


	GIFTag.i128 = 0;
	Register.i128 = 0;

	cifFile = new Custom_Image_Loader(fileName);

	if (cifFile->imageData == NULL)
	{
		printf("Failed to load CIF: No data\n");
		delete cifFile;
		return -1;
	}


	//Check to make sure the file is 256x256 by 8 bit.
	if (cifFile->imageData->header->bpp != 8 || 
		cifFile->imageData->header->width != 256 ||
		cifFile->imageData->header->height != 256)
	{
		printf("Failed to load CIF: Check header \n");
		printf("%d %d %d\n", cifFile->imageData->header->bpp,
			cifFile->imageData->header->width, cifFile->imageData->header->height);
		delete cifFile;
		return -1;
	}
		printf("%d %d %d\n", cifFile->imageData->header->bpp,
			cifFile->imageData->header->width, cifFile->imageData->header->height);



	
	for (x = 0 ;x < 64; x++)
	{
		if (textureSlotOpen[x] == true)
		{
			textureStruct->textureID = x;
			textureSlotOpen[x] = false;
			break;
		}
	}


	if (x == 64)
	{
		printf("Failed to find open slot ");
		delete cifFile;
		return -1;	
	}

	textureStruct->isInMemory = false;
	
	textureStruct->texturePacket = new Dma_Packet(1,CHANNEL1, 0, DmaMemoryEngine);
	textureStruct->texturePacket->usePath2();
	//textureStruct->texturePacket->reset();

	//CLUT DATA

	//Add gif tag for the clut data regs
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 4;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);


	//Add BITBLTBUF info
	Register.i128 = 0;
	Register.A_D.s.DATA = BITBLTBUF_SET(0, 0, 0, clutDst, 1, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_BITBLTBUF;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TRXPOS_SET(0, 0, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXPOS;
	textureStruct->texturePacket->add((uint128)Register.i128);


	//Add reg for texture height/width. 
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXREG_SET(16, 16);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXREG;
	textureStruct->texturePacket->add((uint128)Register.i128);


	//Add reg for direction to send data, IE ram to vram
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXDIR_SET(0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXDIR;
	textureStruct->texturePacket->add((uint128)Register.i128);

	//Setup transfer tag needed to send the clut data to the vram
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = (256 * 4) >> 4; // CLUT size in QWORDs
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG  = GIF_FLG_IMAGE;
	GIFTag.s.NREG = 0;
	GIFTag.s.REGS = 0;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);

	//Add actual clut data.

	textureStruct->texturePacket->add((uint128 *)cifFile->imageData->paletteData, (256 * 4) >> 4);


	//TEXTURE DATA

	//Add gif tag for the texture data regs
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 4;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);


	//Add BITBLTBUF info
	Register.i128 = 0;
	Register.A_D.s.DATA = BITBLTBUF_SET(0, 0, 0, textureDst, 4, PSM_PSMT8);
	Register.A_D.s.ADDR = GIF_A_D_REG_BITBLTBUF;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TRXPOS_SET(0, 0, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXPOS;
	textureStruct->texturePacket->add((uint128)Register.i128);


	//Add reg for texture height/width. 
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXREG_SET(256, 256);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXREG;
	textureStruct->texturePacket->add((uint128)Register.i128);



	//Add reg for send direction. IE ram to vram
	Register.i128 = 0;
	Register.A_D.s.DATA = TRXDIR_SET(0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TRXDIR;
	textureStruct->texturePacket->add((uint128)Register.i128);


	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = (256 * 256 * 1) >> 4;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 0;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_IMAGE;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);

	
	//Add the image data to the packet
	textureStruct->texturePacket->add((uint128 *)cifFile->imageData->imageData, (256 * 256 * 1) >> 4);


	
	//Gif tag for setting the actual use registers
	GIFTag.i128 = 0;
	GIFTag.s.NLOOP = 5;
	GIFTag.s.PRE = 0;
	GIFTag.s.EOP = 1;
	GIFTag.s.PRIM = 0;
	GIFTag.s.FLG = GIF_FLG_PACKED;
	GIFTag.s.NREG = 1;
	GIFTag.s.REGS = GIF_REG_A_D;
	textureStruct->texturePacket->add((uint128)GIFTag.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = 0;
	Register.A_D.s.ADDR = GIF_A_D_REG_TEXFLUSH;
	textureStruct->texturePacket->add((uint128)Register.i128);

	
	Register.i128 = 0;
	Register.A_D.s.DATA = ALPHA_SET(0, 1, 0, 1, 128);
	Register.A_D.s.ADDR = GIF_A_D_REG_ALPHA_1;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = CLAMP_SET(0, 0, 0, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_CLAMP_1;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TEX1_SET(1, 0, 1, 1, 0, 0, 0);
	Register.A_D.s.ADDR = GIF_A_D_REG_TEX1_1;
	textureStruct->texturePacket->add((uint128)Register.i128);


	Register.i128 = 0;
	Register.A_D.s.DATA = TEX0_SET(textureDst, 4, PSM_PSMT8, 8, 8, 1, 0, clutDst, 0, 0, 0, 1);
	Register.A_D.s.ADDR = GIF_A_D_REG_TEX0_1;
	textureStruct->texturePacket->add((uint128)Register.i128);


	textureStruct->texturePacket->finish();

	delete cifFile;
	return 1;
}

int Texture_Engine::deleteTexture(int textureID)
{

	Texture *workTexture = head;

	while (workTexture != NULL)
	{
		if (workTexture->textureID == textureID)
		{
			break;
		}
		workTexture = workTexture->next;
	}

	if (workTexture == NULL)return 0;

	//Open up the texture  slot again
	textureSlotOpen[textureID] = true;

	//Delete the actual texture
	delete workTexture->texturePacket;
	delete workTexture;

	return 1;
}

int Texture_Engine::setLocationID(int texID, int texLocationID)
{
	return 1;
}


Dma_Packet *Texture_Engine::getPacket(int textureID)
{

	Texture *workTexture = head;

	while (workTexture != NULL)
	{
		if (workTexture->textureID == textureID)
		{
			break;
		}
		workTexture = workTexture->next;
	}

	if (workTexture == NULL)
		return NULL;

	return workTexture->texturePacket;
}

Dma_Packet *Texture_Engine::getTransferPacket(int loctionID)
{

	if (loctionID < 0 || loctionID > 11)return NULL;

	
	return NULL;//&transferPackets[loctionID];
}

Texture *Texture_Engine::getTexture(int textureID)
{
	Texture *workTexture = head;

	while (workTexture != NULL)
	{
		if (workTexture->textureID == textureID)
		{
			break;
		}
		workTexture = workTexture->next;
	}

	if (workTexture == NULL)
		return NULL;
	
	return workTexture;
}

//-1 if not in main memory, 0 if not in vram, 
//1 if not currenly selected and 2 if is currently selected
int Texture_Engine::checkCurrentTexture(int textureID)
{

	Texture *workTexture = getTexture(textureID);

	if (workTexture == NULL)
		return -1;

	if (workTexture->isInMemory == false)
		return 0;

	if (workTexture->textureLocationID == currentLocationID)
		return 2;

	return 1;
}


/*
int Texture_Engine::changeCurrentLocation(int textureID)
{
	currentLocationID = getTexture(textureID)->textureLocationID;
	return 1;
}

  */