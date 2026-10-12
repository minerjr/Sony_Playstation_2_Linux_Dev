#ifndef __FONT_H__
#define __FONT_H__

#include <stdio.h>
#include <stdarg.h>
#include "ctexture.h"
#include "sps2wrap.h"

class CFont
{
public:
	CFont();
	~CFont();
	
	CFont(CSPS2Manager * pManager);
	
	int FontStitch(void * pMem);

	bool Initialise(const char * strFontBMP, const char * strFontWidths, 
					uint32 GSStartTile, bool bDoubleHeightForTV = false);

	// Call this before you draw any fonts (don't draw anything else inbetween or you will have to call this again)
	void Begin();

	// Different rendering functions
	void Render(const int x, const int y, const char * strText, 
				uint8 R = 0x80, uint8 G = 0x80, uint8 B = 0x80, uint8 A = 0x80);
	void RenderCenter(const int x, const int y, const char * strText, 
				uint8 R = 0x80, uint8 G = 0x80, uint8 B = 0x80, uint8 A = 0x80);

	void printf(const int x, const int y, uint8 R, uint8 G, uint8 B, uint8 A, const char * strText, ...);
	void printfc(const int x, const int y, uint8 R, uint8 G, uint8 B, uint8 A, const char * strText, ...);

	// Get the number of pixels a string will take up
	int GetStringWidth(const char * strText);

protected:
	CTexture * m_pFontTex;
	CSPS2Manager * m_pManager;

	unsigned char * m_pFontWidths;
	bool m_bDoubleHeight;

	void * m_pMemory;
	void * m_pDMAMem;
	uint32 m_GSStartTile;
	
	// Stitching variables
	int m_FontQWC;
	sps2DMATag_t * m_pFontDMATag;
};

#endif