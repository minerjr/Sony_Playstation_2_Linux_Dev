#include "Bitmap.h"

Bitmap::Bitmap()
{
	reset();
}

Bitmap::Bitmap(char *name)
{
	reset();
	loadBMP(name);
}

Bitmap::~Bitmap()
{
	_log << "BITMAP Delete colors...";
	if (colors != 0)delete[] colors;
	_log << "done\n";
	_log << "BITMAP Delete data...";
	if (data != 0)delete[] data;
	_log << "done\n";
}

bool Bitmap::loadBMP(char *name)
{
	FILE *in;
	char *tempData;
	//int numColors;

	loaded = false;
	
	if (colors != 0)delete[] colors;
	if (data != 0)delete[] data;

	in = fopen(name, "rb");

	if (in == NULL)
	{
		_log << "BITMAP: Failed to open file " << name << "\n";
		fclose(in);
		return false;
		
	}

	//fread(&bmfh, sizeof(BitmapFileHeader), 1, in);
	fread(&bmfh, 14, 1, in);
	_log << "Size of BitmapFileHeader = " << (int)sizeof(BitmapFileHeader) << "\n";
	if (bmfh.bfType != BITMAP_MAGIC_NUMBER)
	{
		_log << "BITMAP: Magic Number failed \n";

		fclose(in);
		return false;
	}

	fread(&bmih, sizeof(BitmapInfoHeader), 1, in);
	_log << "Size of BitmapInfoHeader = " << (int)sizeof(BitmapInfoHeader) << "\n";
	
	width = bmih.biWidth;
	height = bmih.biHeight;
	bpp = bmih.biBitCount;

	_log << "biBitCount		=" << bmih.biBitCount << "\n";
	_log << "biClrUsed		=" << (int)bmih.biClrUsed << "\n";
	_log << "biSize			=" << (int)bmih.biSize << "\n";
	_log << "biHeight		=" << bmih.biHeight << "\n";
	_log << "biWidth		=" << bmih.biWidth << "\n";
	

	dataSize = (width*height*(bmih.biBitCount/8.0));
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
		_log << "BITMAP: Bpp less then 4: bpp = " << bpp << "\n";
		
		fclose(in);
		return false;
	}
	
	if (bpp == 8 || bpp == 4)
	{
		//colors = new RGBQuad[numColors];
		colors = (RGBQuad *)memalign(64, sizeof(RGBQuad) * numColors); 
		_log << "Number of colors = " << (int)numColors << "\n";
		fread(colors, sizeof(RGBQuad),numColors, in);
	}

	//tempData = new char[dataSize];
	tempData = (char *)memalign(16, sizeof(char) * dataSize);
	if (tempData == NULL)
	{
		_log << "BITMAP: not enough memory for tempData\n";

		fclose(in);
		return false;
	}
	_log << "Data size = " << (int)dataSize << "\n";
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

void Bitmap::reset(void)
{
	loaded = false;
	colors = 0;
	data = 0;
}

bool Bitmap::convert24(char *tempData)
{
	int offset, diff;
	
	diff = width * height * RGB_BYTE_SIZE;

	//data = new char[diff];
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

bool Bitmap::convert8(char *tempData)
{
	_log << "BITMAP: Convert8 \n";
	int offset, diff;

	diff = width * height;

	//data = new char[diff];
	data = (char *)memalign(16, sizeof(char) * diff);

	if (data == NULL)
	{
		_log << "BITMAP: Not enough memory for data\n";

		delete[] data;
		return false;
	}
	memcpy(data, tempData, diff);
	/*
	if (height > 0)
	{
		offset = padWidth - byteWidth;
		int j = 0;

		for (int i = 0; i < dataSize; i += 1)
		{
			if ((i + 1) % padWidth == 0)
			{
				i += offset;
			}
			
			*(data + i) = *(tempData + j);
			j++;
		}
	}

	else 
	{
		offset = padWidth - byteWidth;
		int j = dataSize - 1;

		for (int i = 0; i < dataSize; i++)
		{
			if ((i + 1) % width == 0)
			{
				i += offset;
			}

			*(data + i) = *(tempData + j);
		}
	}
	*/

	return true;
}


bool Bitmap::convert4(char *tempData)
{
	_log << "BITMAP: Convert4 \n";
	int offset, diff;

	diff = (int)(width * height / 2);

	//data = new char[diff];
	data = (char *)memalign(16, sizeof(char) * diff);
	if (data == NULL)
	{
		_log << "BITMAP: Not enough memory for data\n";

		delete[] data;
		return false;
	}
	_log << "Convert 4: diff size = " << diff << "\n";
	//memset(data, 0, diff);
	memcpy(data, tempData, diff);

	/*
	if (height > 0)
	{
		offset = padWidth - byteWidth;
		int j = 0;

		for (int i = 0; i < dataSize; i += 1)
		{
			if ((i + 1) % padWidth == 0)
			{
				i += offset;
			}
			
			*(data + i) = *(tempData + j);
			j++;
		}
	}

	else 
	{
		offset = padWidth - byteWidth;
		int j = dataSize - 1;

		for (int i = 0; i < dataSize; i++)
		{
			if ((i + 1) % width == 0)
			{
				i += offset;
			}

			*(data + i) = *(tempData + j);
		}
	}
	*/	

	return true;
}

