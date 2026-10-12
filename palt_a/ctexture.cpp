#include "ctexture.h"
#include <memory.h>

// Returns the base 2 logarithm of the texture size
// (also a handy way to check for illegal texture sizes)
uint32 TextureLog2(uint32 n)
{
	switch(n)
	{
	case 256:
		return 8;
	case 128:
		return 7;
	case 64:
		return 6;
	case 32:
		return 5;
	default:
		{
			printf("Unsupported texture size\n");
			exit(-1);
		}
	}
}

// pMemory should be enough memory for the texture 
// + the clut (if there is one)
CTexture::CTexture(void * pMemory)
{
	m_pMemory = pMemory;
	m_iClutOffset = 0;
	m_iWidth = 0;
	m_iHeight = 0;
	m_l2Width = 0;
	m_l2Height = 0;
	m_bHasCLUT = false;
}

CTexture::~CTexture()
{
}

// This function will load 24 or 8 bit windows bmp files into
// the SPS2 memory that was given to the class constructor
bool CTexture::LoadBitmap(const char * strFilename, bool bBlackTransparent, bool bRedAsAlpha)
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
	// some weird file in.
	if(bmp.bitmapfileheader.bfType != BITMAP_ID)
	{
		printf("%s is not a valid bitmap file\n",strFilename);
		
		fclose(fp);
		return false;
	}

	// Read in the position of the bitmap header, then set the file
	// pointer to it
	fread(&bmp.bitmapinfoheader,sizeof(BitmapInfoHeader),1,fp);

	m_iWidth = bmp.bitmapinfoheader.biWidth;
	m_iHeight = bmp.bitmapinfoheader.biHeight;

	// Make sure that the user isn't trying to load an unsupported colour
	// depth
	assert(bmp.bitmapinfoheader.biBitCount == 8 ||
		   bmp.bitmapinfoheader.biBitCount == 24);

	// The log2 function has an assert in it that will be fired off if
	// the texture is not a power of 2 or is an unsupported size
	m_l2Width = TextureLog2(m_iWidth);
	m_l2Height = TextureLog2(m_iHeight);

	// The image size in bytes
	int iImageSize;

	// Some image utilities don't set this field, so check first
	if(bmp.bitmapinfoheader.biSizeImage!=0)
		iImageSize = bmp.bitmapinfoheader.biSizeImage;
	else
		iImageSize = bmp.bitmapfileheader.bfSize - bmp.bitmapfileheader.bfOffBits;

	// Next, if we are loading an 8 bit texture we must load
	// a CLUT (Colour look-up table). 
	int clut[256], swizzledclut[256];

	if(bmp.bitmapinfoheader.biBitCount == 8)
	{
		m_bHasCLUT = true;

		fread(&clut, sizeof(int), 256, fp);

		// Loop through each CLUT entry and convert it from
		// BGR to RGB, and "swizzle" it
		for(int p=0; p<256; p++)
		{	
			if(bRedAsAlpha)
			{
				// set pixel to white.
				// set alpha to half the red colour
				uint8 * pPtr = (uint8*)&clut[p];
				pPtr[3]= pPtr[2] / 2;
				pPtr[0] = 0xFF;
				pPtr[1] = 0xFF;
				pPtr[2] = 0xFF;
			}
			else
			{
				unsigned char * pPtr = (unsigned char *)&clut[p];
				unsigned char red = pPtr[2];
				pPtr[2] = pPtr[0];
				pPtr[0] = red;

				// Set the alpha to 0 if the colour is black, 0x80 otherwise
				if(clut[p] == 0 && bBlackTransparent)
					pPtr[3] = 0x00;
				else
					pPtr[3] = 0x80;
			}

			// If you look at page 32 of GSUSER_E.pdf you will see
			// a table called "Arrangement of CLUT in IDTEX8" if you
			// look closely you will see that the CLUT is not laid out
			// contiguously. Therefore we must "swizzle" the position so that
			// the clut is placed into memory correctly.
			// (basically what this does is swap bits 3 & 4, see the tutorial
			// doc for more info)
			((uint32 *) swizzledclut)[(p & 0xE7) + ((p & 0x08) << 1) + ((p & 0x10) >> 1)] = clut[p];
		}
	}

	// Seek the file to the image data
	fseek(fp,(int)(bmp.bitmapfileheader.bfOffBits),SEEK_SET);

	// Start on the last line and work our way up the image as
	// bitmaps are stored upside down
	unsigned int uLine = m_iHeight - 1;
	int iColumn = 0;
	int i = 0;

	for(i = 0; i < (m_iWidth * m_iHeight); i++, iColumn++)
	{
		// We have finished a whole row move up one line
		// (we know we have finished a row if i is a multiple
		// of the image width
		if(i > 0 && i % m_iWidth == 0)
		{
			--uLine;
			iColumn = 0;
		}

		if(bmp.bitmapinfoheader.biBitCount == 8)
		{
			// Read the pixel in
			unsigned char pixel = 0;
			fread(&pixel,1,1,fp);

			// Write the pixel to SPS2 memory
			((unsigned char *)m_pMemory)[iColumn + (uLine * m_iWidth)] = pixel;
		}
		else	// Must be 24 bit colour
		{
			// Read the pixel in
			uint32 pixel = 0;
			fread(&pixel,1,3,fp);

			if(bRedAsAlpha)
			{
				uint8 * pPtr = (uint8*)&pixel;
				pPtr[3]= pPtr[2] / 2;
				pPtr[0] = 0xFF;
				pPtr[1] = 0xFF;
				pPtr[2] = 0xFF;
			}
			else
			{
				// Swap the red and blue channels
				uint8 * pPtr = (uint8*)&pixel;
				uint8 red = pPtr[2];
				pPtr[2] = pPtr[0];
				pPtr[0] = red;

				// Set the alpha to 0 if the colour is black
				if(pixel == 0 && bBlackTransparent)
					pPtr[3] = 0;
				else
					pPtr[3] = 0x80;
			}

			((uint32 *)m_pMemory)[iColumn + (uLine * m_iWidth)] = pixel;
		}
	}

	// If we have loaded a clut, copy it to the next free 4K chunk
	// of SPS2 memory
	if(m_bHasCLUT)
	{
		m_iClutOffset = (iImageSize + 0xFFF) & 0xFFFFF000;
		memcpy(&((char *)m_pMemory)[m_iClutOffset], swizzledclut, 256 * 4);
	}

	// Close the file
	fclose(fp);

	return true;
}


void CTexture::Upload(uint32 uGSPage, void * pDMAMem)
{
	// To keep the code simple we will only allow one texture in
	// VMEM at a time. This makes positioning the textures
	// easy.

	// We will always put the clut into the last GS Page
	uint32 uDstPtrClut = 511 << 5; 
	uint32 uQSizeClut =  (256 * 4) >> 4; // CLUT size in QWORDs
	uint32 iDstPtr = (uGSPage << 5);

	sps2DMATag_t DMACnt;
	sps2DMATag_t DMARef;
	sps2GIFTag_t * pGIFTag;
	sps2GIFPackedRegister_t * pRegister;

	uint64 * pDMATag = (uint64 *)pDMAMem;

	// The CLUT is transferred just like an image. Basically
 	// the clut is just a 16*16 32-bit texture.
	if(m_bHasCLUT)
	{
		DMACnt.i64 = 0;
		DMACnt.s.QWC = 6;
		DMACnt.s.ID = DMA_ID_SOURCE_CNT;

		// Add the tag at the start
		*pDMATag = DMACnt.i64;
		pDMATag += 2;

		pGIFTag = (sps2GIFTag_t *)pDMATag;
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
		pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

		// QW2: Set the destination (second) half of BITBLTBUF.
		AddA_D(pRegister, BITBLTBUF_SET(0, 0, 0, uDstPtrClut, 1, 0),	GIF_A_D_REG_BITBLTBUF);
		// QW3: We want to transfer the whole texture, so set all of TRXPOS_SET to 0.
		AddA_D(pRegister, TRXPOS_SET(0, 0, 0, 0, 0),				GIF_A_D_REG_TRXPOS);
		// QW4: Tell the GS the width and height of the CLUT
		AddA_D(pRegister, TRXREG_SET(16, 16),						GIF_A_D_REG_TRXREG);
		// QW5: We are transferring in direction 0: Host to Local (EE -> GSMem)
		AddA_D(pRegister, TRXDIR_SET(0),							GIF_A_D_REG_TRXDIR);

		// QW6: A GIFTag to transfer the image data
		pGIFTag = (sps2GIFTag_t *)pRegister;
		pGIFTag->i128 = 0;
		pGIFTag->s.NLOOP = uQSizeClut; // Tell the GS to expect the whole image
		pGIFTag->s.PRE  = 0;
		pGIFTag->s.EOP  = 0;
		pGIFTag->s.PRIM = 0;
		pGIFTag->s.FLG  = GIF_FLG_IMAGE;
		pGIFTag->s.NREG = 0;
		pGIFTag->s.REGS = 0;
	
		pDMATag = (uint64 *)(pGIFTag + 1);

		DMARef.i64 = 0;
		DMARef.s.QWC = uQSizeClut;
		DMARef.s.ID = DMA_ID_SOURCE_REF;
		DMARef.s.ADDR = sps2GetPhysicalAddress(&((char *)m_pMemory)[m_iClutOffset], pMemCached);

		*pDMATag = DMARef.i64;
		pDMATag += 2;
	}

	// Add a DMA cnt tag to transfer the next 6 QWORDs of data
	DMACnt.i64 = 0;
	DMACnt.s.QWC = 6;
	DMACnt.s.ID = DMA_ID_SOURCE_CNT;

	*pDMATag = DMACnt.i64;
	pDMATag += 2;	// Remember that DMA tags take up 128 bits
					// even though only 64 bits are used

	// QW1: Add a GIFTag to set four GS registers in A+D mode
	pGIFTag = (sps2GIFTag_t *)pDMATag;
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
	pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	// QW2: Set the destination (second) half of BITBLTBUF. The 4 specifies a (64*4 = 256)
	// pixel wide texture buffer. iDstPtr is the address for the texture to be placed
	// We set the pixel format to either PSMT8 or PSMCT32 depending on the image
	// type we are loading.
	AddA_D(pRegister, BITBLTBUF_SET(0, 0, 0, iDstPtr, 4, m_bHasCLUT ? PSM_PSMT8 : PSM_PSMCT32),	GIF_A_D_REG_BITBLTBUF);
	// QW3: We want to transfer the whole texture, so set all of TRXPOS_SET to 0.
	AddA_D(pRegister, TRXPOS_SET(0, 0, 0, 0, 0),				GIF_A_D_REG_TRXPOS);
	// QW4: Tell the GS the width and height of the texture
	AddA_D(pRegister, TRXREG_SET(m_iWidth, m_iHeight),			GIF_A_D_REG_TRXREG);
	// QW5: We are transferring in direction 0: Host to Local (EE -> GSMem)
	AddA_D(pRegister, TRXDIR_SET(0),							GIF_A_D_REG_TRXDIR);

	// The total quadword count for the texture image data
	uint32 iQWC = (m_iWidth * m_iHeight * (m_bHasCLUT ? 1 : 4)) >> 4;

	// QW6: A GIFTag to transfer the image data
	pGIFTag = (sps2GIFTag_t *)pRegister;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = iQWC; // Tell the GS to expect the whole image
	pGIFTag->s.PRE = 0;
	pGIFTag->s.EOP = 0;
	pGIFTag->s.PRIM = 0;
	pGIFTag->s.FLG = GIF_FLG_IMAGE; // We want IMAGE mode
	pGIFTag->s.NREG = 1;
	pGIFTag->s.REGS = GIF_REG_A_D;

	// Note: This is where the CNT tag we added ends its transfer, so
	// we need to add another DMATag. Notice that you can split a GS primitive
	// up over multiple DMA packets.

	// We don't want to copy the whole image into this DMA chain, so we will
	// use a bunch of REF tags to point the DMAC to the image data
	pDMATag = (uint64 *)(pGIFTag + 1);
	DMARef.i64 = 0;
	DMARef.s.QWC = KILOBYTES(4) >> 4;	// Transfer 4K at a time
	DMARef.s.ID = DMA_ID_SOURCE_REF;

	// Remember that SPS2 memory is only contiguous in 4K pages. Therefore
	// we must add a REF tag for each 4K page.
	int iNum4KPages = (m_iWidth * m_iHeight * (m_bHasCLUT ? 1 : 4)) / KILOBYTES(4);

	for(int i4KPage=0; i4KPage < iNum4KPages; i4KPage++)
	{
		// Get the physical address of each 4K page, and put it into the ref tag.
		DMARef.s.ADDR = sps2GetPhysicalAddress(((char *)m_pMemory) + (i4KPage * KILOBYTES(4)), pMemCached);

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
	AddA_D(pRegister, 0,										GIF_A_D_REG_TEXFLUSH);
	// QW3: Set the alpha blending mode. See P100 of GSUSER_E.pdf for details
	AddA_D(pRegister, ALPHA_SET(0, 1, 0, 1, 128),				GIF_A_D_REG_ALPHA_1);
	// QW4: Disable mipmapping, and enable bi-linear texture filtering
	AddA_D(pRegister, TEX1_SET(1, 0, 1, 1, 0, 0, 0),			GIF_A_D_REG_TEX1_1);
	// QW5: TEX0. This is pretty much the same as last time, except now
	//		we have to put the address of the CLUT into CBP, and CLD to 1
	AddA_D(pRegister, TEX0_SET(iDstPtr, 4, (m_bHasCLUT ? PSM_PSMT8 : PSM_PSMCT32), 
		            m_l2Width, m_l2Height, 1, 0, uDstPtrClut, 0, 0, 0, 1),
																GIF_A_D_REG_TEX0_1);

	// Flush the DMA chain to memory before we fire it off
	sps2FlushCache(iSPS2Desc);

	// Set up the transfer registers (path2)
	Dn_CHCR_t chcr;
	//Dn_MADR_t madr;
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