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

    8/22/2003 - Sauce - Added __KERNEL__ section
*/
#ifndef __SPS2_VUMEMORY_H
#define __SPS2_VUMEMORY_H

#ifndef __KERNEL__
/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                            VU MEMORY AREAS                      *
 *                          USER SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *******************************************************************/
#define SPS2_VU_MEMORY_START 0x00030000

/**
 * VU0
 */
#define VU0_MICRO_MEM               ((sps2uint32 *) 0x00030000)
#define VU0_MEM                     ((sps2uint32 *) 0x00034000)

#define VU0_MICRO_MEM_OFF(x)        ((sps2uint32 *) ((sps2uint32) x + 0x0000))
#define VU0_MEM_OFF(x)              ((sps2uint32 *) ((sps2uint32) x + 0x4000))

/**
 * VU1
 */
#define VU1_MICRO_MEM               ((sps2uint32 *) 0x00038000)
#define VU1_MEM                     ((sps2uint32 *) 0x0003c000)

#define VU1_MICRO_MEM_OFF(x)        ((sps2uint32 *) ((sps2uint32) x + 0x8000))
#define VU1_MEM_OFF(x)              ((sps2uint32 *) ((sps2uint32) x + 0xc000))

#else // __KERNEL__

/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                            VU MEMORY AREAS                      *
 *                        KERNEL SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *******************************************************************/
#define SPS2_VU_MEMORY_START 0x11000000

/**
 * VU0
 */
#define VU0_MICRO_MEM               ((sps2uint32 *) 0x11000000)
#define VU0_MEM                     ((sps2uint32 *) 0x11004000)

#define VU0_MICRO_MEM_OFF(x)        ((sps2uint32 *) ((sps2uint32) x + 0x0000))
#define VU0_MEM_OFF(x)              ((sps2uint32 *) ((sps2uint32) x + 0x4000))

/**
 * VU1
 */
#define VU1_MICRO_MEM               ((sps2uint32 *) 0x11008000)
#define VU1_MEM                     ((sps2uint32 *) 0x1100c000)

#define VU1_MICRO_MEM_OFF(x)        ((sps2uint32 *) ((sps2uint32) x + 0x8000))
#define VU1_MEM_OFF(x)              ((sps2uint32 *) ((sps2uint32) x + 0xc000))

#endif // __KERNEL__

#endif

