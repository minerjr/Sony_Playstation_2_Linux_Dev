/****************************************************************************\
|* VU code manager class
|* =====================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|* 
|* This class handles all the VU codecs. Any file in libexec/vu<N>/... 
|* will be loaded in and can be uploaded to vu<N> at any time.
|*
|* Author					: SjG
|* First created			: Sun Jul 21 13:07:48 BST 2002
|* Modification history		: See below
|*
\****************************************************************************/
#ifndef __vu__manager__header__
#define __vu__manager__header__

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ps2vpu.h"

#include "log.h"

class VUManager
	{
	private:
	
	protected:
		int					_codeSizes[2];		// Sizes of data for each VU
		int					_vu;				// Which VU to manage
		int 				_current; 			// Index of the current codec
		int					_num;				// Number of codecs
		unsigned char **	_codecs;			// The codecs themselves
		char **				_names;				// Codec names
  		ps2_vpu *			_dev;				// Access to the device
		 
	public:
							VUManager(int id, ps2_vpu *dev);
							~VUManager(void);
		
		void				initialise(void);
		void				ensureActive(int idx);
		void				ensureActive(char *name);
	};
	
#endif // ! __vu__manager__header__
