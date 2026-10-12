#include "customimage.h"
#include <stdio.h>
#include <stdlib.h>

Custom_Image_Loader::Custom_Image_Loader(char *fileName)
{
	if (loadCustomImage(fileName) == 0)imageData = NULL;
}

Custom_Image_Loader::~Custom_Image_Loader()
{
	if (imageData->header)
		delete imageData->header;
	if (imageData->paletteData)
		delete imageData->paletteData;
	if (imageData->imageData)
		delete imageData->imageData;
	if (imageData)
		delete imageData;
}

int Custom_Image_Loader::loadCustomImage(char *fileName)
{
	FILE *inputFile;

	inputFile = fopen(fileName, "rb");
	if (inputFile == NULL)return 0;

	imageData = new Custom_Image_Data;
	if (imageData == NULL)return 0;

	//Allocate memory for the header
	imageData->header = new Custom_Image_Header;
	//Read in the header
	fread(imageData->header,18, 1, inputFile);

	//Check to see if the file is a valid ps2 custom image format
	if (strcmp(imageData->header->name, "PS2CIF") != 0)
	{
		delete imageData->header;
		delete imageData;
		fclose(inputFile);
		return 0;
	}
	
	if (imageData->header->hasPalette == 1)
	{
		if (imageData->header->bpp == 8)
		{
			imageData->paletteData = new unsigned int[256];
			fread(imageData->paletteData, sizeof(unsigned int), 256, inputFile);
		}
		else if (imageData->header->bpp == 4)
		{
			imageData->paletteData = new unsigned int[16];
			fread(imageData->paletteData, sizeof(unsigned int), 16, inputFile);
		}
	}

	int size = imageData->header->width * imageData->header->height * imageData->header->bpp / 8;
	printf("Size = %d\n", size);
	imageData->imageData = new unsigned char[size];
	fread(imageData->imageData, sizeof(unsigned char), size, inputFile);

	fclose(inputFile);
	return 1;
} 