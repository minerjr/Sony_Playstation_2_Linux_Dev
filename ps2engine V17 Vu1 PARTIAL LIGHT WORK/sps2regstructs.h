#ifndef __SPS2_REGSTRUCTS_H
#include "sps2types.h"
  
#ifdef __cplusplus
extern "C" {
#endif

  typedef enum DMA_CHANNEL {
    DMA_CHANNEL_VIF0=0,
    DMA_CHANNEL_VIF1,
    DMA_CHANNEL_GIF,
    DMA_CHANNEL_FROM_IPU,
    DMA_CHANNEL_TO_IPU,
    DMA_CHANNEL_SIF0,
    DMA_CHANNEL_SIF1,
    DMA_CHANNEL_SIF2,
    DMA_CHANNEL_FROM_SPR,
    DMA_CHANNEL_TO_SPR,
  } DMA_CHANNEL_t;

  typedef enum CHCR_DIR {
    CHCR_DIR_TO_MEMORY=0,
    CHCR_DIR_FROM_MEMORY=1,
  } CHCR_DIR_t;

  typedef enum CHCR_MOD {
    CHCR_MOD_NORMAL=0,
    CHCR_MOD_CHAIN=1,
    CHCR_MOD_INTERLEAVE=2,
  } CHCR_MOD_t;

  // Defines the Dn_CHCR registers
  // The channel control registers
  typedef union Dn_CHCR {
    sps2uint32 i32;

    struct {
      unsigned int DIR     :  1;
      unsigned int _PAD1   :  1;
      unsigned int MOD     :  2;
      unsigned int ASP     :  2;
      unsigned int TTE     :  1;
      unsigned int TIE     :  1;
      unsigned int STR     :  1;
      unsigned int _PAD2   : 10;
      unsigned int TAG_PCE :  2;
      unsigned int TAG_ID  :  3;
      unsigned int TAG_IRQ :  1;
    } s;
  } Dn_CHCR_t;

  // Defines the Dn_MADR registers
  // The transfer address registers
  typedef union Dn_MADR {
    sps2uint32 i32;

    struct {
      unsigned int ADDR    : 31;
      unsigned int SPR     :  1;
    } s;
  } Dn_MADR_t;

  // This defines the Dn_TADR registers
  // The tag address registers
  typedef union Dn_TADR {
    sps2uint32 i32;

    struct {
      unsigned int ADDR    : 31;
      unsigned int SPR     :  1;
    } s;
  } Dn_TADR_t;

  // This defines the Dn_ASR0/1 registers
  // i.e. the Tag address save registers
  typedef union Dn_ASR {
    sps2uint32 i32;

    struct {
      unsigned int ADDR    : 31;
      unsigned int SPR     :  1;
    } s;
  } Dn_ASR_t;

  // This defines the Dn_QWC registers
  // i.e. the transfer qword count
  typedef union Dn_QWC {
    sps2uint32 i32;

    struct {
      unsigned int QWC     : 16;
      unsigned int _PAD1   : 16;
    } s;
  } Dn_QWC_t;

  // This defines the Dn_SADR registers
  // Used to define SPR transfer address
  typedef union Dn_SADR {
    sps2uint32 i32;

    struct {
      unsigned int ADDR    : 14;
      unsigned int _PAD1   : 18;
    } s;
  } Dn_SADR_t;

  // Defines the D_SQWC register
  // Used for interleaved transfers to/from the SPR
  typedef union D_SQWC  {
    sps2uint32 i32;

    struct {
      unsigned int SQWC    :  8;
      unsigned int _PAD1   :  8;
      unsigned int TQWC    :  8;
      unsigned int _PAD2   :  8;
    } s;
  } D_SQWC_t;

  /**
   * Here are the privileged GS registers
   */
  // Defines GS_BGCOLOR
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int R : 8;
      unsigned int G : 8;
      unsigned int B : 8;
      unsigned int _PAD1 : 8;
      unsigned int _PAD2 : 32;
    } s;
  } BGCOLOR_t;

  // Defines GS_BUSDIR
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int DIR : 1;
      unsigned int _PAD1 : 31;
      unsigned int _PAD2 : 32;
    } s;
  } BUSDIR_t;

  // Defines GS_CSR
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int SIGNAL : 1;
      unsigned int FINISH : 1;
      unsigned int HSINT : 1;
      unsigned int VSINT : 1;
      unsigned int EDWINT : 1;
      unsigned int ZERO1 : 1;
      unsigned int ZERO2 : 1;
      unsigned int _PAD1 : 1;
      unsigned int FLUSH : 1;
      unsigned int RESET : 1;
      unsigned int _PAD2 : 2;
      unsigned int NFIELD : 1;
      unsigned int FIELD : 1;
      unsigned int FIFO : 2;
      unsigned int REV : 8;
      unsigned int ID : 8;
      unsigned int _PAD3 : 32;
    } s;
  } CSR_t;

  // Defines the GS_DISPFB1/2 registers
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int FBP : 9;
      unsigned int FBW : 6;
      unsigned int PSM : 5;
      unsigned int _PAD : 12;
      unsigned int DBX : 11;
      unsigned int DBY : 11;
      unsigned int _PAD2 : 10;
    } s;
  } DISPFBn_t;

  // Defines the GS_DISPLAY1/2 registers
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int DX : 12;
      unsigned int DY : 11;
      unsigned int MAGH : 4;
      unsigned int MAGV : 2;
      unsigned int _PAD : 3;
      unsigned int DW : 12;
      unsigned int DH : 11;
      unsigned int _PAD2 : 9;
    } s;
  } DISPLAYn_t;

  // Defines the GS_EXTBUF register
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int EXBP : 14;
      unsigned int EXBW : 6;
      unsigned int FBIN : 2;
      unsigned int WFFMD : 1;
      unsigned int EMODA : 2;
      unsigned int EMODC : 2;
      unsigned int _PAD1 : 5;
      unsigned int WDX : 11;
      unsigned int WDY : 11;
      unsigned int _PAD2 : 10;
    } s;
  } EXTBUF_s;

  // Defines the GS_EXTDATA register
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int SX : 12;
      unsigned int SY : 11;
      unsigned int SMPH : 4;
      unsigned int SMPV : 2;
      unsigned int _PAD1 : 3;
      unsigned int WW : 12;
      unsigned int WH : 11;
      unsigned int _PAD2 : 9;
    } s;
  } EXTDATA_s;

  // Defines the GS_EXTWRITE register
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int WRITE : 1;
      unsigned int _PAD1 : 31;
      unsigned int _PAD2 : 32;
    } s;
  } EXTWRITE_t;

  // Defines the GS_IMR register
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int _PAD1 : 8;
      unsigned int SIGMSK : 1;
      unsigned int FINISHMSK : 1;
      unsigned int HSMSK : 1;
      unsigned int VSMSK : 1;
      unsigned int EDWMSK : 1;
      unsigned int _PAD2 : 19;
      unsigned int _PAD3 : 32;
    } s;
  } IMR_t;

  // Defines the GS_PMODE register
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int EN1 : 1;
      unsigned int EN2 : 1;
      unsigned int CRTMD : 3;
      unsigned int MMOD : 1;
      unsigned int AMOD : 1;
      unsigned int SLBG : 1;
      unsigned int ALP : 8;
      unsigned int _PAD : 16;
      unsigned int _PAD1 : 32;
    } s;
  } PMODE_t;

  // Defines the GS_SIGLBLID register
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int SIGID : 32;
      unsigned int LBLID : 32;
    } s;
  } SIGLBLID_t;

  // Defines the GS_SMODE2 register
  typedef union {
    sps2uint64 i64;
    sps2uint32 ai32[2];

    struct {
      unsigned int INT : 1;
      unsigned int FFMD : 1;
      unsigned int DPMS : 2;
      unsigned int _PAD2 : 28;
      unsigned int _PAD3 : 32;
    } s;
  } SMODE2_t;

#ifdef __cplusplus
}
#endif

#endif
