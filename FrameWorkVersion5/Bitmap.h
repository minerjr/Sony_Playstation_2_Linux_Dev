/****************************************************************************\
|* Bitmap loading routines
|* ======================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: Minerjr
|* First created			: Fri 
|* Modification history		: See below
|*
\****************************************************************************/

#ifndef __bitmap__loader__header__
#define __bitmap__loader__header__

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>

#include <ps2gs.h>

#include "constants.h"
#include "log.h"
#include "framework.h"


const short BITMAP_MAGIC_NUMBER = 19778;
const int RGB_BYTE_SIZE = 3;

typedef struct tagRGBQuad
{
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
} RGBQuad;

typedef struct tagBitmapFileHeader
{
	unsigned short bfType;
	unsigned int bgSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} BitmapFileHeader;

typedef struct tagBitmapInfoHeader
{
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} BitmapInfoHeader;

class Bitmap
{
public:
	RGBQuad *colors;
	char *data;
	bool loaded;
	int width, height;
	unsigned short bpp;
	int numColors;
	Bitmap();
	Bitmap(char *name);
	bool loadBMP(char *);
	~Bitmap();
private:
	BitmapFileHeader bmfh;
	BitmapInfoHeader bmih;
	int byteWidth;
	int padWidth;
	unsigned int dataSize;
	
	void reset();
	bool convert24(char *name);
	bool convert8(char *name);

};

#endif // ! __bitmap__loader__header__
