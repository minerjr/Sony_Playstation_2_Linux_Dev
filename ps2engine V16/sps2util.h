/*
    Copyright (C) 2002 Terratron Technologies Inc.  All Rights Reserved.
    Author: Steven Osman, Morten Mikkelsen, Lionel Lemarie
 
    This file is part of sps2.

    Before using this file you MUST agree to the license agreement in
    the file LICENSE provided with this package.
  
    For more information visit:
    http://www.playstation2-linux.com/projects/sps2/
    http://window.terratron.com/~sosman/ps2linux/
    http://www.terratron.com/

    7/12/2003 - Sauce
    Initial version

    8/8/2003 - Sauce
    Split sps2UScreenSwap to: sps2UScreenSwap and sps2UScreenClear(r, g, b)
*/
#ifndef __SPS2_UTIL_H
#define __SPS2_UTIL_H

#include "sps2types.h"
#include "sps2lib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPS2U_SCR_VESA		0
#define SPS2U_SCR_NTSC		2
#define SPS2U_SCR_PAL		3

  /**
   * Creates a virtual console, builds a double and Z buffer and prepares the
   * GS for graphics output.
   * iSuppressSignalHandlers -- if zero, then sps2UScreenInit will trap all
   * signals and make sure that the display is shutdown gracefully before the
   * program aborts when it receives a signal.  If set to non-zero, then
   * the programmer is responsible for making sure that the screen is properly
   * shut down when the program aborts.
   */ 
  void sps2UScreenInit(int iSuppressSignalHandlers);

  /**
   * Destroys the virtual console and reverts the GS to it's original state.
   * You should make sure to call this after calling sps2UScreenInit.
   */
  void sps2UScreenShutdown();

  /**
   * VSync and swap the two buffers.  The one that was being drawn will now
   * be displayed and the one that was being displayed is prepared for drawing.
   * It is not, however cleared.  Use sps2UScreenClear() to clear the new
   * drawing screen.
   */
  void sps2UScreenSwap();

  /**
   * Clears the drawing screen with the specified color.
   */
  void sps2UScreenClear(int iRed, int iGreen, int iBlue);

  /**
   * Returns the page number of the first available page in the GS memory,
   * after the two buffers and Z buffer.
   */
  unsigned int sps2UScreenGetFirstFreeGSPage();

  /**
   * Returns the first page number of the Z buffer
   */
  unsigned int sps2UScreenGetZPtr();

  /**
   * Returns the first page number of the first draw buffer.
   */
  unsigned int sps2UScreenGetDrawBuff1();

  /**
   * Returns the first page number of the second draw buffer.
   */
  unsigned int sps2UScreenGetDrawBuff2();

  /**
   * Returns the first page number of the current buffer that is being drawn
   * to.
   */
  unsigned int sps2UScreenGetDrawBuffCurrent();

  /**
   * Returns the width of the current screen resolution.
   * Note, the current implementation of sps2UScreen always sets a width of
   * 640 pixels.
   */
  unsigned int sps2UScreenGetWidth();

  /**
   * Returns the height of the current screen resolution.
   * Note, the current implementation of sps2UScreen always sets a height of:
   * 480 for sps2UScreenGetVideoMode() == SPS2U_SCR_VESA
   * 448 for sps2UScreenGetVideoMode() == SPS2U_SCR_NTSC
   * 512 for sps2UScreenGetVideoMode() == SPS2U_SCR_PAL
   */
  unsigned int sps2UScreenGetHeight();

  /**
   * Returns the Z depth of the screen configuration (i.e. the largest possible
   * Z value).
   * Note, since this is currently always 24 bit, this function will always
   * return 0xffffff.
   */
  unsigned int sps2UScreenGetZDepth();

  /**
   * Returns the video mode that has been auto detected.  The choices are:
   *
   * SPS2U_SCR_VESA
   * SPS2U_SCR_NTSC
   * SPS2U_SCR_PAL
   */
  unsigned int sps2UScreenGetVideoMode();
  
  /**
   * Returns the pixel format of the current display settings.
   * Note, the current implementation of sps2UScreen always returns a
   * pixel format of 0 which corresponds to PSMCT32.
   */
  unsigned int sps2UScreenGetPixelFormat();

  /**
   * The following sets of functions are non-inline versions of their
   * counterparts in sps2lib.h.  For documentation of any of the specific
   * functions, please see sps2lib.h or refer to the manual.
   * These functions were created to assist those who want to use sps2
   * outside of a C programming environment.  For example, an assembly
   * program could call sps2NIInit() and link to libsps2util.a though it
   * would not have been able to expand the inline version defined in
   * sps2lib.h
   */
  void *_sps2UNIMapEERegisters(void *pvWhere, int iDeviceHandle);
  void *_sps2UNIMapGSRegisters(void *pvWhere, int iDeviceHandle);
  void *_sps2UNIMapVUMemory(void *pvWhere, int iDeviceHandle);
  void *_sps2UNIMapScratchPad(void *pvWhere, int iDeviceHandle);
  int sps2UNIInit();
  void sps2UNIRelease(int iDevice);
  int _sps2UNIOpen();
  void _sps2UNIClose(int iDeviceHandle);
  int sps2UNIFlushCache(int iDeviceHandle);
  int _sps2UNIEnableCOP2Access(int iDeviceHandle);
  unsigned long sps2UNIGetPhysicalAddress(void *pvAddress,
					  sps2Memory_t *pDescriptor);
  sps2Memory_t *sps2UNIAllocate(unsigned long ulSize,
				int iMapOptions,
				int iDeviceHandle);
  sps2Memory_t *sps2UNIRemap(sps2Memory_t *pOriginalArea,
			     int iMapOptions,
			     int iDeviceHandle);
  void sps2UNIFree(sps2Memory_t *pMapping);
  int sps2UNIWaitForDMA(int iChannel,
			int iDeviceHandle);
  int sps2UNIValidateDMA(int iDeviceHandle, int iChannel,
			 sps2uint32 ui32CHCR, int iSizeLimit);
#ifdef __cplusplus
}
#endif

#endif
