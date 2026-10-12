#include "texture.h"

bool LoadBitmap(const char * strFilename, void * pMemory, bool bBlackTransparent)
{
	// Open the file
	FILE * fp = NULL;
	fp = fopen(strFilename, "rb");

	if(fp == NULL)
	{
		printf("Failed to load %s\n", strFilename);
		return false;
	}

	// This struct contains the headers that we will read in.
	BitmapFile bmp;

	// Read in the bitmap header
	fread(&bmp.bitmapfileheader,sizeof(BitmapFileHeader),1,fp);

	// Check that this is a bitmap file and someone isn't trying to read
	// a jpeg or something silly like that.
	if(bmp.bitmapfileheader.bfType != BITMAP_ID)
	{
		printf("%s is not a valid bitmap file\n",strFilename);
		
		fclose(fp);
		return false;
	}

	// Read in the position of the bitmap header, then set the file
	// pointer to it
	fread(&bmp.bitmapinfoheader,sizeof(BitmapInfoHeader),1,fp);
	fseek(fp,(int)(bmp.bitmapfileheader.bfOffBits),SEEK_SET);

	// Assert that the bitmap file is 256 * 256 and in 24 bit colour
	assert(256 == bmp.bitmapinfoheader.biWidth);
	assert(256 == bmp.bitmapinfoheader.biHeight);
	assert(24  == bmp.bitmapinfoheader.biBitCount);

	// The image size in bytes
	int iImageSize;

	// Some image utilities don't set this field, so check
	if(bmp.bitmapinfoheader.biSizeImage!=0)
		iImageSize = bmp.bitmapinfoheader.biSizeImage;
	else
		iImageSize = bmp.bitmapfileheader.bfSize - bmp.bitmapfileheader.bfOffBits;

	// Start on the last line and work our way up the image as
	// bitmaps are stored upside down
	unsigned int uLine = 255;
	int iColumn = 0;
	int i = 0;

	// m_iImageSize / 3 is the number of pixels
	for(i = 0; i < iImageSize / 3; i++, iColumn++)
	{
		// We have finished a whole row move up one line
		// (we know we have finished a row if i is a multiple
		// of 256
		if(i > 0 && i % 256 == 0)
		{
			--uLine;
			iColumn = 0;
		}

		// Read the pixel in
		unsigned int pixel = 0;
		fread(&pixel,1,3,fp);

		// Swap the red and blue channels
		unsigned char * pPtr = (unsigned char *)&pixel;
		unsigned char red = pPtr[2];
		pPtr[2] = pPtr[0];
		pPtr[0] = red;

		// Set the alpha to 0 if the colour is black, 0x80 otherwise
		if(pixel == 0 && bBlackTransparent)
			pPtr[3] = 0;
		else
			pPtr[3] = 0x80;

		// Write the pixel to SPS2 memory
		((unsigned int *)pMemory)[iColumn + (uLine * 256)] = pixel;
	}

	// Close the file
	fclose(fp);

	return true;
}

void Upload(uint32 uGSPage, char * pcTexture, void * pDMAMem)
{
	// Get the dstptr, which is in word addresses / 64
	// GSPages are units of 8192, which will be their byte
	// address, so for the word address divide this by 4.
	// Which gives us the word address of a page = page * 2048
	// Then divide this by 64 to get 2048 / 64 = 32.
	// A faster way of multiplying by 32 is << 5.
	uint32 iDstPtr = (uGSPage << 5);

	// Add a DMA cnt tag to transfer the next 6 QWORDs of data
	sps2DMATag_t DMACnt;
	DMACnt.i64 = 0;
	DMACnt.s.QWC = 6;
	DMACnt.s.ID = DMA_ID_SOURCE_CNT;

	// Add the tag at the start
	uint64 * pDMATag = (uint64 *)pDMAMem;
	*pDMATag = DMACnt.i64;
	pDMATag += 2;	// Remember that DMA tags take up 128 bits
					// even though only 64 bits are used

	// QW1: Add a GIFTag to set four GS registers in A+D mode
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pDMATag;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 4; // We want to set 4 registers
	pGIFTag->s.PRE = 0;	// No PRIM field
	pGIFTag->s.EOP = 0; // Still more to come
	pGIFTag->s.PRIM = 0;
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 1;
	pGIFTag->s.REGS = GIF_REG_A_D;

	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	// QW2: Set the destination (second) half of BITBLTBUF. The 4 specifies a (64*4 = 256)
	// pixel wide texture buffer. iDstPtr is the address for the texture to be placed
	// and the final param is 0 = PSMCT32 (i.e. 32 bit texture depth)
	AddA_D(pRegister, BITBLTBUF_SET(0, 0, 0, iDstPtr, 4, PSMCT32),	GS_BITBLTBUF);
	// QW3: We want to transfer the whole texture, so set all of TRXPOS_SET to 0.
	AddA_D(pRegister, TRXPOS_SET(0, 0, 0, 0, 0),				GS_TRXPOS);
	// QW4: Tell the GS the width and height of the texture
	AddA_D(pRegister, TRXREG_SET(256, 256),						GS_TRXREG);
	// QW5: We are transferring in direction 0: Host to Local (EE -> GSMem)
	AddA_D(pRegister, TRXDIR_SET(0),							GS_TRXDIR);

	// The total quadword count for the texture image data
	uint32 iQWC = (256 * 256 * 4) >> 4;

	// QW6: A GIFTag to transfer the image data
	// All fields apart from FLG and NLOOP have no meaning
	pGIFTag = (sps2GIFTag_t *)pRegister;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = iQWC; 		// Tell the GS to expect the whole image
	pGIFTag->s.FLG = GIF_FLG_IMAGE; // We want IMAGE mode


	// Note: This is where the CNT tag we added ends its transfer, so
	// we need to add another DMATag. Notice that you can split a GS primitive
	// up over multiple DMA packets.

	// We don't want to copy the whole image into this DMA chain, so we will
	// use a bunch of REF tags to point the DMAC to the image data
	pDMATag = (uint64 *)(pGIFTag + 1);
	sps2DMATag_t DMARef;

	DMARef.i64 = 0;
	DMARef.s.QWC = KILOBYTES(4) >> 4;	// Transfer 4K at a time
	DMARef.s.ID = DMA_ID_SOURCE_REF;

	// Remember that SPS2 memory is only contiguous in 4K pages. Therefore
	// we must add a REF tag for each 4K page.
	int iNum4KPages = (256 * 256 * 4) / KILOBYTES(4);

	for(int i4KPage=0; i4KPage < iNum4KPages; i4KPage++)
	{
		// Get the physical address of each 4K page, and put it into the ref tag.
		DMARef.s.ADDR = sps2GetPhysicalAddress(pcTexture + \
						(i4KPage * KILOBYTES(4)), pMemCached);

		// Add the ref tag to the DMA chain.
		*pDMATag = DMARef.i64;
		pDMATag += 2;
	}
	
	// Now that we have uploaded the image, the DMA chain needs to be ended. We will use
	// this final packet to set a few GS registers that will be used for texturing.
	sps2DMATag_t DMAEnd;

	DMAEnd.i64 = 0;
	DMAEnd.s.QWC = 5;	// 4 GS Registers + the giftag
	DMAEnd.s.ID = DMA_ID_SOURCE_END;

	// Add the DMA tag to the chain
	*pDMATag = DMAEnd.i64;
	pDMATag += 2;

	// QW1: The Giftag for A+D mode
	pGIFTag = (sps2GIFTag_t *)pDMATag;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 4;	// We want to set 4 registers
	pGIFTag->s.PRE = 0;
	pGIFTag->s.EOP = 1;		// This is the end of the GS Packet
	pGIFTag->s.PRIM = 0;
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 1;
	pGIFTag->s.REGS = GIF_REG_A_D;

	pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	// QW2: Flush the texture cache. This is very important, the texture may
	//      not show up without this.
	AddA_D(pRegister, 0,										GS_TEXFLUSH);
	// QW3: Set the alpha blending mode. See P100 of GSUSER_E.pdf for details
	AddA_D(pRegister, ALPHA_SET(0, 1, 0, 1, 128),				GS_ALPHA_1);
	// QW4: Disable mipmapping, and enable bi-linear texture filtering
	AddA_D(pRegister, TEX1_SET(1, 0, 1, 1, 0, 0, 0),			GS_TEX1_1);
	// QW5: The important fields are iDstPtr, which is the address of the
	//      texture is GSMem. 4 is the width of the texture buffer (256 pixels)
	//      the two parameters set to 8 are the width and height of the texture, 
	//		given in 2^X format (2^8 = 256 pixels). The final parameter we care
	//		about is TCC which we have set to one to indicate we want to
	//		use the alpha component of the texture
	AddA_D(pRegister, TEX0_SET(iDstPtr, 4, 0, 8, 8, 1, 0, 0, 0, 0, 0, 0), GS_TEX0_1);

	// Flush the DMA chain to memory before we fire it off
	sps2FlushCache(iSPS2Desc);

	// Set up the transfer registers (path2)
	Dn_CHCR_t chcr;
	Dn_TADR_t tadr;

	// Set the address of the first tag (right at the start of the chain)
	tadr.i32 = 0;
	tadr.s.SPR = 0;
	tadr.s.ADDR = sps2GetPhysicalAddress(pDMAMem, pMemCached);
	*EE_D2_TADR = tadr.i32;

	// Don't transfer any data from MADR
	*EE_D2_QWC = 0;

	// Set the channel control register
	chcr.i32 = 0;
	chcr.s.MOD = CHCR_MOD_CHAIN;
	chcr.s.STR = 1;

	// Fire off the packet!
	*EE_D2_CHCR = chcr.i32;

	// Wait for it to complete
	sps2WaitForDMA(2, iSPS2Desc);
}
