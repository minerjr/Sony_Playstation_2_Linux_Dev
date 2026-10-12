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
	load(name);
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
|* Load a PCX format image
\****************************************************************************/
int Image::load(char *filename)
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
\****************************************************************************/
void Image::setAddress(Framework *f)
	{
	int textureAddr = f->getNext2k() * 2048 / 64;
	
	clut.fbp 		= textureAddr;
	textureAddr    += 32;
	
	f->setNext2k(textureAddr);
	texture.fbp 	= textureAddr;

	switch (texture.psm)
		{
		case PS2_GS_PSMT8:
			textureAddr += 32 * ((texture.w < 128) ? 1 : (texture.w / 128)) *
								((texture.h < 64)  ? 1 : (texture.h / 64));
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
	ps2_gs_load_image((struct ps2_image*)(&clut));
	ps2_gs_load_image((struct ps2_image*)(&texture));
	}
	
	
