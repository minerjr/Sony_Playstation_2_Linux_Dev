#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "sps2.h"
#include <assert.h>
#include "sps2wrap.h"
#include "PS2Defines.h"
#include "AddRegisters.h"

// These are global in main.cpp, extern them here so we can use them
//extern sps2Memory_t * pMemCached;
//extern int iSPS2Desc;

// All the following is the layout of a bitmap file header (the bit before
// the actual image data). The __attribute__((packed)) tells the compiler
// not to insert any annoying padding. (If it was padded we wouldn't be
// able to read it in in one go easily)
#define BITMAP_ID      0x4D42

struct BitmapFileHeader
{
	uint16  bfType;
    uint32  bfSize;
    uint16  bfReserved1;
    uint16  bfReserved2;
    uint32  bfOffBits;
} __attribute__((packed));

struct BitmapInfoHeader
{
	uint32	biSize;
	int32	biWidth;
	int32	biHeight;
	uint16	biPlanes;
	uint16	biBitCount;
	uint32	biCompression;
	uint32	biSizeImage;
	int32	biXPelsPerMeter;
	int32	biYPelsPerMeter;
	uint32	biClrUsed;
	uint32	biClrImportant;
} __attribute__((packed));

struct BitmapFile
{
	BitmapFileHeader  bitmapfileheader;
	BitmapInfoHeader  bitmapinfoheader;
} __attribute__((packed));

class CTexture
{
public:
	CTexture(void * pMemory, void * pDMAMem, CSPS2Manager * M);
	~CTexture();

	bool LoadBitmap(const char * strFilename, bool bBlackTransparent, bool bRedAsAlpha = false);
	void Upload();

private:
	uint32 m_uGSPage;
	sps2Memory_t * m_pMemCached;
	int m_iSPS2Desc;
	void * m_pDMAMem;
	void * m_pMemory;
	int m_iClutOffset;
	int m_iWidth, m_iHeight;
	int m_l2Width, m_l2Height;
	bool m_bHasCLUT;
};

#endif
