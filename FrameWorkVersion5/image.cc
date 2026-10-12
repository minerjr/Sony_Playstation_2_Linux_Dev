/****************************************************************************\
|* Image loading routines
|* ======================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG, various others
|* First created			: Sat Jul 20 15:02:39 GMT 2002
|* Modification history		: See class header file
|*
\****************************************************************************/
#include "image.h"


/****************************************************************************\
|* Constructor
\****************************************************************************/
Image::Image()
	{
	init();
	}
	
/****************************************************************************\
|* Constructor
\****************************************************************************/
Image::Image(char *name)
{
	init();
	if (strstr(name, ".pcx") != 0)
	{
		loadpcx(name);
	}
	else if (strstr(name, ".bmp") != 0)
	{
		loadbmp(name);
	}
	else if (strstr(name, "tga") != 0)
	{
		loadtga(name);
	}

}
	
/****************************************************************************\
|* Destructor
\****************************************************************************/
Image::~Image()
	{
	erase();
	}
	
/****************************************************************************\
|* Initialise the image
\****************************************************************************/
void Image::init(void)
	{
	memset(this, 0, sizeof(Image));
	}

/****************************************************************************\
|* Swap bits 3&4 in bytes, works on 32bit value
\****************************************************************************/
unsigned int Image::swapBits3and4(unsigned int value)
	{
	return ( value & 0xe7e7e7e7)   		| 
	       ((value & 0x08080808)<<1) 	| 
		   ((value & 0x10101010)>>1);
	}

/****************************************************************************\
|* Load a TGA format image
\****************************************************************************/
int Image::loadtga(char *filename)
{
	int ttmp;
	int width = 0, height = 0;// The Dimensions of the image.
	byte length = 0;// The length in bytes to the pixels
	byte imageType = 0;//The image type (RLE, RGB, ALPHA...)
	byte bits = 0;// The bits per pixel for the image(16, 24, 32)
	FILE *pFile = NULL;// The file pointer
	int channels = 0;//THe channels of the image( 3 = RGA: 4 = RGBA)
	int stride = 0;//The stride(channels * width)
	int i = 0;//A counter
	_log << "Start loading tga file.\n";
	pFile = fopen(filename, "rb");
	if (pFile == NULL)
	{
		_log << "TGA file file not found: " << filename << "\n";
		return 1;
	}
	
	fread(&length, sizeof(byte), 1, pFile);

	fseek(pFile, 1, SEEK_CUR);
	
	fread(&imageType, sizeof(byte), 1, pFile);

	fseek(pFile, 9, SEEK_CUR);

	fread(&width, sizeof(unsigned short), 1, pFile);

	fread(&height, sizeof(unsigned short), 1, pFile);

	fread(&bits, sizeof(byte), 1, pFile);

	fseek(pFile, length + 1, SEEK_CUR);

	if(imageType != TGA_RLE)
	{
		if (bits == 24 || bits == 32)
		{
			channels = bits / 8;
			stride = channels * width;

			texture.w = width;
			texture.h = height;
			_log << "TGA: width = " << width << " height = " << height << "\n";
			textData= (unsigned char*) memalign(16, texture.w * texture.h * (bits / 8));
			if (texture.w > 64)
				texture.fbw=texture.w / 64;
			else
				texture.fbw=1;

			if (bits == 24)
			{
				texture.psm = PS2_GS_PSMCT24;
			}
			else if (bits == 32)
			{
				texture.psm = PS2_GS_PSMCT32;
			}		

			for (int y = 0; y < height; y++)
			{
				unsigned char *pLine = &(textData[stride * y]);

				fread(pLine, stride, 1, pFile);

				for (i = 0; i < stride; i+= channels)
				{
					int temp = pLine[i];
					pLine[i] = pLine[i + 2];
					pLine[i + 2] = temp;
				}
			}
		}
		//else

	}
	ttmp=2;
	tw=1;
	while (ttmp<texture.w)
		{
		ttmp*=2;
		tw++;
		}
		
	ttmp=2;
	th=1;
	while (ttmp<texture.h)
		{
		ttmp*=2;
		th++;
		}

	texture.ptr = textData;

	return 1;
}

/****************************************************************************\
|* Load a BMP format image
\****************************************************************************/
int Image::loadbmp(char *filename)
{
	int ttmp;
	Bitmap *LoadingImage;
	int xtmp=0, ytmp=0;

	LoadingImage = new Bitmap(filename);
	
	if (LoadingImage->loaded == false)
	{
		_log << "Failed to load Bitmap " << filename << "\n";
		return 0;
	}
	
	if (LoadingImage->bpp == 8)
	{
		clutData= (unsigned char*) memalign(16, 256*4);
		for (ttmp=0; ttmp<256*4; ttmp+=4)
			//fread(clutData+ttmp, 1, 3, im_file);
			if ((ttmp / 4) <= LoadingImage->numColors)
			{
				*(clutData + ttmp) = LoadingImage->colors[ttmp / 4].rgbRed;
				*(clutData + ttmp + 1) = LoadingImage->colors[ttmp / 4].rgbGreen;
				*(clutData + ttmp + 2) = LoadingImage->colors[ttmp / 4].rgbBlue;
			}
			else
			{
				*(clutData + ttmp) = 0;
				*(clutData + ttmp + 1) = 0;
				*(clutData + ttmp + 2) = 0;
			}
	}

	texture.w = LoadingImage->width;
	texture.h = LoadingImage->height;
	
	textData= (unsigned char*) memalign(16, texture.w * texture.h * (LoadingImage->bpp / 8));

	if (LoadingImage->bpp == 8)
	{
		for (ytmp = 0; ytmp < texture.h; ytmp++)
		{
			for (xtmp=0; xtmp<=texture.w; xtmp++)
			{
				//Reverse the reading of the image from one
				//location to the other.
				//Bitmaps are stored upside down.
				textData[ytmp * texture.w + xtmp] = LoadingImage->data[(texture.h * texture.w) - ytmp * texture.w + xtmp];
			
			}
		}
	}
	else if (LoadingImage->bpp == 24)
	{
		for (ytmp = 0; ytmp < texture.h * (LoadingImage->bpp / 8.0); ytmp += 3)
		{
			for (xtmp=0; xtmp<=texture.w * (LoadingImage->bpp / 8.0); xtmp += 3)
			{
				//Reverse the reading of the image from one
				//location to the other.
				//Bitmaps are stored upside down :(
				textData[ytmp * texture.w + xtmp] = LoadingImage->data[(int)(texture.h * texture.w * (LoadingImage->bpp / 8.0)) - ytmp * texture.w + xtmp];
				textData[ytmp * texture.w + xtmp + 1] = LoadingImage->data[(int)(texture.h * texture.w * (LoadingImage->bpp / 8.0)) - ytmp * texture.w + xtmp + 1];
				textData[ytmp * texture.w + xtmp + 2] = LoadingImage->data[(int)(texture.h * texture.w * (LoadingImage->bpp / 8.0)) - ytmp * texture.w + xtmp + 2];
			
			}
		}

	}

	
	if (LoadingImage->bpp == 8)
	{
		// swap bits 3 and 4 to get equivalent to CLUT adjustment
		for (ttmp=0; ttmp<(texture.w * texture.h/4); ttmp++)
			((unsigned int*)textData)[ttmp] =
				swapBits3and4(((unsigned int*)textData)[ttmp]);
	}
	texture.ptr = textData;

	if (LoadingImage->bpp == 8)
	{
		clut.ptr    = clutData;

		texture.psm = PS2_GS_PSMT8;
		if (texture.w > 64)
			texture.fbw=texture.w / 64;
		else
			texture.fbw=1;
		
		clut.psm	= PS2_GS_PSMCT32;
		clut.h		= clut.w=16;
		clut.fbw	= 1;
	}
	else if (LoadingImage->bpp == 24)
	{
		texture.psm = PS2_GS_PSMCT24;
		if (texture.w > 64)
			texture.fbw=texture.w / 64;
		else
			texture.fbw=1;

	}
	ttmp=2;
	tw=1;
	while (ttmp<texture.w)
		{
		ttmp*=2;
		tw++;
		}
		
	ttmp=2;
	th=1;
	while (ttmp<texture.h)
		{
		ttmp*=2;
		th++;
		}
	delete LoadingImage;	
	_log << "Loaded image '" << filename << "' (" << texture.w 
	     << " x " << texture.h << ")\n";
	return 0;

}

/****************************************************************************\
|* Load a PCX format image
\****************************************************************************/
int Image::loadpcx(char *filename)
	{
	int ttmp;
	struct pcxheader
		{
		unsigned char manufacturer, version, encoding, bpp;
		short int xmin, ymin, xmax, ymax;
		unsigned char other[116];
		} header;
	
	FILE *im_file=fopen(filename, "r");
	unsigned char howmany, databyte;
	int xtmp=0, ytmp=0;

	if (im_file==NULL)
		{
		_log << "Cannot open image '" << filename << "'\n";
		init();
		return 1;
		}

	fseek(im_file, 0, SEEK_SET);
	fread(&header, 1, 128, im_file);

	fseek(im_file, -769, SEEK_END);
	fread(&databyte, 1, 1, im_file);
	if (databyte==12)
		{
		clutData= (unsigned char*) memalign(16, 256*4);
		for (ttmp=0; ttmp<256*4; ttmp+=4)
			fread(clutData+ttmp, 1, 3, im_file);
		}
	else return 3;

	if ((header.version!=5) || (header.bpp!=8) || (databyte!=12))
		{
		_log << "PCX image '" << filename << "' has unsupported format.\n";
		fclose(im_file);
		init();
		return 2;
		}

	texture.w = header.xmax - header.xmin + 1;
	texture.h = header.ymax - header.ymin + 1;
	_log << "texture.h = " << texture.h << "\n";
	_log << "texture.w = " << texture.w << "\n";

	textData= (unsigned char*) memalign(16, texture.w * texture.h);
	fseek(im_file, 128, SEEK_SET);

	while (ytmp < texture.h)
		{
		int c;
		fread (&databyte, 1, 1, im_file);
		howmany=1;
		if ((databyte&0xC0)==0xC0)
			{
			howmany = databyte & 0x3F;
			fread (&databyte, 1, 1, im_file);
			}
		for (c=1; c<=howmany; c++)
			{
			textData[ytmp * texture.w+xtmp] = databyte;
			if (++xtmp >= texture.w)
				{
				xtmp=0;
				ytmp++;
				}
			}
		}

	fclose(im_file);

	// swap bits 3 and 4 to get equivalent to CLUT adjustment
	for (ttmp=0; ttmp<(texture.w * texture.h/4); ttmp++)
		((unsigned int*)textData)[ttmp] =
				swapBits3and4(((unsigned int*)textData)[ttmp]);

	texture.ptr = textData;
	clut.ptr    = clutData;

	texture.psm = PS2_GS_PSMT8;
	if (texture.w > 64)
		texture.fbw=texture.w / 64;
	else
		texture.fbw=1;
		
	clut.psm	= PS2_GS_PSMCT32;
	clut.h		= clut.w=16;
	clut.fbw	= 1;

	ttmp=2;
	tw=1;
	while (ttmp<texture.w)
		{
		ttmp*=2;
		tw++;
		}
		
	ttmp=2;
	th=1;
	while (ttmp<texture.h)
		{
		ttmp*=2;
		th++;
		}
	
	_log << "Loaded image '" << filename << "' (" << texture.w 
	     << " x " << texture.h << ")\n";
	return 0;
	}

/****************************************************************************\
|* Delete the image data
\****************************************************************************/
void Image::erase()
	{
	if (textData != NULL)
		free(textData);
	textData=NULL;
	
	if (clutData != NULL)
		free(clutData);
	clutData=NULL;
	}

/****************************************************************************\
|* Set the address of the image in texture RAM
|*	The addressing works in units of blocks. 1 block is 256 bytes
\****************************************************************************/
void Image::setAddress(Framework *f, int firstimage)
	{
	
	int textureAddr;
	//The very first time you load an image into memory you
	//have to multiply the returned address by the size of one page
	//in blocks
	//1 page = 32 blocks
	//1 block = 256 bytes
	if (firstimage == 0)
	{
		textureAddr = f->getNext2k() * GS_PAGE_SIZE;
	}
	else
	{
		textureAddr = f->getNext2k();
	}
	_log << "IMAGE: Initial textureAddr = " << textureAddr << "\n";
	
	//If using a palleted texture then load the pallet into the clut
	if (texture.psm == PS2_GS_PSMT8)
	{		
		
		clut.fbp 	= textureAddr;
		//Calculate the number of blocks the clut takes and set
		//the next texture address to it.
		//Cacluation works in this formula.
		//((texture width + block width - 1) / block width) * 
		//	((texture height + block height - 1) / block height)
		//32 bit textures uses 8 for the block height and width.
		//The clut is a 16 by 16 32bit image.
		textureAddr    += ((16 + 8 - 1) / 8) * ((16 + 8 -1) / 8);
		_log << "IMAGE: After clut textureAddr = " << textureAddr << "\n";
		

	}
	f->setNext2k(textureAddr);
	//set the textures memeory address to what is calculated for textureaddr
	texture.fbp 	= textureAddr;

	//Depending on what type of pixel format used for the texture 
	//calculate the next address based on the same formula used for 
	//calculating the number of blocks the clut takes.
	switch (texture.psm)
		{
		case PS2_GS_PSMT8:
			//8bit textures use 16 for the block height and width
			textureAddr += ((texture.w + 16 - 1) / 16) * ((texture.h + 16 - 1) / 16);
			_log << "IMAGE: After PSMT8 textureAddr = " << textureAddr << "\n";

			f->setNext2k(textureAddr);
			break;

		case PS2_GS_PSMCT24:
			//24bit textures use 8 for the block height and width
			textureAddr += ((texture.w + 8 - 1) / 8) * ((texture.h + 8 - 1) / 8);
			_log << "IMAGE: After PSMCT24 textureAddr = " << textureAddr << "\n";

			f->setNext2k(textureAddr);
			break;

		default:
			_log << "Couldn't get the texture size (file "
			    << __FILE__ << ", line " << __LINE__ << ")\n";
			break;
		}
	}

/****************************************************************************\
|* Upload the image to the framebuffer. 
\****************************************************************************/
void Image::upload(Framework *f)
{
	if (texture.psm == PS2_GS_PSMT8)
	{
		ps2_gs_load_image((struct ps2_image*)(&clut));
		ps2_gs_load_image((struct ps2_image*)(&texture));
	}
	else if (texture.psm == PS2_GS_PSMCT24)
	{
		ps2_gs_load_image((struct ps2_image*)(&texture));
	}
}
	
	
