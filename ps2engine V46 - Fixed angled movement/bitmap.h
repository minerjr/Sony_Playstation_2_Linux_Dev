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

#ifndef BITMAP_H
#define BITMAP_H

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>


const short BITMAP_MAGIC_NUMBER = 19778;//Needed to check to see if bitmap is ok
const int RGB_BYTE_SIZE = 3;//Number of bytes for a RGB pixel

//Bitmaps pixel format used for storing the pallet.
typedef struct tagRGBQuad
{
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
} RGBQuad;

//The file header of the bitmap
typedef struct tagBitmapFileHeader
{
	unsigned short bfType;
	unsigned int bgSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} BitmapFileHeader;

//This holds the information about the bitmap.
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

class Bitmap_Loader
{
public:
	RGBQuad *colors;//Pallet if bpp <= 8
	char *data;//The actual image data.
	bool loaded;//Flag to tell if the bitmap loaded or not.
	int width, height;//The height and width of the bitmap.
	unsigned short bpp;//The number of bits per pixel, 4,8,24
	int numColors;//Number of colors in the pallet, 4bit = 16, 8bit = 256
	Bitmap_Loader(char *fileName);//Constructor that loads a file.
	bool loadBMP(char *fileName);//Actuall bitmap loader function.
	~Bitmap_Loader();
private:
	BitmapFileHeader bmfh;//The bitmap file header. DUH :)
	BitmapInfoHeader bmih;//The bitmap info header. DUH :)
	int byteWidth;// The width of the image in bytes.
	int padWidth;//The padding width, needed if you don't use a 2x texture.
	unsigned int dataSize;//The number of bytes for the image data.
	
	void reset();//Resets the bitmap file.
	//Bitmap's are actually stored upside down ;)
	bool convert24(char *tempData);//Inverts image and loads it into memory
	bool convert8(char *tempData);//Inverts image and loads it into memory
	bool convert4(char *tempData);//Inverts image and loads it into memory

};

#endif // ! __bitmap__loader__header__
