/****************************************************************************\
|* Sprite class - used for rectangular image areas
|* ===============================================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Sat Jul 20 19:10:21 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/
#ifndef __sprite__header__
#define __sprite__header__

#include "framework.h"
#include "image.h"

class Sprite
	{
	private:
	
	protected:
		Framework *			_framework;			// The PS2 environment
		int 				_r, _g, _b, _a;		// brightness values
		int 				_x0,_y0, _x1, _y1;	// Transformed position & size
		int 				_w, _h, _x, _y;		// Position & size
		
		u_int *				_base;				// The Tag storage area
		GifPacket			_gif;				// The GIF packet to send
		
	public:
	
							Sprite(Framework *f, Image *im, 
							                    int x, int y, int w, int h);
							~Sprite();
		
		void				setRGBA(int r=-1, int g=-1, int b=-1, int a=-1);
		u_int *				getGifTag(void)		{return _base;}
		void				draw(void);
	};

#endif // ! __sprite__header__
