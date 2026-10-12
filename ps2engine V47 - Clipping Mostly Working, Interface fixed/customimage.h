#include "structs.h"

#ifndef CUSTOM_IMAGE_H
#define CUSTOM_IMAGE_H

typedef struct Custom_Image_Header_t
{
	char name[8];
	int width;
	int height;
	unsigned char bpp;
	unsigned char hasPalette;

}Custom_Image_Header;

typedef struct Custom_Image_Data_t
{
	Custom_Image_Header *header;
	unsigned char *imageData;
	unsigned int *paletteData;
}Custom_Image_Data;

class Custom_Image_Loader
{
public:
	Custom_Image_Data *imageData;

	Custom_Image_Loader(char *fileName);
	~Custom_Image_Loader();

	int loadCustomImage(char *fileName);
};

#endif