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
*/
#ifndef __SPS2_INTERFACE_H
#define __SPS2_INTERFACE_H

#include "sps2registers.h"

#define SPS2_LIB_VERSION_MAJOR 0
#define SPS2_LIB_VERSION_MINOR 3
#define SPS2_LIB_VERSION_REVISION 0

#define SPS2_MAP_MEMORY                0x00000000
#define SPS2_MAP_EE_REGISTERS          0x10000000
#define SPS2_MAP_EE_REGISTERS_SIZE     0x00010000
#define SPS2_MAP_VU_MEMORY             0x11000000
#define SPS2_MAP_VU_MEMORY_SIZE        0x00010000
#define SPS2_MAP_GS_REGISTERS          0x12000000
#define SPS2_MAP_GS_REGISTERS_SIZE     0x00002000
#define SPS2_MAP_SCRATCH_PAD           0x80000000
#define SPS2_MAP_SCRATCH_PAD_SIZE      0x00004000

#define SPS2_MAP_BLOCK_4K              0x0000

#define SPS2_MAP_UNCACHED              0x0008
#define SPS2_MAP_CACHED                0x0000

#define SPS2_MAP_BLOCK_SIZE(x) (1 << (12+(x & 0x0007)))

// IOCTL Commands possible with SPS2 lib
#define SPS2_IOCTL_PHYSICAL_ADDRESS             0
#define SPS2_IOCTL_FLUSH_CACHE                  1
#define SPS2_IOCTL_WAIT_FOR_DMA                 2
#define SPS2_IOCTL_QUERY_SUPPORTED_VERSION      3
#define SPS2_IOCTL_VALIDATE_DMA                 4
#define SPS2_IOCTL_ENABLE_COP2_ACCESS           5

// Errors that can happen with the DMA transfer
// The error mask masks out what type of error occurred
#define SPS2_DMA_ERROR_ERROR_MASK              0x00ff
// The location mask masks out where the error occurred
#define SPS2_DMA_ERROR_LOCATION_MASK           0xff00

// Pointer wasn't aligned on a qword (16 byte) boundary
#define SPS2_DMA_ERROR_UNALIGNED_POINTER       0x0001
// Tried to access a physical address not mapped by process
#define SPS2_DMA_ERROR_NOT_YOUR_MEMORY         0x0002
// Access crosses allocation boundary
#define SPS2_DMA_ERROR_CROSS_BOUNDARY          0x0003
// Chain exceeded maximum size or was not the expected size
#define SPS2_DMA_ERROR_WRONG_CHAIN_SIZE        0x0004
// A pointer was too large for it's source/destination
#define SPS2_DMA_ERROR_POINTER_OUT_OF_RANGE    0x0005
// An invalid DMA transfer mode was selected for the channel
#define SPS2_DMA_ERROR_INVALID_TRANSFER_MODE   0x0006
// An invalid ID was specified in a TAG for source/destination
// chain mode
#define SPS2_DMA_ERROR_INVALID_TAG_ID          0x0007
// Used the SPR when it wasn't available (i.e channels 8-9)
#define SPS2_DMA_ERROR_INVALID_SPR_USE         0x0008
// Issued a CALL tag when there were two calls on the stack
// already
#define SPS2_DMA_ERROR_ADDRESS_STACK_OVERFLOW  0x0009
// Issued a RET tag when there were no calls on the stack
#define SPS2_DMA_ERROR_ADDRESS_STACK_UNDERFLOW 0x000A
// Issued a call on a channel other than 0-2 which has no
// address stack
#define SPS2_DMA_ERROR_NO_ADDRESS_STACK        0x000B

// Defines the possible locations of the errors

// Problem was with the TADR register
#define SPS2_DMA_ERROR_BAD_TADR                0x0100
// Problem was with the MADR register
#define SPS2_DMA_ERROR_BAD_MADR                0x0200
// Problem was with the SADR register
#define SPS2_DMA_ERROR_BAD_SADR                0x0300
// Problem was with the ADDR value in the tag
#define SPS2_DMA_ERROR_BAD_TAG_ADDR            0x0400
// Problem was with the ID value in the tag
#define SPS2_DMA_ERROR_BAD_TAG_ID              0x0500
// Problem was with the CHCR register
#define SPS2_DMA_ERROR_BAD_CHCR                0x0600
// Problem was with the ASR0 register
#define SPS2_DMA_ERROR_BAD_ASR0                0x0700
// Problem was with the ASR1 register
#define SPS2_DMA_ERROR_BAD_ASR1                0x0800

typedef struct {
  void *pvVirtualStart;
  int iBufferSize;
  unsigned long *pulPhysicalPageList;
} sps2PhysicalAddressInfo;

typedef struct {
  // Populated by caller
  int iChannel;
  sps2uint32 ui32CHCR;
  int iSizeLimit;

  // Populated by DMA checker
  int iBytesTransferred;
  int iChainLinks;
  int iError;
  int iErrorLocation;
} sps2ValidateDMAInfo;

/**
 * Note:
 * SPS2 version numbers are in the form of:
 * major.minor.revision
 * but compatability must be the same for major.minor
 * This means that if the revision changes, the interface
 * CANNOT change and must remain compatible across all
 * revisions of a major, minor version.
 */
typedef union {
  unsigned long ulVersion;

  struct {
    unsigned int uiWillShare : 1;
    unsigned short usMajor : 15;
    unsigned short usMinor;
  } v;
} sps2VersionInfo;



#endif
