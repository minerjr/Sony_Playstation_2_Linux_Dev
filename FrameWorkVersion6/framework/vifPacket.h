/****************************************************************************\
|* VIF packet definitions
|* ======================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/

#ifndef __framework__vif__packet__header__
#define __framework__vif__packet__header__

#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "constants.h"
#include "log.h"

class VifPacket
	{
	public:
	  	u_int *				current;			// Current packet data item
  		u_long128 *			base;				// 1st data item
  		u_long128 *			dmatag;				// Packet DMA tag
  		u_int *				vifcodes;			// Packet codes
  		u_int 				len;				// Packet length
  		u_long64 *			giftag;				// GIF tags for packet
  		u_int 				pad2;				// padding
  		u_int 				pad3;
	
							VifPacket();
							~VifPacket();
		
		void				init(u_long128 *buffer);
		void				reset();
		u_long128 *			terminate();
		int  				size();

		void 				cnt(u_int flag);
		void 				next(u_long128 *next, u_int flag);
		void 				end(u_int flag);
		void 				ref(u_long128 *ref, u_int size, u_int opt1,
		 						u_int opt2, u_int flag);

		void 				openDirectCode(int stall);
		void 				closeDirectCode();
		void 				addCode(u_int code);
		void 				closeUpkCode();
		void 				openUpkCode(u_short vuaddr, u_int upkcmd, u_int cl,
										u_int wl);

		void 				add(u_int data);
		void 				addq(u_long64 data);
		void 				addq(u_long64* pData, u_int count);
		void 				add2q(u_long128 data);
		void 				add2q(u_long128* pData, u_int count);

		void 				dump();
		
	protected:
		void				align( u_int bit, u_int pos);
	};

#endif // ! __framework__vif__packet__header__
