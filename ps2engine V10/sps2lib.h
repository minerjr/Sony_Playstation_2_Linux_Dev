/*
    Copyright (C) 2002 Terratron Technologies Inc.  All Rights Reserved.
    Author: Steven Osman
 
    This file is part of sps2.

    Before using this file you MUST agree to the license agreement in
    the file LICENSE provided with this package.
  
    For more information visit:
    http://www.playstation2-linux.com/projects/sps2/
    http://window.terratron.com/~sosman/ps2linux/
    http://www.terratron.com/

    6/26/2003 - Sauce
    Made _sps2FixedMapError and sps2ValidateDMA both static.  Thanks to jbit.
*/

#ifndef __SPS2LIB_H
#define __SPS2LIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
  
#include "sps2interface.h"
#include "sps2registers.h"
#include "sps2vumemory.h"
#include "sps2scratchpad.h"
  
  typedef struct {
    void *pvStart;
    void *pvEnd;
    unsigned long ulSize;
    int iBlockCount;
    int iBlockSize;
    int iBlockSlack;
    int iBlockOrder;
    unsigned long *pulPhysical;
  } sps2Memory_t;
  
#define SPS2_PAGE_SIZE 4096
#define SPS2_PAGE_SHIFT 12
#define SPS2_PAGE_SLACK 4095

  static inline void _sps2FixedMapError(const char *pcType, void *pvLocation) {
    fprintf(stderr,"sps2 library FATAL ERROR: Can't map %s to %p\n" 
	    "Please link your program so that 0x00010000-0x0004ffff is "
	    "free\nChecking /proc/%i/maps before terminating this program"
	    " may help\nPress enter to exit...\n",
	    pcType, pvLocation, getpid());
    fflush(stderr);
    getchar();
    exit(-1);
  }

  static inline void *_sps2MapEERegisters(void *pvWhere, int iDeviceHandle);
  static inline void *_sps2MapGSRegisters(void *pvWhere, int iDeviceHandle);
  static inline void *_sps2MapVUMemory(void *pvWhere, int iDeviceHandle);
  static inline void *_sps2MapScratchPad(void *pvWhere, int iDeviceHandle);
  static inline int _sps2EnableCOP2Access(int iDeviceHandle);

  /**
   * This needs to be the very first call made after the device has been
   * opened.  It is used to ensure that the module is aware of the
   * version desired by the software.  If the module is unable to
   * work with the version needed by the software, this call will
   * return a negative value.  On success, this function will return a
   * file descriptor.  This descriptor will either be iDeviceHandle
   * or another file descriptor if the module has been previously
   * opened and is being used with the same version.  In that case, the
   * caller may want to use the dup2() command to replace her current
   * descriptor by the one returned so as to avoid multiple copies
   * of the module state.  Note that this case only applies if both
   * the current caller and the caller of the previous file descriptor
   * agree to share.
   */
  static inline int _sps2CheckModuleVersion(int iDeviceHandle,
					   int iShare) {
    sps2VersionInfo vDesiredVersion;
    vDesiredVersion.v.uiWillShare=iShare;
    vDesiredVersion.v.usMajor=SPS2_LIB_VERSION_MAJOR;
    vDesiredVersion.v.usMinor=SPS2_LIB_VERSION_MINOR;

    return (ioctl(iDeviceHandle, SPS2_IOCTL_QUERY_SUPPORTED_VERSION,
		  vDesiredVersion.ulVersion));
  }

  /**
   * Initializes the sps2 library.
   * On success, it returns the handle for the device
   * On failure, it returns <0 and errno contains the error.
   * Note that if your linker settings do not have 0x10000000-0x12002000 free,
   * this bit of code will abort your program.
   */
  static inline int sps2Init() {
    int iDevice=open("/dev/sps2",O_RDWR,0);
    int iVersionCheck;

    if (iDevice<0) {
      return iDevice;
    }

    // See if it is the correct version
    iVersionCheck=_sps2CheckModuleVersion(iDevice,1);

    if (iVersionCheck<0) {
      fprintf(stderr,
	      "sps2 library FATAL ERROR: The kernel module cannot support "
	      "version %i.%i\n", SPS2_LIB_VERSION_MAJOR,
	      SPS2_LIB_VERSION_MINOR);
      close(iDevice);
      return iVersionCheck;
    }

    // See if it is already opened
    if (iVersionCheck!=iDevice) {
      // It has already been opened.  Just clone the descriptor and
      // finish.
      dup2(iVersionCheck,iDevice);

      return iDevice;
    }

    // Map the regions for the EE and GS registers as well as the
    // VU memory and the scratch pad memory

    if (_sps2MapEERegisters((void *) SPS2_EE_REGISTERS_START, iDevice)!=
	(void *) SPS2_EE_REGISTERS_START) {
      _sps2FixedMapError("EE registers", (void *) SPS2_EE_REGISTERS_START);
    }

    if (_sps2MapGSRegisters((void *) SPS2_GS_REGISTERS_START, iDevice)!=
	(void *) SPS2_GS_REGISTERS_START) {
      _sps2FixedMapError("GS registers", (void *) SPS2_GS_REGISTERS_START);
    }

    if (_sps2MapScratchPad((void *) SPS2_SCRATCH_PAD_START, iDevice)!=
	(void *) SPS2_SCRATCH_PAD_START) {
      _sps2FixedMapError("scratch pad memory", (void *) SPS2_SCRATCH_PAD_START);
    }

    if (_sps2MapVUMemory((void *) SPS2_VU_MEMORY_START, iDevice)!=
	(void *) SPS2_VU_MEMORY_START) {
      _sps2FixedMapError("VU memory", (void *) SPS2_VU_MEMORY_START);
    }

    if (_sps2EnableCOP2Access(iDevice)) {
      fprintf(stderr, "sps2 library FATAL ERROR: Cannot enable cop2 access.");
      close(iDevice);
      return -1;
    }

    return iDevice;
  }

  /**
   * Releases the device -- the opposite of sps2Init.
   * The difference between this and sps2Close is that this function
   * maintains a reference counter and actually closes the device
   * only if the reference counter has reached zero.
   */
  static inline void sps2Release(int iDevice) {
    close(iDevice);
  }
  
  /**
   * Opens the sps2 device.
   * On success, it returns the handle for the device.
   * On failure, it returns < 0 and errno contains the error.
   */
  static inline int _sps2Open() {
    int iVersionCheck;
    int iDevice=open("/dev/sps2",O_RDWR,0);

    if (iDevice>=0) {
      // See if it is the correct version
      iVersionCheck=_sps2CheckModuleVersion(iDevice,1);

      if (iVersionCheck<0) {
	close(iDevice);
	return iVersionCheck;
      }

      // See if it is already opened
      if (iVersionCheck!=iDevice) {
	// It has already been opened.  Just clone the descriptor and
	// finish.
	dup2(iVersionCheck,iDevice);
      }
    }

    return iDevice;
  }
  
  /**
   * Close the sps2 device.
   * iDeviceHandle is the handle returned by sps2Open();
   */
  static inline void _sps2Close(int iDeviceHandle) {
    if (iDeviceHandle>=0) {
      close(iDeviceHandle);
    }
  }
  
  /**
   * Request all caches flushed.
   * Returns 0 on success, non-zero on failure with errno pointing
   * to error.
   * iDeviceHandle is the handle returned by sps2Open();
   */
  static inline int sps2FlushCache(int iDeviceHandle) {
    if (iDeviceHandle<0) {
      return -EINVAL;
    }
    
    return ioctl(iDeviceHandle,SPS2_IOCTL_FLUSH_CACHE,0);
  }
  
  /**
   * Map a virtual address to a physical one.
   */
  static inline unsigned long sps2GetPhysicalAddress(void *pvAddress,
					      sps2Memory_t *pDescriptor) {
    unsigned long ulOffset;

    if (pvAddress>pDescriptor->pvEnd ||
	pvAddress<pDescriptor->pvStart) {
      fprintf(stderr,"Virtual address passed out of range %08x\n",
	      (unsigned int) pvAddress);
      exit(-1);
    }

    ulOffset=(unsigned long) pvAddress-
      (unsigned long)pDescriptor->pvStart;

    return (ulOffset & pDescriptor->iBlockSlack) +
      pDescriptor->pulPhysical[ulOffset / pDescriptor->iBlockSize];
  }
  
  /**
   * Allocate some memory.
   * ulSize is the desired memory size, in bytes
   * iMapOptions are the options for the memory size.
   *
   * Contiguous block size:
   *       SPS2_MAP_BLOCK_4K map in 4096 byte contiguous chunks (default)
   *
   * Cache control:
   *       SPS2_MAP_CACHED memory access is cached (default)
   *       SPS2_MAP_UNCACHED memory is uncached
   *
   * Important #1: This memory is not contiguous in physical space!  Only
   *  chunks of block-sized bytes are contiguous.  For example, if allocation
   *  is done with blocks of 4K, only data within the same 4K chunk is
   *  guaranteed contiguous.  Byte 4097 may not be in the same area as
   *  byte 4095.  (though 0-4095 and 4096-8191, etc. will be contigous).
   */
  static inline sps2Memory_t *sps2Allocate(unsigned long ulSize,
					   int iMapOptions,
					   int iDeviceHandle) {
    sps2Memory_t *pDescriptor;
    sps2PhysicalAddressInfo mapping;
    void *pvStart;
    int iBlockCount;
    int iBlockSize;
    int iBlockOrder;

    if (iDeviceHandle<0) {
      errno=-EINVAL;
      return (sps2Memory_t *) -1;
    }

    // we only have 8 bits maximum of map options
    if (iMapOptions & (~0xff)) {
      errno=-EINVAL;
      return (sps2Memory_t *) -1;
    }

    iBlockOrder=iMapOptions & 7;

    if (iBlockOrder > 0 || iBlockOrder<0) {
      errno=-EINVAL;
      return (sps2Memory_t *) -1;
    }

    iBlockSize=1 << (SPS2_PAGE_SHIFT+iBlockOrder);

    while (iBlockOrder>0 && ulSize<=((unsigned int) iBlockSize >> 1)) {
      iBlockSize>>=1;
      iBlockOrder--;
    }

    iMapOptions &= ~7;
    iMapOptions |= iBlockOrder;

    if (ulSize & (iBlockSize-1)) {
      ulSize+=iBlockSize-1;
      ulSize&=~(iBlockSize-1);
    }
    
    iBlockCount=ulSize >> (SPS2_PAGE_SHIFT+iBlockOrder);
    
    pvStart=mmap((void *) 0, ulSize, PROT_READ | PROT_WRITE,
		 MAP_PRIVATE, iDeviceHandle,
		 SPS2_MAP_MEMORY | iMapOptions);
    
    if (pvStart==(void *) -1) {
      return (sps2Memory_t *) -1;
    }
    
    pDescriptor=(sps2Memory_t *) malloc(sizeof(sps2Memory_t));
    
    if (!pDescriptor) {
      munmap(pvStart, ulSize);
      errno=-ENOMEM;
      return (sps2Memory_t *) -1;
    }
    
    pDescriptor->pvStart=pvStart;
    pDescriptor->ulSize=ulSize;
    pDescriptor->pvEnd=(void *) ((unsigned long) pvStart+ulSize);
    pDescriptor->iBlockCount=iBlockCount;
    pDescriptor->iBlockSize=iBlockSize;
    pDescriptor->iBlockSlack=iBlockSize-1;
    pDescriptor->iBlockOrder=iBlockOrder;
    
    pDescriptor->pulPhysical=
      (unsigned long *) malloc(iBlockCount * sizeof(unsigned long));
    
    if (!pDescriptor->pulPhysical) {
      free(pDescriptor);
      munmap(pvStart, ulSize);
      errno=-ENOMEM;
      return (sps2Memory_t *) -1;
    }
    
    mapping.pvVirtualStart=pvStart;
    mapping.iBufferSize=sizeof(unsigned long) * iBlockCount;
    mapping.pulPhysicalPageList=pDescriptor->pulPhysical;
    
    if (ioctl(iDeviceHandle, SPS2_IOCTL_PHYSICAL_ADDRESS, &mapping)) {
      free(pDescriptor->pulPhysical);
      free(pDescriptor);
      munmap(pvStart, ulSize);
      return (sps2Memory_t *) -1;
    }
    
    return pDescriptor;
  }

  /**
   * This function can take a section of memory and remap it.
   * Typically this will be done if the caller wants to have an uncached
   * equivalent to a cached memory area.
   * pOriginalArea is the area to remap.  The original area is still valud
   * iMapOptions are the new map options to use.  Do not attempt to
   *     change the block allocation size -- it will be ignored.
   * iDeviceHandle is the handle returned by sps2Open()
   * Returns a memory descriptor or -1 if failed, with errno being the error
   */
  static inline sps2Memory_t *sps2Remap(sps2Memory_t *pOriginalArea,
					int iMapOptions,
					int iDeviceHandle) {

    void *pvStart;
    sps2Memory_t *pDescriptor;

    // we only have 8 bits maximum of map options
    if (iMapOptions & (~0xff)) {
      errno=-EINVAL;
      return (sps2Memory_t *) -1;
    }

    if (!pOriginalArea) {
      errno=-EINVAL;
      return (sps2Memory_t *) -1;
    }

    // Take block order from original area
    iMapOptions &= ~7;
    iMapOptions |= pOriginalArea->iBlockOrder;
    
    // Try to remap
    pvStart=mmap((void *) 0, pOriginalArea->ulSize, PROT_READ | PROT_WRITE,
		 MAP_PRIVATE, iDeviceHandle,
		 *pOriginalArea->pulPhysical | iMapOptions);
    
    if (pvStart==(void *) -1) {
      return (sps2Memory_t *) -1;
    }

    // Allocate new descriptor
    pDescriptor=(sps2Memory_t *) malloc(sizeof(sps2Memory_t));

    if (!pDescriptor) {
      munmap(pvStart, pOriginalArea->ulSize);
      errno=-ENOMEM;
      return (sps2Memory_t *) -1;
    }
      
    memcpy(pDescriptor, pOriginalArea, sizeof(sps2Memory_t));

    pDescriptor->pvStart=pvStart;
    pDescriptor->pvEnd=(void *) 
      ((unsigned long) pvStart+pOriginalArea->ulSize);
    
    pDescriptor->pulPhysical=
      (unsigned long *) malloc(pDescriptor->iBlockCount *
			       sizeof(unsigned long));
    
    if (!pDescriptor->pulPhysical) {
      free(pDescriptor);
      munmap(pvStart, pOriginalArea->ulSize);
      errno=-ENOMEM;
      return (sps2Memory_t *) -1;
    }

    memcpy(pDescriptor->pulPhysical, pOriginalArea->pulPhysical,
	   pDescriptor->iBlockCount * sizeof(unsigned long));

    return pDescriptor;
  }
  
  /**
   * This is used by the various Map functions to map to an existing
   * portion of the EE's memory.
   * pvWhere is mmap's start address -- a suggestion as to where the
   *  memory should be mapped.  If pvWhere is unavailable (or zero)
   *  the region will be mapped elsewhere automatically
   */
  static inline void *_sps2MapExisting(unsigned long ulStart,
				       unsigned long ulLength,
				       void *pvWhere,
				       int iDeviceHandle) {
    if (iDeviceHandle<0) {
      errno=-EINVAL;
      return (unsigned long *) -1;
    }  
    
    return (unsigned long *) mmap(pvWhere, ulLength,
				  PROT_READ | PROT_WRITE,
				  MAP_PRIVATE, iDeviceHandle,
				  ulStart);
  }
  
  /**
   * Map the EE registers to an area of user memory.
   * pvWhere is mmap's start address -- a suggestion as to where the
   *  memory should be mapped.  If pvWhere is unavailable (or zero)
   *  the region will be mapped elsewhere automatically
   */
  static inline void *_sps2MapEERegisters(void *pvWhere,
					  int iDeviceHandle) {
    return _sps2MapExisting(SPS2_MAP_EE_REGISTERS,
			    SPS2_MAP_EE_REGISTERS_SIZE,
			    pvWhere,
			    iDeviceHandle);
  }
  
  /**
   * Map the VU memory to an area of user memory.
   * pvWhere is mmap's start address -- a suggestion as to where the
   *  memory should be mapped.  If pvWhere is unavailable (or zero)
   *  the region will be mapped elsewhere automatically
   */
  static inline void *_sps2MapVUMemory(void *pvWhere,
					  int iDeviceHandle) {
    return _sps2MapExisting(SPS2_MAP_VU_MEMORY,
			    SPS2_MAP_VU_MEMORY_SIZE,
			    pvWhere,
			    iDeviceHandle);
  }
  
/**
 * Map the GS registers to an area of user memory.
   * pvWhere is mmap's start address -- a suggestion as to where the
   *  memory should be mapped.  If pvWhere is unavailable (or zero)
   *  the region will be mapped elsewhere automatically
 */
  static inline void *_sps2MapGSRegisters(void *pvWhere,
					  int iDeviceHandle) {
    return _sps2MapExisting(SPS2_MAP_GS_REGISTERS,
			    SPS2_MAP_GS_REGISTERS_SIZE,
			    pvWhere,
			    iDeviceHandle);
  }
  
  /**
   * Map the scratch pad to an area of user memory.
   * pvWhere is mmap's start address -- a suggestion as to where the
   *  memory should be mapped.  If pvWhere is unavailable (or zero)
   *  the region will be mapped elsewhere automatically
   */
  static inline void *_sps2MapScratchPad(void *pvWhere,
					  int iDeviceHandle) {
    return _sps2MapExisting(SPS2_MAP_SCRATCH_PAD,
			    SPS2_MAP_SCRATCH_PAD_SIZE*2,
			    pvWhere,
			    iDeviceHandle);
  }
  
  /**
   * Free an area allocated with sps2Alloc()
   * Do not use the memory or the structure after calling this function!
   */
  static inline void sps2Free(sps2Memory_t *pMapping) {
    if (pMapping) {
      if (pMapping->pulPhysical) {
	free(pMapping->pulPhysical);
      }
      
      if (pMapping->pvStart) {
	munmap((void *) pMapping->pvStart, pMapping->ulSize);
      }
      
      free(pMapping);
    }
  }
  

  /**
   * Waits for a DMA transfer to complete, yielding to the scheduler.
   * You could just wait for the appropriate str bit to become 0 in a
   * simple while loop, but you'll be sucking up CPU time unnecessarily.
   * iChannel is the DMA channel to wait for (0-9)
   * iDeviceHandle is the handle returned by sps2Open();
   * Returns 0 on success, <0 on failure with errno set to the error.
   */
  static inline int sps2WaitForDMA(int iChannel,
				   int iDeviceHandle) {
    if (iDeviceHandle<0) {
      errno=-EINVAL;
      return -1;
    }

    return ioctl(iDeviceHandle, SPS2_IOCTL_WAIT_FOR_DMA,
		 (unsigned long *) iChannel);
  }

  /**
   * Enables access to the cop2 coprocessor (i.e. VU0).  This allows
   * macro mode instructions, for example.
   * iDeviceHandle is the handle returned by sps2Open();
   * Returns 0 on success, <0 on failure with errno set to the error.
   */
  static inline int _sps2EnableCOP2Access(int iDeviceHandle) {
    if (iDeviceHandle<0) {
      errno=-EINVAL;
      return -1;
    }

    return ioctl(iDeviceHandle, SPS2_IOCTL_ENABLE_COP2_ACCESS, 0);
  }

#define SPS2_DEFAULT_MAX_DMA_SIZE (16*1024*1024)

  /**
   * Validates a DMA chain before the transfer is begun.
   * iChannel is the DMA channel to validate (0-9)
   * iDeviceHandle is the handle returned by sps2Open();
   * iSizeLimit indicates the size/maximum size of the DMA transfer,
   * including tags
   * iSizeLimit = 0 means use the default max size for the test (16MB)
   * iSizeLimit < 0 means that the max size is defined by -iSizeLimit
   * iSizeLimit > 0 means that the *exact* size is defined by iSizeLimit
   * Returns 0 on a good chain, <0 on an error with the test, and >0 on a
   * bad chain
   */
static inline int sps2ValidateDMA(int iDeviceHandle, int iChannel,
				  sps2uint32 ui32CHCR, int iSizeLimit) {
  sps2ValidateDMAInfo info;
  int iReturnValue;

  if (iDeviceHandle<0) {
    return -EINVAL;
  }

  info.iChannel=iChannel;
  info.ui32CHCR=ui32CHCR;
  info.iSizeLimit=iSizeLimit;

  iReturnValue=ioctl(iDeviceHandle, SPS2_IOCTL_VALIDATE_DMA, &info);

  if (iReturnValue) {
    printf("DMA Checker says:\n\tSize\t= %i bytes\n\tLinks\t= %i\n"
	   "\tError\t= %04x\n\tLoc.\t= %04x\n\tReturn\t= %i\n",
	   info.iBytesTransferred, info.iChainLinks,
	   info.iError, info.iErrorLocation,
	   iReturnValue);
  } else {
    printf(".");
    //    printf("DMA OK %i bytes transferred in %i links\n",
    //	   info.iBytesTransferred, info.iChainLinks);
  }

  if (!iReturnValue) {
    return info.iBytesTransferred;
  } else if (iReturnValue>0) {
    return -iReturnValue;
  } else {
    return iReturnValue;
  }
}


#ifdef __cplusplus
}
#endif
       
#endif
