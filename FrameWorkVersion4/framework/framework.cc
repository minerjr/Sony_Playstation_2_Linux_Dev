/****************************************************************************\
|* PS2 framework definitions
|* =========================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|* 
|* This code sets up the initial state of the PS2 and allows easy changes
|* to the hardware to be made
|*
|* Author					: SjG, various others
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See class header files
|*
\****************************************************************************/
#include "framework.h"

/****************************************************************************\
|* This is a static variable for callbacks to access the class. It's only
|* visible within this file
\****************************************************************************/
static Framework *__framework;


/****************************************************************************\
|* These are callback functions - they're only visible in this file and use
|* the above static variable to call methods of the Framework object
\****************************************************************************/
static int Release()
	{ return __framework->release(); }

static int Acquire()
	{ return __framework->acquire(); }

static void terminate(int signal)
	{
	Release();
	_log << "Terminating on signal " << signal << "\n";
	exit(0);
	}


/****************************************************************************\
|* Constructor
\****************************************************************************/
Framework::Framework(int argc, char **argv)
	{
	if( __framework!=NULL )
    	throw( "Framework is meant to be a singleton!\n" );
  	__framework = this;

  	_gp 				= ps2_gs_get_gparam();
  	_fdGs 				= ps2_gs_open(-1);
  	_vpu0 				= ps2_vpu_open(0); // must be opened for vpu1...
  	_vpu1 				= ps2_vpu_open(1);
	_db					= (ps2_gs_dbuff *) memalign(16, sizeof(ps2_gs_dbuff));
	_vu1mgr				= new VUManager(1, _vpu1);
	
	_frame 				= 1;
    _useTestControls 	= 0;
	
	parseCmdLine( argc, argv);
    
	ps2_gs_vc_graphicsmode();
	gsReset();
    gsSetDoubleBuffer();
    setRgbAq();
	
	if (signal(SIGINT, terminate) == SIG_ERR)
		_log << "Failed to attach SIGINT signal handler\n";
	if (signal(SIGTERM, terminate) == SIG_ERR)
		_log << "Failed to attach SIGTERM signal hander\n";
	}

/****************************************************************************\
|* Destructor
\****************************************************************************/
Framework::~Framework()
	{
	ps2_vpu_close(_vpu1);
	ps2_vpu_close(_vpu0);
	ps2_gs_close();
	}
	
/****************************************************************************\
|* Public method - reset the GS
\****************************************************************************/
void Framework::gsReset(void)
	{
	ps2_vpu_reset(_vpu1);
	ps2_vpu_reset(_vpu0);
	ps2_gs_reset(0, _inter, _outMode, _ffMode, _resolution, _refreshRate);
 	}
	
/****************************************************************************\
|* Public method - send data to VU1
\****************************************************************************/
void Framework::dmaSendToVu1( VPUFILE *vfd, ps2_dmatag *dma_start)
	{
    ps2_dma_send( ps2_vpu_fd(_vpu1), vfd, dma_start);
  	}
	
/****************************************************************************\
|* Public method - wait until the DMA queue is of the specified size
\****************************************************************************/
void Framework::dmaWait(int size)
	{
    ps2_dma_wait(_fdGs, size);
    ps2_dma_wait(ps2_vpu_fd(_vpu1), size);
  	}
	
/****************************************************************************\
|* Public method - tell VU1 to start
\****************************************************************************/
void Framework::dmaStartOnVu1( VPUFILE *vfd, ps2_dmatag *dma_start)
	{
    ps2_dma_start(ps2_vpu_fd(_vpu1), vfd, dma_start);
  	}
	
/****************************************************************************\
|* Public method - Release our deathgrip on the system
\****************************************************************************/
int Framework::release()
	{
  	if (_vpu1)
    	ps2_vpu_close(_vpu1);
	
  	if (_vpu0) 
    	ps2_vpu_close(_vpu0);
	
  	if (_fdGs >= 0)
    	ps2_gs_close();
	
  	_vpu1 	= NULL;
  	_vpu0 	= NULL;
  	_fdGs 	= -1;

  	doExtraRelease();

  	return PS2_GS_VC_REL_SUCCESS;
	}

	
/****************************************************************************\
|* Public method - Acquire a deathgrip on the system
\****************************************************************************/
int Framework::acquire()
	{
  	_fdGs 	= ps2_gs_open(-1);
  	_vpu0 	= ps2_vpu_open(0);
  	_vpu1 	= ps2_vpu_open(1);
	
  	if (_fdGs < 0 || _vpu0 == NULL || _vpu1 == NULL)
		{
    	release();
    	return PS2_GS_VC_ACQ_FAILURE;
  		}
	
  	ps2_vpu_reset(_vpu1);
  	ps2_vpu_reset(_vpu0);
	
  	ps2_gs_vc_graphicsmode();	// ??? (seconded, ???)
  
  	ps2_gs_reset(0, _inter, _outMode, _ffMode, _resolution, _refreshRate);
	
  	doExtraAcquire();
	
  	return PS2_GS_VC_ACQ_SUCCESS;
	}


	
/****************************************************************************\
|* Public method - Tell the user what the options are
\****************************************************************************/
void Framework::usage(void)
	{
	fprintf(stderr,
		"Usage: %s [<ii>] [<mode>] [<ff>] [<reso>] "
		             "[<rate>] [<bpp>] [<z-bpp>]\n", _progName);
	
	fprintf(stderr,
		"    ii: interlace or noninterlace\n"
		"        -inter, -nointer\n"
		"    mode: video mode\n"
		"        -ntsc, -pal, -dtv, -vesa\n"
		"    ff: field/frame mode\n"
		"        -frame, -field\n"
		"    reso: resolution\n"
		"        -640x480, -800x600, -1024x768\n"
		"    rate: refresh rate\n"
		"        -60, -75\n"
		"    bpp: color bits per pixel\n"
		"        -16, -24, -32\n"
		"    z-bpp: depth bits per pixel\n"
		"        -z0, -z16, -z24\n"
		);
	exit(0);
	}
	
	
/****************************************************************************\
|* Public method - parse the commandline and set the relevant options
\****************************************************************************/
void Framework::parseCmdLine(int argc, char **argv)
	{
  	_progName 		= argv[0];

  	_inter 			= PS2_GS_NOINTERLACE;
  	_outMode 		= PS2_GS_VESA;
  	_ffMode 		= PS2_GS_FRAME;
  	_resolution 	= PS2_GS_640x480;
  	_refreshRate 	= PS2_GS_60Hz;
  	_psm 			= PS2_GS_PSMCT32;
  	_zpsm 			= PS2_GS_PSMZ24;
  	_zbits 			= 24;
	
  	for (int i = 1; i < argc; i++)
		{
    	char *p = argv[i];
		
		if (strcmp(p, "-inter") == 0) 
		  	_inter = PS2_GS_INTERLACE;
		else if (strcmp(p, "-nointer") == 0)
		  	_inter = PS2_GS_NOINTERLACE;
		else if (strcmp(p, "-ntsc") == 0)
		  	_outMode = PS2_GS_NTSC;
		else if (strcmp(p, "-pal") == 0)
		  	_outMode = PS2_GS_PAL;
		else if (strcmp(p, "-dtv") == 0)
		  	_outMode = PS2_GS_DTV;
		else if (strcmp(p, "-vesa") == 0)
		  	_outMode = PS2_GS_VESA;
		else if (strcmp(p, "-frame") == 0)
		  	_ffMode = PS2_GS_FRAME;
		else if (strcmp(p, "-field") == 0)
		  	_ffMode = PS2_GS_FIELD;
		else if (strcmp(p, "-640x480") == 0)
		  	_resolution = PS2_GS_640x480;
		else if (strcmp(p, "-800x600") == 0)
		  	_resolution = PS2_GS_800x600;
		else if (strcmp(p, "-1024x768") == 0)
		  	_resolution = PS2_GS_1024x768;
		else if (strcmp(p, "-1280x1024") == 0)
		  	_resolution = PS2_GS_1280x1024;
		else if (strcmp(p, "-60") == 0)
		  	_refreshRate = PS2_GS_60Hz;
		else if (strcmp(p, "-75") == 0)
		  	_refreshRate = PS2_GS_75Hz;
		else if (strcmp(p, "-32") == 0)
		  	_psm = PS2_GS_PSMCT32;
		else if (strcmp(p, "-24") == 0)
		  	_psm = PS2_GS_PSMCT24;
		else if (strcmp(p, "-16") == 0)
		  	_psm = PS2_GS_PSMCT16S;
		else if (strcmp(p, "-z24") == 0)
			{
		  	_zpsm = PS2_GS_PSMZ24;
		  	_zbits = 24;
			}
		else if (strcmp(p, "-z16") == 0)
			{
		  	_zpsm = PS2_GS_PSMZ16S;
		  	_zbits = 16;
			}
		else if (strcmp(p, "-z0") == 0)
			{
		  	_zpsm = PS2_GS_PSMZ24;
		  	_zbits = 0;
			}
		else
			{
		  	usage();
			}
		}

	if (_psm == PS2_GS_PSMCT16S && _zpsm == PS2_GS_PSMZ16)
		{
		_psm = PS2_GS_PSMCT16;
		_zpsm = PS2_GS_PSMZ16;
		}
	}
	
/****************************************************************************\
|* Public method - Register the acquire/release functions with the kernel so
|*                 we get told when the VC has switched
\****************************************************************************/
void Framework::gsEnableVcSwitch()
	{
	ps2_gs_vc_enablevcswitch( Acquire, Release);
	}
	
/****************************************************************************\
|* Public method - Start the display
\****************************************************************************/
void Framework::gsStartDisplay()
	{
  	ps2_gs_start_display(1);
	}
	
/****************************************************************************\
|* Public method - Snapshot the display
\****************************************************************************/
bool Framework::snapShotScreen()
	{
  	int cbSave = _gp->width * _gp->height * 3;
  	void * image = memalign(16, cbSave);
  	if (image == NULL)
    	return false;

  	// get the screen image
  	ps2_image im;
  	const int ibVideoRAM = 0; // first video buffer at start of video RAM
  	ps2_gs_set_image(&im, ibVideoRAM/64, _gp->width/64, PS2_GS_PSMCT24,
		   						0, 0, _gp->width, _gp->height, image);
  	
	if (ps2_gs_store_image(&im) < 0)
    	{
      	printf("Error snapshotting screen\n");
      	free(image);
      	return false;
    	}

  	bool bOK = false;
	struct stat sbuf;
	
  	for (int iFile = 1; iFile < 100; iFile++)
		{
    	char szFile[64];
    	sprintf(szFile, "snap%02d.rgb", iFile);
		if (!stat(szFile, &sbuf))
			continue;

    	FILE *pf = fopen(szFile, "wb");
    	bOK = true;
    	if (fwrite(image, cbSave, 1, pf) != 1)
      		bOK = false;
    	if (fclose(pf) != 0)
      		bOK = false;
    	if (bOK)
      		_log << "INFO: saved screenshot to " << szFile << "\n";
    	break;
  		}

  	free(image);
  	if (!bOK)
   		_log << "ERROR: failed to save screenshot\n";
  	return bOK;
	}


	
/****************************************************************************\
|* Method - return the next 2k ?
\****************************************************************************/
int Framework::getNext2k()
	{
	return _next2k;
	}
	
/****************************************************************************\
|* Method - clear the non-displayed buffer
\****************************************************************************/
void Framework::clearBackBuffer(int frame)
	{
    if (frame < 0)
		frame = _frame & 0x01;
		
    ps2_gs_finish 		finish;
	ps2_gs_set_finish(&finish);
	ps2_gs_put_drawenv( frame ? &(_db->giftag0) : &(_db->giftag1) );
	
    ps2_gs_wait_finish(&finish);
  	}

	
/****************************************************************************\
|* Method - swap the buffers
\****************************************************************************/
void Framework::swapBuffers( int frame, int odev )
	{
    ps2_dma_wait(_fdGs, 0);
	ps2_gs_set_half_offset((frame & 1) ? &(_db->draw1) : &(_db->draw0), odev);
    ps2_gs_swap_dbuff(_db, frame);
	}

	
/****************************************************************************\
|* Method - Set double-buffering
\****************************************************************************/
void Framework::gsSetDoubleBuffer()
	{
    ps2_gs_gparam *gp = ps2_gs_get_gparam();
    
	_next2k = ps2_gs_set_dbuff(_db, _psm, gp->width, gp->height,
			      (_zbits == 0) ? 0 : PS2_GS_ZGREATER, _zpsm, 1);
  	}
	
/****************************************************************************\
|* Method - set the controls to standard mode
\****************************************************************************/
void Framework::useStandardTestControls(void)
	{
	_useTestControls = 1;
	}
			
/****************************************************************************\
|* Method - Rock'n'roll
\****************************************************************************/
void Framework::start(void)
	{
    doExtraAcquire();
    clearBackBuffer( _frame & 1 );
    _odev = !ps2_gs_sync_v(0);
    gsStartDisplay();
    gsEnableVcSwitch();

    ps2_gs_vc_lock();
    swapBuffers( _frame, _odev );

    _log << "Info: _gp->center: " << _gp->center_x << ", " 
	     << _gp->center_y << "\n";
    _log << "Info: _gp->offset: " << _gp->offset_x << ", "
	     << _gp->offset_y << "\n";
    _log << "Info: _gp->version: " << _gp->version << "\n";
    _log << "Info: _gp->interlace_mode: " << _gp->interlace_mode << "\n";
    _log << "Info: _gp->interlace_format: " << _gp->interlace_format << "\n";
    _log << "Info: _gp->out_mode: " << _gp->out_mode << "\n";
    _log << "Info: _gp->ff_mode: " << _gp->ff_mode << "\n";
    _log << "Info: _gp->resolution: " << _gp->resolution << "\n";
    _log << "Info: _gp->refresh_rate: " << _gp->refresh_rate << "\n";
    _log << "Info: _gp->width: " << _gp->width << "\n";
    _log << "Info: _gp->height: " << _gp->height << "\n";
    _log << "Info: _gp->pixel_ratio: " << _gp->pixel_ratio << "\n";
    
	_startTime = clock();
	}
				
/****************************************************************************\
|* Method - flip the screen at the next vsync
\****************************************************************************/
void Framework::flip()
	{
    _frame++;
    _odev = !ps2_gs_sync_v(0);
	
    ps2_gs_vc_unlock();
    ps2_gs_vc_lock();
    
	swapBuffers( _frame, _odev );
  	}
			
/****************************************************************************\
|* Method - do something to the RGB AQ (!)
\****************************************************************************/
void Framework::setRgbAq(int r, int g, int b)
	{
    *(__u64 *)&(_db->clear0.rgbaq) = PS2_GS_SETREG_RGBAQ(r,g,b, 0x80, 0x3f800000);
    *(__u64 *)&(_db->clear1.rgbaq) = PS2_GS_SETREG_RGBAQ(r,g,b, 0x80, 0x3f800000);
  	}
	
