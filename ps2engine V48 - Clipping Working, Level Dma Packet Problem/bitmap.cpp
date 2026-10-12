#include "bitmap.h"

Bitmap_Loader::Bitmap_Loader(char *fileName)
{
	reset();
	loadBMP(fileName);
}

Bitmap_Loader::~Bitmap_Loader()
{

	if (colors != 0)delete[] colors;

	if (data != 0)delete[] data;

}

bool Bitmap_Loader::loadBMP(char *fileName)
{
	FILE *in;
	char *tempData;


	loaded = false;
	
	if (colors != 0)delete[] colors;
	if (data != 0)delete[] data;

	in = fopen(fileName, "rb");

	if (in == NULL)
	{
		fclose(in);
		return false;
		
	}

	//fread(&bmfh, sizeof(BitmapFileHeader), 1, in);
	fread(&bmfh, 14, 1, in);
	
	if (bmfh.bfType != BITMAP_MAGIC_NUMBER)
	{
		

		fclose(in);
		return false;
	}

	fread(&bmih, sizeof(BitmapInfoHeader), 1, in);
	
	
	width = bmih.biWidth;
	height = bmih.biHeight;
	bpp = bmih.biBitCount;

	
	dataSize = (unsigned int)(width*height*(bmih.biBitCount/8.0));
	if (bmih.biClrUsed > 0)
	{
		numColors = bmih.biClrUsed;
	}
	else
	{
		numColors = 1<<bmih.biBitCount;
	}

	if (bpp < 4)
	{
	
		
		fclose(in);
		return false;
	}
	
	if (bpp == 8 || bpp == 4)
	{
	
		colors = (RGBQuad *)memalign(64, sizeof(RGBQuad) * numColors); 
	
		fread(colors, sizeof(RGBQuad),numColors, in);
	}

	
	tempData = (char *)memalign(16, sizeof(char) * dataSize);
	if (tempData == NULL)
	{
	

		fclose(in);
		return false;
	}
	
	fread( tempData, sizeof(char), dataSize, in);

	fclose(in);

	byteWidth = padWidth = (int)((float)width * (float)bpp/8.0);

	while(padWidth%4 != 0)
	{
		padWidth++;
	}

	if (bpp == 8)
	{
		loaded = convert8(tempData);
	}
	else if (bpp == 4)
	{
		loaded = convert4(tempData);
	}
	else if (bpp == 24)
	{
		loaded = convert24(tempData);
	}

	delete[] tempData;

	return loaded;
}

void Bitmap_Loader::reset(void)
{
	loaded = false;
	colors = 0;
	data = 0;
}

bool Bitmap_Loader::convert24(char *tempData)
{
	int offset, diff;
	
	diff = width * height * RGB_BYTE_SIZE;

	
	data = (char *)memalign(16, sizeof(char) * diff);
	if (data == NULL)
	{
		delete[] data;
		return false;
	}
	
	if (height > 0)
	{
		offset = padWidth - byteWidth;
		
		for (int i = 0; i < dataSize; i+= 3)
		{
			if ((i + 1) % padWidth == 0)
			{
				i += offset;
			}

			*(data + i + 2)= *(tempData + i);
			*(data + i + 1)= *(tempData + i + 1);
			*(data + i) = *(tempData + i + 2);
		}
	}
	else
	{
		offset = padWidth-byteWidth;

		int j = dataSize - 3;

		for (int i = 0; i < dataSize;i+=3)
		{
			if ((i + 1) % padWidth == 0)
			{
				i += offset;
			}

			*(data + j + 2) = *(tempData + i);
			*(data + j + 1) = *(tempData + i + 1);
			*(data + j) = *(tempData + i + 2);
			j -= 3;
		}
	}

	return true;
}

bool Bitmap_Loader::convert8(char *tempData)
{
	int offset, diff;

	diff = width * height;

	
	data = (char *)memalign(16, sizeof(char) * diff);

	if (data == NULL)
	{
	

		delete[] data;
		return false;
	}
	memcpy(data, tempData, diff);

	return true;
}


bool Bitmap_Loader::convert4(char *tempData)
{
	
	int offset, diff;

	diff = (int)(width * height / 2);

	//data = new char[diff];
	data = (char *)memalign(16, sizeof(char) * diff);
	if (data == NULL)
	{
	

		delete[] data;
		return false;
	}
	
	//memset(data, 0, diff);
	memcpy(data, tempData, diff);


	return true;
}

