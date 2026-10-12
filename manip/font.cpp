#include <stdio.h>
#include "sps2.h"
#include <assert.h>
#include <memory.h>
#include <sps2util.h>
#include "PS2Defines.h"
#include "ctexture.h"
#include "font.h"

// There are 16 letters on each row of the bitmap
#define LETTERSPERROW 16

//extern sps2Memory_t * pMemCached;
//extern int iSPS2Desc;

// Stitching stuff 
//int g_FontQWC = 0;
//sps2DMATag_t * pFontDMATag;

int CFont::FontStitch(void * pMem)
{
	if((((int32)pMem) & 0xFFF) == 0xFF0)
	{
		m_pFontDMATag->s.QWC = m_FontQWC;
		m_FontQWC = 1;

		m_pFontDMATag = (sps2DMATag_t *)pMem;
		m_pFontDMATag->i64 = 0;
		m_pFontDMATag->s.QWC = 0;
		m_pFontDMATag->s.ID = DMA_ID_SOURCE_NEXT;
		m_pFontDMATag->s.ADDR = m_pManager->GetPhysicalAddress((void *)(((int32)pMem) + 0x10));

		++m_pFontDMATag;
		m_pFontDMATag->i64 = 0;
		m_pFontDMATag->s.QWC = 0;
		m_pFontDMATag->s.ID = DMA_ID_SOURCE_CNT;

		return 2;
	}

	m_FontQWC++;

	return 0;
}

CFont::CFont()
:m_pFontWidths(NULL)
{

}

CFont::CFont(CSPS2Manager * pManager)
{
	m_pFontWidths = NULL;
	m_pManager = pManager;
	m_FontQWC = 0;
}



CFont::~CFont()
{
	if(m_pFontWidths)
		delete m_pFontWidths;
}

// Call this function once at the start of the program to load the texture and the font
// widths into memory.
bool CFont::Initialise(const char * strFontBMP, const char * strFontWidths, uint32 GSStartTile)
{
	m_pDMAMem = m_pManager->GetFontDmaBase();
	
	// Set to m_bDoubleHeight false if using a monitor
	if(m_pManager->GetVideoMode() == SPS2U_SCR_VESA)m_bDoubleHeight = false;
	else m_bDoubleHeight = true;
	
	m_pMemory = m_pManager->GetFontTexBase();
	m_GSStartTile = GSStartTile;

	// Load the texture (this should all be familiar)
	m_pFontTex = new CTexture(m_pMemory , m_pDMAMem, m_pManager);

	// We have added an extra parameter to LoadBitmap which tells
	// the function to load pure white pixels into sps2 memory, but use the 
	// red channel in the texture as the alpha channel
	if(!m_pFontTex->LoadBitmap(strFontBMP, false, true))
		return false;

	// Load the font widths from the file
	FILE * fp = fopen(strFontWidths, "rb");

	if(fp == NULL)
		return false;

	if(m_pFontWidths)
		delete [] m_pFontWidths;

	m_pFontWidths = new unsigned char[256];
	fread(m_pFontWidths, sizeof(unsigned char), 256, fp);
	fclose(fp);

	return true;
}

void CFont::Begin()
{
	// Upload the font texture
	m_pFontTex->Upload();
}

void CFont::Render(const int x, const int y, const char * strText, uint8 R, uint8 G, uint8 B, uint8 A)
{
	// How many sprites we have to render
	size_t textlen = strlen(strText);

	if(textlen == 0)
		return;

	sps2GIFTag_t * pGIFTag;
	sps2GIFPackedRegister_t * pRegister;

	// This code may be quite confusing, but basically all it does is:
	// * Insert a cnt tag,
	// * Add an A+D giftag to set RGBAQ register to the font color
	// * Add giftag to draw the sprites
	// * Loop through all the letters and add one sprite for each letter
	// * Set the qwc of the cnt tag
	// * End the DMA chain with an END tag
	// * Fire!

	// Add DMA cnt tag
	m_pFontDMATag = (sps2DMATag_t *)m_pDMAMem;
	m_pFontDMATag->i64 = 0;
	m_pFontDMATag->s.ID = DMA_ID_SOURCE_CNT;
	m_FontQWC = 0;

	pGIFTag = (sps2GIFTag_t *)(m_pFontDMATag + 1);
	pGIFTag += FontStitch(pGIFTag);

	// Add an A+D mode giftag
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 1;
	pGIFTag->s.PRE = 0;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 0;		// This is the last giftag in the packet
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 1; 
	pGIFTag->s.REGS = GIF_REG_A_D;

	// Set the RGBA colour of the sprites we are about to draw
	pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);
	pRegister += FontStitch(pRegister);
	pRegister->i128 = 0;
	pRegister->A_D.s.ADDR = GIF_A_D_REG_RGBAQ; //GIF_REG_RGBAQ;  //GS_RGBAQ;
	pRegister->A_D.s.DATA = RGBAQ_SET(R,G,B,A,0);

	pGIFTag = (sps2GIFTag_t *)(pRegister + 1);
	pGIFTag += FontStitch(pGIFTag);

	// Add the giftag for the sprites we are about to draw
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = textlen;
	pGIFTag->s.PRE = 1;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;		// This is the last giftag in the packet
	pGIFTag->s.PRIM = GS_PRIM(GS_SPRITE, 0, 1, 0, 1, 0, 1, 0, 0);
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 4; 
	pGIFTag->s.REGS = GIF_REG_UV | (GIF_REG_XYZ2 << 4) | (GIF_REG_UV << 8) |
					  (GIF_REG_XYZ2 << 12);

	pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	uint32 lastx = x;
	uint32 line = 0;

	uint32 uvOffset = 0;
	uint32 lineHeight = 16;

	// Hackish double height mode for PAL / NTSC
	if(m_bDoubleHeight)
	{
		uvOffset = 8;
		lineHeight = 32;
	}

	// Loop through all the letters
	for(uint32 letter = 0; letter < textlen; letter++)
	{
		char cLetter = strText[letter];

		// If the letter is 'Newline' move down a line
		if(cLetter == '\n')
		{
			++line;
			lastx = x;
		}

		// The width of the letter in pixels
		int letterwidth = m_pFontWidths[cLetter];

		// Make the letterwidth even and spaced out a bit more
		letterwidth = (letterwidth % 2) ? letterwidth + 1 : letterwidth;
		
		// Get the texture coordinates of the letter
		int tx = cLetter % LETTERSPERROW;
		int ty = cLetter / LETTERSPERROW;

		// The texture coordinates of the center of the letter
		int cx = (tx * 16) + 8;
	
		// Work out the texture coordinates from the center of the letter
		int u0 = ((cx - ((letterwidth) / 2)) << 4);
		int v0 = ((ty * 16) << 4) + uvOffset;

		int u1 = ((letterwidth) << 4) + u0;
		int v1 = ((16) << 4) + v0 - uvOffset;

		// Work out the positions of the corners of the sprites
		int left	= lastx;
		int right	= lastx + letterwidth;
		int top		= y + (line * lineHeight);
		int bottom	= y + (line * lineHeight) + lineHeight;

		// The first UV coordinate
		pRegister += FontStitch(pRegister);
		pRegister->i128 = 0;
		pRegister->UV.s.U = u0;
		pRegister->UV.s.V = v0;
		++pRegister;

		// The top left sprite vertex
		pRegister += FontStitch(pRegister);
		pRegister->i128 = 0;
		pRegister->XYZ2.s.X = ((left + 2048) << 4) - 8;
		pRegister->XYZ2.s.Y = ((top + 2048) << 4) - 8;
		pRegister->XYZ2.s.Z = 0xFFFFFF;
		++pRegister;

		// Second UV coordinate
		pRegister += FontStitch(pRegister);
		pRegister->i128 = 0;
		pRegister->UV.s.U = u1;
		pRegister->UV.s.V = v1;
		++pRegister;

		// Bottom right vertex
		pRegister += FontStitch(pRegister);
		pRegister->i128 = 0;
		pRegister->XYZ2.s.X = ((right + 2048) << 4) - 8;
		pRegister->XYZ2.s.Y = ((bottom + 2048) << 4) - 8;
		pRegister->XYZ2.s.Z = 0xFFFFFF;
		++pRegister;

		// Move the sprite position forward the size of the letter we just drew
		lastx += letterwidth;
	}

	// Set the QWC of the last DMA tag we added
	m_pFontDMATag->s.QWC = m_FontQWC;

	// End the DMA chain with an END tag
	m_pFontDMATag = (sps2DMATag_t *)pRegister;
	m_pFontDMATag->i64 = 0;
	m_pFontDMATag->s.QWC = 0;
	m_pFontDMATag->s.ID = DMA_ID_SOURCE_END;

	m_pManager->FlushCache();

	// Fire off the packet!
	Dn_CHCR_t chcr;
	Dn_TADR_t tadr;

	asm("sync.l");
	assert(((*EE_D2_CHCR)&0x0100) == 0);

	// DMA Channel 2 is the one we want, it goes to the GS
	// All this should be familiar from program 3-1 as this is simply
	// a normal mode transfer.
	*EE_D2_QWC = 0;	// Amount of data to be transmitted

	tadr.i32 = 0;		// Clear the register to zero
	// Set the address field to the physical address of the allocated mem
	//tadr.s.ADDR = sps2GetPhysicalAddress(m_pDMAMem, pMemCached);
	tadr.s.ADDR = m_pManager->GetPhysicalAddress(m_pDMAMem);
	*EE_D2_TADR = tadr.i32;	// Set the register

	// Finally the channel control register
	chcr.i32 = 0;
	chcr.s.MOD = CHCR_MOD_CHAIN;	// Normal mode transfer
	chcr.s.STR = 1;	// Start the transfer

	*EE_D2_CHCR = chcr.i32;	// Set the register to start the transfer

	asm("sync.l");

	// Wait for it to complete
	m_pManager->WaitForDMA(2);
	//sps2WaitForDMA(2, m_pManager->m_iSPS2Desc);
}

// Uses (x,y) as the center of the string instead of the top left corner
void CFont::RenderCenter(const int x, const int y, const char * strText,
								uint8 R = 0x80, uint8 G = 0x80, uint8 B = 0x80, uint8 A = 0x80)
{
	Render(x - (GetStringWidth(strText) / 2), y - 8, strText, R, G, B, A);
}

// Get the width of the first line of the string in pixels
int CFont::GetStringWidth(const char * strText)
{
	int letterwidth;
	size_t textlen = strlen(strText);
	int size = 0;

	for(uint32 letter = 0; letter < textlen; letter++)
	{
		if(strText[letter] == '\n')
			break;
			
		// The width of the letter in pixels	
		letterwidth = m_pFontWidths[strText[letter]];
		
		// Make the letterwidth even and spaced out a bit more
		letterwidth = (letterwidth % 2) ? letterwidth + 1 : letterwidth;

	
		size += letterwidth;
	}

	return size;
}

// A printf wrapper that uses our Render function.
void CFont::printf(const int x, const int y, uint8 R, uint8 G, uint8 B, uint8 A, const char * strText, ...)
{
	// Unpack the extra arguments (...) using the format specificiation in 
	// strText into strBuffer
	va_list args;
	char strBuffer[4096];

	if (!strText)
		return;

	va_start(args, strText);
	vsprintf(strBuffer, strText, args);
	va_end(args);

	Render(x, y, strBuffer, R, G, B, A);
}

// Same as the above function, but uses RenderCenter instead.
void CFont::printfc(const int x, const int y, uint8 R, uint8 G, uint8 B, uint8 A, const char * strText, ...)
{
	va_list args;
	char strBuffer[4096];

	if (!strText)
		return;

	va_start(args, strText);
	vsprintf(strBuffer, strText, args);
	va_end(args);

	RenderCenter(x, y, strBuffer, R, G, B, A);
}
