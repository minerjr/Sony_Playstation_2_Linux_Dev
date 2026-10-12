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
#ifndef __SPS2_SCRATCHPAD_H
#define __SPS2_SCRATCHPAD_H


#ifndef __KERNEL__
/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                         SCRATCH PAD MEMORY                      *
 *                          USER SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *******************************************************************/

#define SPS2_SCRATCH_PAD_START 0x00040000

#define SCRATCH_PAD                 ((sps2uint32 *) 0x00040000)
#define SCRATCH_PAD_OFF(x)          ((sps2uint32 *) ((sps2uint32) x)

#else //__KERNEL__
/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                         SCRATCH PAD MEMORY                      *
 *                        KERNEL SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *******************************************************************/

#define SPS2_SCRATCH_PAD_START 0x80000000

#define SCRATCH_PAD                 ((sps2uint32 *) 0x80000000)
#define SCRATCH_PAD_OFF(x)          ((sps2uint32 *) ((sps2uint32) x)


#endif //__KERNEL__

#endif
