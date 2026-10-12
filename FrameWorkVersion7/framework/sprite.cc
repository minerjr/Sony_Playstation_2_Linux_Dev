/****************************************************************************\
|* Sprite class - used for rectangular image areas
|* ===============================================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG, various others
|* First created			: Sat Jul 20 19:10:21 GMT 2002
|* Modification history		: See class header files
|*
\****************************************************************************/
#include "sprite.h"


/****************************************************************************\
|* Constructor
\****************************************************************************/
Sprite::Sprite(Framework *f, Image *im, int x, int y, int w, int h)
	{
	_framework = f;
	_im = im;	
	_r = _g = _b = _a = 0x80;			// Full brightness
	
	// Input (user) co-ordinates
	_x = x;
	_y = y;
	_w = w;
	_h = h;
	
	int W = f->getParams()->width;
	int H = f->getParams()->height;
	
	// transform to screen coordinate system
	_x0 = (((4096 - W) / 2) << 4) + _x;
	_y0 = (((4096 - H) / 2) << 4) + _y;
	_x1 = _x0 + _w;
	_y1 = _y0 + _h;
	
	_log << "Displaying sprite at (" << _x0 << "," << _y0 << ") => ("
	     << _x1 << "," << _y1 << ")\n";
	_base = (u_int *) memalign (16, 16*1024);
	
	// start GIF packet for the sprite
	_gif.init((u_long128*)(_base));

	// Add GIF tag to setup enviromnent
	_gif.add(PS2_GIFTAG_SET_TOPHALF(NLOOP(1), EOP(1), PRE(0), 0, 
	                                PS2_GIFTAG_FLG_PACKED, NREG(3)));
	_gif.add(0xEEEL);

	// Set TEX0 register to use the ps2tux image
	_gif.add(PS2_GS_SETREG_TEX0(im->texture.fbp, im->texture.fbw,
				im->texture.psm, im->tw, im->th, 1,
				PS2_GS_TEX_TFX_DECAL, im->clut.fbp, im->clut.psm, 0, 0, 1));
	_gif.add(PS2_GS_TEX0_1);

	// Set TEX1 to bilinear filtering
	_gif.add(PS2_GS_SETREG_TEX1_1(0, 0, PS2_GS_TEX1_MMAG_LINEAR,
	                                    PS2_GS_TEX1_MMIN_LINEAR, 0, 0, 0 ));
	_gif.add(PS2_GS_TEX1_1);

	// Set CLAMP to clamp, not repeat
	_gif.add(PS2_GS_SETREG_CLAMP(1, 1, 0, im->texture.w, 0, im->texture.h));
	_gif.add(PS2_GS_CLAMP_1);

	// Add GIF tag to send vertex data
	_gif.add(PS2_GIFTAG_SET_TOPHALF(NLOOP(1), EOP(1), PRE(0), 0,
	                                     PS2_GIFTAG_FLG_REGLIST, NREG(6)));
	_gif.add(0x525210L);

	// add the sprite data
	_gif.add(PS2_GS_SETREG_PRIM( PS2_GS_PRIM_PRIM_SPRITE, GOURAUD(0),
	                                       TEXTURED(1), 0, 0, 0, 0, 0, 0));
	_gif.add(PS2_GS_SETREG_RGBAQ(_r, _g, _b, _a, FLOAT_ONE));
	_gif.add(PS2_GS_SETREG_ST(0, 0));
	_gif.add(PS2_GS_SETREG_XYZ(_x0, _y0, 1));
	_gif.add(PS2_GS_SETREG_ST(FLOAT_ONE, FLOAT_ONE));
	_gif.add(PS2_GS_SETREG_XYZ(_x1, _y1, 1));

	// terminate the packet
	_gif.terminate();
	}
	
/****************************************************************************\
|* Destructor
\****************************************************************************/
Sprite::~Sprite()
	{
	}
		
/****************************************************************************\
|* Method - set the RGBA values for the sprite brightness levels. Any -ve
|* values specified will not be set. All values default to -1
\****************************************************************************/
void Sprite::setRGBA(int r, int g, int b, int a)
	{
	if (r>=0) _r = r;
	if (g>=0) _g = g;
	if (b>=0) _b = b;
	if (a>=0) _a = a;
	}

/****************************************************************************\
|* Method - draw the sprite to the screen
\****************************************************************************/
void Sprite::draw(void)
	{
	ps2_dma_start_n(_framework->getGs(), _gif.base, _gif.size());
	}

/****************************************************************************\
|* Method - change the x y coordinates
|*	Rebuilds a packet with some new x y coordinates.
\****************************************************************************/
void Sprite::setXY(int x, int y)
{
	int W = _framework->getParams()->width;
	int H = _framework->getParams()->height;
	
	// transform to screen coordinate system
	_x0 = (((4096 - W) / 2) << 4) + x;
	_y0 = (((4096 - H) / 2) << 4) + y;
	_x1 = _x0 + _w;
	_y1 = _y0 + _h;
	
	_log << "Displaying sprite at (" << _x0 << "," << _y0 << ") => ("
	     << _x1 << "," << _y1 << ")\n";
	//_base = (u_int *) memalign (16, 16*1024);
	
	// start GIF packet for the sprite
	_gif.init((u_long128*)(_base));

	// Add GIF tag to setup enviromnent
	_gif.add(PS2_GIFTAG_SET_TOPHALF(NLOOP(1), EOP(1), PRE(0), 0, 
	                                PS2_GIFTAG_FLG_PACKED, NREG(3)));
	_gif.add(0xEEEL);

	// Set TEX0 register to use the ps2tux image
	_gif.add(PS2_GS_SETREG_TEX0(_im->texture.fbp, _im->texture.fbw,
				_im->texture.psm, _im->tw, _im->th, 1,
				PS2_GS_TEX_TFX_DECAL, _im->clut.fbp, _im->clut.psm, 0, 0, 1));
	_gif.add(PS2_GS_TEX0_1);

	// Set TEX1 to bilinear filtering
	_gif.add(PS2_GS_SETREG_TEX1_1(0, 0, PS2_GS_TEX1_MMAG_LINEAR,
	                                    PS2_GS_TEX1_MMIN_LINEAR, 0, 0, 0 ));
	_gif.add(PS2_GS_TEX1_1);

	// Set CLAMP to clamp, not repeat
	_gif.add(PS2_GS_SETREG_CLAMP(1, 1, 0, _im->texture.w, 0, _im->texture.h));
	_gif.add(PS2_GS_CLAMP_1);

	// Add GIF tag to send vertex data
	_gif.add(PS2_GIFTAG_SET_TOPHALF(NLOOP(1), EOP(1), PRE(0), 0,
	                                     PS2_GIFTAG_FLG_REGLIST, NREG(6)));
	_gif.add(0x525210L);

	// add the sprite data
	_gif.add(PS2_GS_SETREG_PRIM( PS2_GS_PRIM_PRIM_SPRITE, GOURAUD(0),
	                                       TEXTURED(1), 0, 0, 0, 0, 0, 0));
	_gif.add(PS2_GS_SETREG_RGBAQ(_r, _g, _b, _a, FLOAT_ONE));
	_gif.add(PS2_GS_SETREG_ST(0, 0));
	_gif.add(PS2_GS_SETREG_XYZ(_x0, _y0, 2));
	_gif.add(PS2_GS_SETREG_ST(FLOAT_ONE, FLOAT_ONE));
	_gif.add(PS2_GS_SETREG_XYZ(_x1, _y1, 2));

	// terminate the packet
	_gif.terminate();

	

}
