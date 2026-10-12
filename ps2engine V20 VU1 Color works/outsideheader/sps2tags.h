/*
    Copyright (C) 2002 Terratron Technologies Inc.  All Rights Reserved.
    Author: Steven Osman
 
    This file is part of sps2.

    For more information visit:
    http://window.terratron.com/~sosman/ps2linux/
    http://www.terratron.com/
*/
#ifndef __SPS2_TAGS_H
#define __SPS2_TAGS_H

#include "sps2registers.h"
  
#ifdef __cplusplus
extern "C" {
#endif

#define SPS2_SET_GIF_REG(gifTag, iRegNo, uiValue) \
  {((sps2GIFTag_t *) &gifTag)->ai64[1] |= ((uiValue & 0xf) << (iRegNo << 2));}

  // There is a PSM_ in front of everything so as not to be confused with
  // #defines that may exist in other people's code
  typedef enum {
    PSM_PSMCT32=0,
    PSM_PSMCT24=1,
    PSM_PSMCT16=2,
    PSM_PSMCT16S=10,
    PSM_PSMT8=19,
    PSM_PSMT4=20,
    PSM_PSMT8H=27,
    PSM_PSMT4HL=36,
    PSM_PSMT4HH=44,
    PSM_PSMZ32=48,
    PSM_PSMZ24=49,
    PSM_PSMZ16=50,
    PSM_PSMZ16S=58,
  } PSM_t;

  typedef enum GIF_REG {
    GIF_REG_PRIM      =0x00,
    GIF_REG_RGBAQ     =0x01,
    GIF_REG_ST        =0x02,
    GIF_REG_UV        =0x03,
    GIF_REG_XYZF2     =0x04,
    GIF_REG_XYZ2      =0x05,
    GIF_REG_TEX0_1    =0x06,
    GIF_REG_TEX0_2    =0x07,
    GIF_REG_CLAMP_1   =0x08,
    GIF_REG_CLAMP_2   =0x09,
    GIF_REG_FOG       =0x0a,
    GIF_REG_XYZF3     =0x0c,
    GIF_REG_XYZ3      =0x0d,
    GIF_REG_A_D       =0x0e,
    GIF_REG_NOP       =0x0f,
  } GIF_REG_t;

  typedef enum GIF_A_D_REG {
    GIF_A_D_REG_PRIM      =0x00,
    GIF_A_D_REG_RGBAQ     =0x01,
    GIF_A_D_REG_ST        =0x02,
    GIF_A_D_REG_UV        =0x03,
    GIF_A_D_REG_XYZF2     =0x04,
    GIF_A_D_REG_XYZ2      =0x05,
    GIF_A_D_REG_TEX0_1    =0x06,
    GIF_A_D_REG_TEX0_2    =0x07,
    GIF_A_D_REG_CLAMP_1   =0x08,
    GIF_A_D_REG_CLAMP_2   =0x09,
    GIF_A_D_REG_FOG       =0x0a,
    GIF_A_D_REG_XYZF3     =0x0c,
    GIF_A_D_REG_XYZ3      =0x0d,
    GIF_A_D_REG_NOP       =0x0f,
    GIF_A_D_REG_ALPHA_1   =0x42,
    GIF_A_D_REG_ALPHA_2   =0x43,
    GIF_A_D_REG_BITBLTBUF =0x50,
    GIF_A_D_REG_COLCLAMP  =0x46,
    GIF_A_D_REG_DIMX      =0x44,
    GIF_A_D_REG_DTHE      =0x45,
    GIF_A_D_REG_FBA_1     =0x4a,
    GIF_A_D_REG_FBA_2     =0x4b,
    GIF_A_D_REG_FINISH    =0x61,
    GIF_A_D_REG_FOGCOL    =0x3d,
    GIF_A_D_REG_FRAME_1   =0x4c,
    GIF_A_D_REG_FRAME_2   =0x4d,
    GIF_A_D_REG_HWREG     =0x54,
    GIF_A_D_REG_LABEL     =0x62,
    GIF_A_D_REG_MIPTBP1_1 =0x34,
    GIF_A_D_REG_MIPTBP1_2 =0x35,
    GIF_A_D_REG_MIPTBP2_1 =0x36,
    GIF_A_D_REG_MIPTBP2_2 =0x37,
    GIF_A_D_REG_PABE      =0x49,
    GIF_A_D_REG_PRMODE    =0x1b,
    GIF_A_D_REG_PRMODECONT=0x1a,
    GIF_A_D_REG_SCANMSK   =0x22,
    GIF_A_D_REG_SCISSOR_1 =0x40,
    GIF_A_D_REG_SCISSOR_2 =0x41,
    GIF_A_D_REG_SIGNAL    =0x60,
    GIF_A_D_REG_TEST_1    =0x47,
    GIF_A_D_REG_TEST_2    =0x48,
    GIF_A_D_REG_TEX1_1    =0x14,
    GIF_A_D_REG_TEX1_2    =0x15,
    GIF_A_D_REG_TEX2_1    =0x16,
    GIF_A_D_REG_TEX2_2    =0x17,
    GIF_A_D_REG_TEXA      =0x3b,
    GIF_A_D_REG_TEXCLUT   =0x1c,
    GIF_A_D_REG_TEXFLUSH  =0x3f,
    GIF_A_D_REG_TRXDIR    =0x53,
    GIF_A_D_REG_TRXPOS    =0x51,
    GIF_A_D_REG_TRXREG    =0x52,
    GIF_A_D_REG_XYOFFSET_1=0x18,
    GIF_A_D_REG_XYOFFSET_2=0x19,
    GIF_A_D_REG_ZBUF_1    =0x4e,
    GIF_A_D_REG_ZBUF_2    =0x4f,
  } GIF_A_D_REG_t;

  typedef enum GIF_FLG {
    GIF_FLG_PACKED    =0x0,
    GIF_FLG_REGLIST   =0x1,
    GIF_FLG_IMAGE     =0x2,
  } GIF_FLG_t;

  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int NLOOP : 15;
      unsigned int EOP   : 1;
      unsigned int _PAD1 : 16;
      unsigned int _PAD2 : 14;
      unsigned int PRE   : 1;
      unsigned int PRIM  : 11;
      GIF_FLG_t    FLG   : 2;
      unsigned int NREG  : 4;
      sps2uint64 REGS  : 64;
    } s;

    sps2uint64 ai64[2];
  } sps2GIFTag_t;

  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int PRIM: 11;
      unsigned int _PAD1: 21;
      unsigned int _PAD2: 32;
      unsigned int _PAD3: 32;
      unsigned int _PAD4: 32;
    } s;
  } sps2GIFPackedPRIM_t;

  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int R: 8;
      unsigned int _PAD1: 24;
      unsigned int G: 8;
      unsigned int _PAD2: 24;
      unsigned int B: 8;
      unsigned int _PAD3: 24;
      unsigned int A: 8;
      unsigned int _PAD4: 24;
    } s;
  } sps2GIFPackedRGBAQ_t;

  typedef union {
    sps2uint128 i128;

    struct {
      float S;
      float T;
      float Q;
      unsigned int _PAD1: 32;
    } s;
  } sps2GIFPackedST_t;

  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int U: 14;
      unsigned int _PAD1: 18;
      unsigned int V: 14;
      unsigned int _PAD2: 18;
      unsigned int _PAD3: 32;
      unsigned int _PAD4: 32;
    } s;
  } sps2GIFPackedUV_t;

  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int X: 16;
      unsigned int _PAD1: 16;
      unsigned int Y: 16;
      unsigned int _PAD2: 16;
      unsigned int _PAD3: 4;
      unsigned int Z: 24;
      unsigned int _PAD4: 4;
      unsigned int _PAD5: 4;
      unsigned int F: 8;
      unsigned int _PAD6: 3;
      unsigned int ADC: 1;
      unsigned int _PAD7: 16;
    } s;
  } sps2GIFPackedXYZF2_t;

  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int X: 16;
      unsigned int _PAD1: 16;
      unsigned int Y: 16;
      unsigned int _PAD2: 16;
      unsigned int Z: 32;
      unsigned int _PAD3: 15;
      unsigned int ADC: 1;
      unsigned int _PAD4: 16;
    } s;
  } sps2GIFPackedXYZ2_t;
  
  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int _PAD1: 32;
      unsigned int _PAD2: 32;
      unsigned int _PAD3: 32;
      unsigned int _PAD4: 4;
      unsigned int F: 8;
      unsigned int _PAD5: 20;
    } s;
  } sps2GIFPackedFOG_t;
  
  typedef union {
    sps2uint128 i128;

    struct {
      sps2uint64 DATA: 64;
      GIF_A_D_REG_t ADDR: 8;
      unsigned int _PAD1: 24;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFPackedA_D_t;

  typedef union {
    sps2uint128 i128;

    struct {
      unsigned int _PAD1: 32;
      unsigned int _PAD2: 32;
      unsigned int _PAD3: 32;
      unsigned int _PAD4: 32;
    } s;
  } sps2GIFPackedNOP_t;

  typedef union {
    sps2uint128          i128;
    sps2uint64           ai64[2];
    sps2uint32           ai32[4];
    sps2GIFPackedPRIM_t  PRIM;
    sps2GIFPackedRGBAQ_t RGBAQ;
    sps2GIFPackedST_t    ST;
    sps2GIFPackedUV_t    UV;
    sps2GIFPackedXYZF2_t XYZF2;
    sps2GIFPackedXYZ2_t  XYZ2;
    sps2GIFPackedFOG_t   FOG;
    sps2GIFPackedA_D_t   A_D;
    sps2GIFPackedNOP_t   NOP;
  } sps2GIFPackedRegister_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int A: 2;
      unsigned int B: 2;
      unsigned int C: 2;
      unsigned int D: 2;
      unsigned int _PAD1: 24;
      unsigned int FIX: 8;
      unsigned int _PAD2: 24;      
    } s;
  } sps2GIFRegALPHA_n_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int SBP: 14;
      unsigned int _PAD1: 2;
      unsigned int SBW: 6;
      unsigned int _PAD2: 2;
      unsigned int SPSM : 6;
      unsigned int _PAD3 : 2;
      unsigned int DBP : 14;
      unsigned int _PAD4 : 2;
      unsigned int DBW : 6;
      unsigned int _PAD5 : 2;
      unsigned int DPSM : 6;
      unsigned int _PAD6 : 2;
    } s;
  } sps2GIFRegBITBLTBUF_t;

  // These are sps2uint64 because one of the values crosses bits 31,32
  typedef union {
    sps2uint64 i64;
    struct {
      sps2uint64 WMS : 2;
      sps2uint64 WMT : 2;
      sps2uint64 MINU : 10;
      sps2uint64 MAXU : 10;
      sps2uint64 MINV : 10;
      sps2uint64 MAXV : 10;
      sps2uint64 _PAD : 20;
    } s;
  } sps2GIFRegCLAMP_n_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int CLAMP: 1;
      unsigned int _PAD1: 31;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegCOLCLAMP_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int DM00: 3;
      unsigned int _PAD00: 1;
      unsigned int DM01: 3;
      unsigned int _PAD01: 1;
      unsigned int DM02: 3;
      unsigned int _PAD02: 1;
      unsigned int DM03: 3;
      unsigned int _PAD03: 1;

      unsigned int DM10: 3;
      unsigned int _PAD10: 1;
      unsigned int DM11: 3;
      unsigned int _PAD11: 1;
      unsigned int DM12: 3;
      unsigned int _PAD12: 1;
      unsigned int DM13: 3;
      unsigned int _PAD13: 1;

      unsigned int DM20: 3;
      unsigned int _PAD20: 1;
      unsigned int DM21: 3;
      unsigned int _PAD21: 1;
      unsigned int DM22: 3;
      unsigned int _PAD22: 1;
      unsigned int DM23: 3;
      unsigned int _PAD23: 1;

      unsigned int DM30: 3;
      unsigned int _PAD30: 1;
      unsigned int DM31: 3;
      unsigned int _PAD31: 1;
      unsigned int DM32: 3;
      unsigned int _PAD32: 1;
      unsigned int DM33: 3;
      unsigned int _PAD33: 1;
    } s;
  } sps2GIFRegDIMX_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int DTHE: 1;
      unsigned int _PAD1: 31;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegDTHE_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int FBA: 1;
      unsigned int _PAD1: 31;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegFBA_n_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int _PAD1: 32;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegFINISH_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int _PAD1 : 32;
      unsigned int _PAD2 : 24;
      unsigned int F : 8;
    } s;
  } sps2GIFRegFOG_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int FCR: 8;
      unsigned int FCG: 8;
      unsigned int FCB: 8;
      unsigned int _PAD1: 8;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegFOGCOL_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int FBP: 9;
      unsigned int _PAD1: 7;
      unsigned int FBW: 6;
      unsigned int _PAD2: 2;
      unsigned int PSM: 6;
      unsigned int _PAD3: 2;
      unsigned int FBMSK: 32;
    } s;
  } sps2GIFRegFRAME_n_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int DATA_LOWER: 32;
      unsigned int DATA_UPPER: 32;
    } s;
  } sps2GIFRegHWREG_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int ID: 32;
      unsigned int IDMSK: 32;
    } s;
  } sps2GIFRegLABEL_t;

  // These are sps2uint64 because one of the values crosses bits 31,32
  typedef union {
    sps2uint64 i64;

    struct {
      sps2uint64 TBP1: 14;
      sps2uint64 TBW1: 6;
      sps2uint64 TBP2: 14;
      sps2uint64 TBW2: 6;
      sps2uint64 TBP3: 14;
      sps2uint64 TBW3: 6;
      sps2uint64 _PAD: 4;
    } s;
  } sps2GIFRegMIPTBP1_n_t;

  // These are sps2uint64 because one of the values crosses bits 31,32
  typedef union {
    sps2uint64 i64;

    struct {
      sps2uint64 TBP4: 14;
      sps2uint64 TBW4: 6;
      sps2uint64 TBP5: 14;
      sps2uint64 TBW5: 6;
      sps2uint64 TBP6: 14;
      sps2uint64 TBW6: 6;
      sps2uint64 _PAD: 4;
    } s;
  } sps2GIFRegMIPTBP2_n_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int _PAD1 : 32;
      unsigned int _PAD2 : 32;
    } s;
  } sps2GIFRegNOP_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int PABE: 1;
      unsigned int _PAD1: 31;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegPABE_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int PRIM : 3;
      unsigned int IIP : 1;
      unsigned int TME : 1;
      unsigned int FGE : 1;
      unsigned int ABE : 1;
      unsigned int AA1 : 1;
      unsigned int FST : 1;
      unsigned int CTXT : 1;
      unsigned int FIX : 1;
      unsigned int _PAD1 : 21;
      unsigned int _PAD2 : 32;
    } s;
  } sps2GIFRegPRIM_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int _PAD1: 3;
      unsigned int IIP : 1;
      unsigned int TME : 1;
      unsigned int FGE : 1;
      unsigned int ABE : 1;
      unsigned int AA1 : 1;
      unsigned int FST : 1;
      unsigned int CTXT : 1;
      unsigned int FIX : 1;
      unsigned int _PAD2 : 21;
      unsigned int _PAD3 : 32;
    } s;
  } sps2GIFRegPRMODE_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int AC: 1;
      unsigned int _PAD1: 31;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegPRMODECONT_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int R: 8;
      unsigned int G: 8;
      unsigned int B: 8;
      unsigned int A: 8;
      float Q;
    } s;
  } sps2GIFRegRGBAQ_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int MSK: 2;
      unsigned int _PAD1: 30;
      unsigned int _PAD2: 32;
    } s;
  } sps2GIFRegSCANMSK_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int SCAX0: 11;
      unsigned int _PAD1: 5;
      unsigned int SCAX1: 11;
      unsigned int _PAD2: 5;
      unsigned int SCAY0: 11;
      unsigned int _PAD3: 5;
      unsigned int SCAY1: 11;
      unsigned int _PAD4: 5;
    } s;
  } sps2GIFRegSCISSOR_n_t;

  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int ID: 32;
      unsigned int IDMSK: 32;
    } s;
  } sps2GIFRegSIGNAL_t;

  typedef union {
    sps2uint64 i64;
    struct {
      float S;
      float T;
    } s;
  } sps2GIFRegST_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int ATE : 1;
      unsigned int ATST : 3;
      unsigned int AREF : 8;
      unsigned int AFAIL : 2;
      unsigned int DATE : 1;
      unsigned int DATM : 1;
      unsigned int ZTE : 1;
      unsigned int ZTST : 2;
      unsigned int _PAD1 : 13;
      unsigned int _PAD2 : 32;
    } s;
  } sps2GIFRegTEST_n_t;

  // These are sps2uint64 because one of the values crosses bits 31,32
  typedef union {
    sps2uint64 i64;

    struct {
      sps2uint64 TBP0 : 14;
      sps2uint64 TBW : 6;
      sps2uint64 PSM : 6;
      sps2uint64 TW : 4;
      sps2uint64 TH : 4;
      sps2uint64 TCC : 1;
      sps2uint64 TFX : 2;
      sps2uint64 CBP : 14;
      sps2uint64 CPSM : 4;
      sps2uint64 CSM : 1;
      sps2uint64 CSA : 5;
      sps2uint64 CLD : 3;
    } s;
  } sps2GIFRegTEX0_n_t;


  typedef union {
    sps2uint64 i64;

    struct {
      unsigned int LCM : 1;
      unsigned int _PAD1 : 1;
      unsigned int MXL : 3;
      unsigned int MMAG : 1;
      unsigned int MMIN : 3;
      unsigned int MTBA : 1;
      unsigned int _PAD2 : 9;
      unsigned int L : 2;
      unsigned int _PAD3 : 11;
      unsigned int K : 12;
      unsigned int _PAD4 : 20;
    } s;
  } sps2GIFRegTEX1_n_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int _PAD1: 20;
      unsigned int PSM : 6;
      unsigned int _PAD2 : 6;
      unsigned int _PAD3 : 5;
      unsigned int CBP : 14;
      unsigned int CPSM : 4;
      unsigned int CSM : 1;
      unsigned int CSA : 5;
      unsigned int CLD : 3;
    } s;
  } sps2GIFRegTEX2_n_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int TA0 : 8;
      unsigned int _PAD1 : 7;
      unsigned int AEM : 1;
      unsigned int _PAD2 : 16;
      unsigned int TA1 : 8;
      unsigned int _PAD3 : 24;
    } s;
  } sps2GIFRegTEXA_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int CBW : 6;
      unsigned int COU : 6;
      unsigned int COV : 10;
      unsigned int _PAD1 : 10;
      unsigned int _PAD2 : 32;
    } s;
  } sps2GIFRegTEXCLUT_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int _PAD1 : 32;
      unsigned int _PAD2 : 32;
    } s;
  } sps2GIFRegTEXFLUSH_t;


  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int XDIR : 2;
      unsigned int _PAD1 : 30;
      unsigned int _PAD2 : 32;
    } s;
  } sps2GIFRegTRXDIR_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int SSAX : 11;
      unsigned int _PAD1 : 5;
      unsigned int SSAY : 11;
      unsigned int _PAD2 : 5;
      unsigned int DSAX : 11;
      unsigned int _PAD3 : 5;
      unsigned int DSAY : 11;
      unsigned int DIR : 2;
      unsigned int _PAD4 : 3;
    } s;
  } sps2GIFRegTRXPOS_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int RRW : 12;
      unsigned int _PAD1 : 20;
      unsigned int RRH : 12;
      unsigned int _PAD2 : 20;
    } s;
  } sps2GIFRegTRXREG_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int U : 14;
      unsigned int _PAD1 : 2;
      unsigned int V : 14;
      unsigned int _PAD2 : 2;
      unsigned int _PAD3 : 32;
    } s;
  } sps2GIFRegUV_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int OFX : 16;
      unsigned int _PAD1 : 16;
      unsigned int OFY : 16;
      unsigned int _PAD2 : 16;
    } s;
  } sps2GIFRegXYOFFSET_n_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int X : 16;
      unsigned int Y : 16;
      unsigned int Z : 32;
    } s;
  } sps2GIFRegXYZn_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int X : 16;
      unsigned int Y : 16;
      unsigned int Z : 24;
      unsigned int F : 8;
    } s;
  } sps2GIFRegXYZFn_t;

  typedef union {
    sps2uint64 i64;
    struct {
      unsigned int ZBP : 9;
      unsigned int _PAD1 : 15;
      unsigned int PSM : 4;
      unsigned int _PAD2 : 4;
      unsigned int ZMSK : 1;
      unsigned int _PAD3 : 31;
    } s;
  } sps2GIFRegZBUF_n_t;

  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    sps2GIFRegALPHA_n_t ALPHA_n;
    sps2GIFRegBITBLTBUF_t BITBLTBUF;
    sps2GIFRegCLAMP_n_t CLAMP_n;
    sps2GIFRegCOLCLAMP_t COLCLAMP;
    sps2GIFRegDIMX_t DIMX;
    sps2GIFRegDTHE_t DTHE;
    sps2GIFRegFBA_n_t FBA_n;
    sps2GIFRegFINISH_t FINISH;
    sps2GIFRegFOG_t FOG;
    sps2GIFRegFOGCOL_t FOGCOL;
    sps2GIFRegFRAME_n_t FRAME_n;
    sps2GIFRegHWREG_t HWREG;
    sps2GIFRegLABEL_t LABEL;
    sps2GIFRegMIPTBP1_n_t MIPTBP1_n;
    sps2GIFRegMIPTBP2_n_t MIPTBP2_n;
    sps2GIFRegNOP_t NOP;
    sps2GIFRegPABE_t PABE;
    sps2GIFRegPRIM_t PRIM;
    sps2GIFRegPRMODE_t PRMODE;
    sps2GIFRegPRMODECONT_t PRMODECONT;
    sps2GIFRegRGBAQ_t RGBAQ;
    sps2GIFRegSCANMSK_t SCANMSK;
    sps2GIFRegSCISSOR_n_t SCISSOR_n;
    sps2GIFRegSIGNAL_t SIGNAL;
    sps2GIFRegST_t ST;
    sps2GIFRegTEST_n_t TEST_n;
    sps2GIFRegTEX0_n_t TEX0_n;
    sps2GIFRegTEX1_n_t TEX1_n;
    sps2GIFRegTEX2_n_t TEX2_n;
    sps2GIFRegTEXA_t TEXA;
    sps2GIFRegTEXCLUT_t TEXCLUT;
    sps2GIFRegTEXFLUSH_t TEXFLUSH;
    sps2GIFRegTRXDIR_t TRXDIR;
    sps2GIFRegTRXPOS_t TRXPOS;
    sps2GIFRegTRXREG_t TRXREG;
    sps2GIFRegUV_t UV;
    sps2GIFRegXYOFFSET_n_t XYOFFSET_n;
    sps2GIFRegXYZn_t XYZn;
    sps2GIFRegXYZFn_t XYZFn;
    sps2GIFRegZBUF_n_t ZBUF_n;
  } sps2GIFReg_t;

  /**
   * Here are DMA related tags
   */

  // Used by CHCR and the DMA tag
  typedef enum DMA_ID {
    DMA_ID_SOURCE_REFE=0,
    DMA_ID_SOURCE_CNT =1,
    DMA_ID_SOURCE_NEXT=2,
    DMA_ID_SOURCE_REF =3,
    DMA_ID_SOURCE_REFS=4,
    DMA_ID_SOURCE_CALL=5,
    DMA_ID_SOURCE_RET =6,
    DMA_ID_SOURCE_END =7, 
    DMA_ID_DEST_CNTS  =0,
    DMA_ID_DEST_CNT   =1,
    DMA_ID_DEST_END   =7,
  } DMA_ID_t;

  typedef union {
    sps2uint64 i64;
    
    struct {
      unsigned int QWC    : 16;
      unsigned int _PAD1  : 10;
      unsigned int PCE    :  2;
      DMA_ID_t     ID     :  3;
      unsigned int IRQ    :  1;
      unsigned int ADDR   : 31;
      unsigned int SPR    :  1;
      unsigned int _PAD2  : 32;
      unsigned int _PAD3  : 32;
    } s;
  } sps2DMATag_t;

  typedef enum {
    VIF_CMD_NOP=0,
    VIF_CMD_STCYCL=1,
    VIF_CMD_OFFSET=2,
    VIF_CMD_BASE=3,
    VIF_CMD_ITOP=4,
    VIF_CMD_STMOD=5,
    VIF_CMD_MSKPATH3=6,
    VIF_CMD_MARK=7,
    VIF_CMD_FLUSHE=16,
    VIF_CMD_FLUSH=17,
    VIF_CMD_FLUSHA=19,
    VIF_CMD_MSCAL=20,
    VIF_CMD_MSCNT=23,
    VIF_CMD_MSCALF=21,
    VIF_CMD_STMASK=32,
    VIF_CMD_STROW=48,
    VIF_CMD_STCOL=49,
    VIF_CMD_MPG=74,
    VIF_CMD_DIRECT=80,
    VIF_CMD_DIRECTHL=81,
    VIF_CMD_UNPACK=3,   // NOTE! This is 3 because it is only 2 bits
    // It would be 96 otherwise
  } VIF_CMD_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_NOP
      unsigned int I : 1;
    } s;
  } sps2VIFCodeNOP_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int CL : 8;
      unsigned int WL : 8;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_STCYCL
      unsigned int I : 1;
    } s;
  } sps2VIFCodeSTCYCL_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int OFFSET : 10;
      unsigned int _PAD : 6;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_OFFSET
      unsigned int I : 1;
    } s;
  } sps2VIFCodeOFFSET_t;


  typedef union {
    unsigned int i32;

    struct {
      unsigned int BASE : 10;
      unsigned int _PAD : 6;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_BASE
      unsigned int I : 1;
    } s;
  } sps2VIFCodeBASE_t;


  typedef union {
    unsigned int i32;

    struct {
      unsigned int ADDR : 10;
      unsigned int _PAD : 6;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_ITOP
      unsigned int I : 1;
    } s;
  } sps2VIFCodeITOP_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int MODE : 2;
      unsigned int _PAD : 14;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_STMOD
      unsigned int I : 1;
    } s;
  } sps2VIFCodeSTMOD_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _PAD : 15;
      unsigned int MASK : 1;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_MSKPATH3
      unsigned int I : 1;
    } s;
  } sps2VIFCodeMSKPATH3_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int MARK : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_MARK
      unsigned int I : 1;
    } s;
  } sps2VIFCodeMARK_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_FLUSHE
      unsigned int I : 1;
    } s;
  } sps2VIFCodeFLUSHE_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_FLUSH
      unsigned int I : 1;
    } s;
  } sps2VIFCodeFLUSH_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_FLUSHA
      unsigned int I : 1;
    } s;
  } sps2VIFCodeFLUSHA_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int EXECADDR : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_MSCAL
      unsigned int I : 1;
    } s;
  } sps2VIFCodeMSCAL_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_MSCNT
      unsigned int I : 1;
    } s;
  } sps2VIFCodeMSCNT_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int EXECADDR : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_MSCALF
      unsigned int I : 1;
    } s;
  } sps2VIFCodeMSCALF_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_STMASK
      unsigned int I : 1;
    } s;
  } sps2VIFCodeSTMASK_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_STROW
      unsigned int I : 1;
    } s;
  } sps2VIFCodeSTROW_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_STCOL
      unsigned int I : 1;
    } s;
  } sps2VIFCodeSTCOL_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int LOADADDR : 16;
      unsigned int SIZE : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_MPG
      unsigned int I : 1;
    } s;
  } sps2VIFCodeMPG_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int SIZE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_DIRECT
      unsigned int I : 1;
    } s;
  } sps2VIFCodeDIRECT_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int SIZE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Should be VIF_CMD_DIRECTHL
      unsigned int I : 1;
    } s;
  } sps2VIFCodeDIRECTHL_t;

  typedef enum {
    VIF_UNPACK_S_32  =  0,
    VIF_UNPACK_S_16  =  1,
    VIF_UNPACK_S_8   =  2,
    VIF_UNPACK_V2_32 =  4, 
    VIF_UNPACK_V2_16 =  5,
    VIF_UNPACK_V2_8 =   6, 
    VIF_UNPACK_V3_32 =  8, 
    VIF_UNPACK_V3_16 =  9,
    VIF_UNPACK_V3_8 =  10,
    VIF_UNPACK_V4_32 = 12, 
    VIF_UNPACK_V4_16 = 13,
    VIF_UNPACK_V4_8 =  14,
    VIF_UNPACK_V4_5 =  15,
  } VIF_UNPACK_FORMAT_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int ADDR : 10;
      unsigned int _PAD : 4;
      unsigned int USN : 1;
      unsigned int FLG : 1;
      unsigned int SIZE : 8;
      VIF_UNPACK_FORMAT_t FORMAT : 4;
      unsigned int M : 1;
      unsigned int CMD : 2;          // Should be VIF_CMD_UNPACK
      unsigned int I : 1;
    } s;
  } sps2VIFCodeUNPACK_t;

  typedef union {
    unsigned int i32;

    struct {
      unsigned int _IMMEDIATE : 16;
      unsigned int _NUM : 8;
      unsigned int CMD : 7;          // Works for everything but UNPACK
      unsigned int I : 1;
    } s;
  } sps2VIFCodeUNKNOWN_t;

  typedef union {
    sps2VIFCodeNOP_t NOP;
    sps2VIFCodeSTCYCL_t STCYCL;
    sps2VIFCodeOFFSET_t OFFSET;
    sps2VIFCodeBASE_t BASE;
    sps2VIFCodeITOP_t ITOP;
    sps2VIFCodeSTMOD_t STMOD;
    sps2VIFCodeMSKPATH3_t MSKPATH3;
    sps2VIFCodeMARK_t MARK;
    sps2VIFCodeFLUSHE_t FLUSHE;
    sps2VIFCodeFLUSH_t FLUSH;
    sps2VIFCodeFLUSHA_t FLUSHA;
    sps2VIFCodeMSCAL_t MSCAL;
    sps2VIFCodeMSCNT_t MSCNT;
    sps2VIFCodeMSCALF_t MSCALF;
    sps2VIFCodeSTMASK_t STMASK;
    sps2VIFCodeSTROW_t STROW;
    sps2VIFCodeSTCOL_t STCOL;
    sps2VIFCodeMPG_t MPG;
    sps2VIFCodeDIRECT_t DIRECT;
    sps2VIFCodeDIRECTHL_t DIRECTHL;
    sps2VIFCodeUNPACK_t UNPACK;
    sps2VIFCodeUNKNOWN_t UNKNOWN;
  } sps2VIFCode_t;

  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];
    sps2VIFCode_t c[2];
  } sps2VIFCode2_t;

  typedef union {
    sps2uint128 i128;
    sps2uint64 ai64[2];
    sps2uint32 ai32[4];

    sps2VIFCode_t c[4];
  } sps2VIFCode4_t;

  
#ifdef __cplusplus
}
#endif
#endif
