/****************************************************************************\
|* PS2 framework definitions
|* =========================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/
#ifndef __framework__header__
#define __framework__header__

#include <stdio.h>
#include <stdlib.h>
#include <stream.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#include <signal.h>

#include "ps2gs.h"
#include "ps2vpufile.h"
#include "ps2dma.h"
#include "ps2vpu.h"
#include <libvu0.h>

#include "log.h"
#include "structs.h"
#include "vuManager.h"

class Framework
	{
	private:
			
	protected:
  		char * 				_progName;			// argv[0]
  		int 				_inter;				// Are we interlaced ?
  		int 				_outMode;			// Type of display: Pal, Vesa...
  		int 				_ffMode;			// Fields or Frames
  		int 				_resolution;		// Screen size
  		int 				_refreshRate;		// Screen update frequency
  		int 				_psm;				// Pixel format: rgb, rgba...
  		int 				_zpsm;				// Z-buffer pixel format
  		int 				_zbits;				// Z-buffer depth

  		int 				_fdGs;				// GS file descriptor
  		ps2_vpu *			_vpu0;				// Access to VU0
		ps2_vpu *			_vpu1;				// Access to VU1
  		ps2_gs_gparam *		_gp;				// The system parameters

  		ps2_gs_dbuff * 		_db;				// The display buffer control
  		int 				_next2k;			// ?
  		int 				_odev;				// Used for double buffering
  		u_int 				_frame;				// The frame number
  		bool 				_useTestControls;	// Whether to use test controls!
  		clock_t 			_startTime;			// When the program started
	
		VUManager *			_vu1mgr;			// Manager for VU1 codecs
	
	public:
							Framework(int argc, char **argv);
		virtual				~Framework();
	
 	 	void 				gsEnableVcSwitch();
  		void 				gsStartDisplay();
  		void 				gsReset();
  		int 				release();
  		int 				acquire();

  		int 				getNext2k();
		void				setNext2k(int x)	{ _next2k = x; }

  		void 				clearBackBuffer( int frame = -1);
  		void 				swapBuffers( int frame, int odev );
  		void 				gsSetDoubleBuffer();
		
  		virtual void 		doExtraAcquire() 	{}
  		virtual void 		doExtraRelease() 	{}

  		bool 				hasZBuffer() 		{ return _zbits != 0; }
  		int  				zBufferEnd() 		{ return (1<<_zbits)-1; }


  		void 				dmaSendToVu1(VPUFILE *vfd, ps2_dmatag *start);
  		void 				dmaStartOnVu1(VPUFILE *vfd, ps2_dmatag *start);
		void				dmaWait(int size=0);
		
  		bool 				snapShotScreen();
		void				usage(void);
		void 				parseCmdLine(int argc, char **argv);

 		void 				setRgbAq(int r=0x10, int g=0x10, int b=0x10);
  		void 				useStandardTestControls();
  		void 				start();
		void 				flip();
		
  		u_int 				getFrame() 			{return _frame;}
 		ps2_gs_dbuff 		getDrawConfig()		{return *_db;}
		ps2_gs_gparam *		getParams()			{return _gp;}
		int					getGs()				{return _fdGs;}
	};


#endif // ! __framework__header__
