/****************************************************************************\
|* Image loading routines
|* ======================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Sat Jul 20 15:02:39 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/

#ifndef __image__loader__header__
#define __image__loader__header__

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/types.h>

#include <ps2gs.h>

#include "constants.h"
#include "log.h"
#include "framework.h"

typedef struct {
	void *ptr;
	int fbp;
	int fbw;
	int psm;
	int x, y;
	int w, h;
	} ImageType;

class Image
	{
	public:
		ImageType		clut;				// The CLUT, if indexed
		ImageType		texture;			// The texture itself
		int				tw,th;				// Width & height
		byte			*textData;			// Texture data
		byte			*clutData;			// Clut data
	
						Image(void);
						Image(char *name);
						~Image(void);
		
		void			init(void);
		int				load(char *name);
		void			erase(void);
		void			setAddress(Framework *f);
		void			upload(Framework *f);
		
	protected:
		unsigned int	swapBits3and4(unsigned int val);
	};

#endif // ! __image__loader__header__
