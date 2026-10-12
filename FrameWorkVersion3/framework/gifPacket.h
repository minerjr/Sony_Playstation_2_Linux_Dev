/****************************************************************************\
|* GIF packet definitions
|* ======================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Sun Jul 21 12:45:40 BST 2002
|* Modification history		: See below
|*
\****************************************************************************/

#ifndef __framework__gif__packet__header__
#define __framework__gif__packet__header__

#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "constants.h"


class GifPacket
	{
	public:
		u_int *				current;			// Current tag
		u_long128 *			base;				// 1st tag
		u_long128 *			dmatag;				// DMA tags
		u_long64 *			giftag;				// GIF tags
		
	
							GifPacket();
							~GifPacket();
		
		void 				init(u_long128 *buffer);
		void 				reset();
		u_long128 *			terminate();
		int  				size();

		void 				add(u_long64  data);
		void 				add2(u_long128 data);

		void 				dump();
	};

#endif // ! __framework__gif__packet__header__
