/*
    Copyright (C) 2002 Terratron Technologies Inc.  All Rights Reserved.
    Author: Steven Osman
 
    This file is part of sps2.
  
    For more information visit:
    http://window.terratron.com/~sosman/ps2linux/
    http://www.terratron.com/
*/
#ifndef __SPS2_TYPES_H
#define __SPS2_TYPES_H

#include "sps2types.h"
  
#ifdef __cplusplus
extern "C" {
#endif

  typedef int sps2int32;
  typedef unsigned int sps2uint32;
  typedef long long sps2int64;
  typedef unsigned long long sps2uint64;
  typedef int sps2int128
          __attribute__ (( mode(TI), aligned(16) ));
  typedef unsigned int sps2uint128
          __attribute__ (( mode(TI), aligned(16) ));

#ifdef __cplusplus
}
#endif

#endif
