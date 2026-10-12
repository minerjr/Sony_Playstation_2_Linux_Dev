/*
    Copyright (C) 2002 Terratron Technologies Inc.  All Rights Reserved.
    Author: Steven Osman
 
    This file is part of sps2.

    Before using this file you MUST agree to the license agreement in
    the file LICENSE provided with this package.
  
    For more information visit:
    http://window.terratron.com/~sosman/ps2linux/
    http://www.terratron.com/

    8/22/2003 - Sauce - Minor changes in __KERNEL__ section
*/
#ifndef __SPS2_REGISTERS_H
#define __SPS2_REGISTERS_H

#include "sps2types.h"

/**
 * The following macros hold true only in user space.
 * They are not the physical addresses of the registers.
 */
#ifndef __KERNEL__
/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                            EE REGISTERS                         *
 *                          USER SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *******************************************************************/


#define SPS2_EE_REGISTERS_START 0x00010000
/**
 * Timer
 */
#define EE_T0_COUNT      ((volatile sps2uint32 *) 0x00010000)
#define EE_T0_MODE       ((volatile sps2uint32 *) 0x00010010)
#define EE_T0_COMP       ((volatile sps2uint32 *) 0x00010020)
#define EE_T0_HOLD       ((volatile sps2uint32 *) 0x00010030)

#define EE_T1_COUNT      ((volatile sps2uint32 *) 0x00010800)
#define EE_T1_MODE       ((volatile sps2uint32 *) 0x00010810)
#define EE_T1_COMP       ((volatile sps2uint32 *) 0x00010820)
#define EE_T1_HOLD       ((volatile sps2uint32 *) 0x00010830)

#define EE_T2_COUNT      ((volatile sps2uint32 *) 0x00011000)
#define EE_T2_MODE       ((volatile sps2uint32 *) 0x00011010)
#define EE_T2_COMP       ((volatile sps2uint32 *) 0x00011020)

#define EE_T3_COUNT      ((volatile sps2uint32 *) 0x00011800)
#define EE_T3_MODE       ((volatile sps2uint32 *) 0x00011810)
#define EE_T3_COMP       ((volatile sps2uint32 *) 0x00011820)

/**
 * IPU
 */
#define EE_IPU_CMD       ((volatile sps2uint64 *) 0x00012000)
#define EE_IPU_CTRL      ((volatile sps2uint32 *) 0x00012010)
#define EE_IPU_BP        ((const volatile sps2uint32 *) 0x00012020)
#define EE_IPU_TOP       ((const volatile sps2uint64 *) 0x00012030)

/**
 * GIF
 */
#define EE_GIF_CTRL      ((volatile sps2uint32 *) 0x00013000)
#define EE_GIF_MODE      ((volatile sps2uint32 *) 0x00013010)
#define EE_GIF_STAT      ((const volatile sps2uint32 *) 0x00013020)
#define EE_GIF_TAG0      ((const volatile sps2uint32 *) 0x00013040)
#define EE_GIF_TAG1      ((const volatile sps2uint32 *) 0x00013050)
#define EE_GIF_TAG2      ((const volatile sps2uint32 *) 0x00013060)
#define EE_GIF_TAG3      ((const volatile sps2uint32 *) 0x00013070)
#define EE_GIF_CNT       ((const volatile sps2uint32 *) 0x00013080)
#define EE_GIF_P3CNT     ((const volatile sps2uint32 *) 0x00013090)
#define EE_GIF_P3TAG     ((const volatile sps2uint32 *) 0x000130a0)

/**
 * VIF0
 */
#define EE_VIF0_STAT     ((const volatile sps2uint32 *) 0x00013800)
#define EE_VIF0_FBRST    ((volatile sps2uint32 *) 0x00013810)
#define EE_VIF0_ERR      ((volatile sps2uint32 *) 0x00013820)
#define EE_VIF0_MARK     ((volatile sps2uint32 *) 0x00013830)
#define EE_VIF0_CYCLE    ((const volatile sps2uint32 *) 0x00013840)
#define EE_VIF0_MODE     ((const volatile sps2uint32 *) 0x00013850)
#define EE_VIF0_NUM      ((const volatile sps2uint32 *) 0x00013860)
#define EE_VIF0_MASK     ((const volatile sps2uint32 *) 0x00013870)
#define EE_VIF0_CODE     ((const volatile sps2uint32 *) 0x00013880)
#define EE_VIF0_ITOPS    ((const volatile sps2uint32 *) 0x00013890)
#define EE_VIF0_ITOP     ((const volatile sps2uint32 *) 0x000138d0)
#define EE_VIF0_R0       ((const volatile sps2uint32 *) 0x00013900)
#define EE_VIF0_R1       ((const volatile sps2uint32 *) 0x00013910)
#define EE_VIF0_R2       ((const volatile sps2uint32 *) 0x00013920)
#define EE_VIF0_R3       ((const volatile sps2uint32 *) 0x00013930)
#define EE_VIF0_C0       ((const volatile sps2uint32 *) 0x00013940)
#define EE_VIF0_C1       ((const volatile sps2uint32 *) 0x00013950)
#define EE_VIF0_C2       ((const volatile sps2uint32 *) 0x00013960)
#define EE_VIF0_C3       ((const volatile sps2uint32 *) 0x00013970)

#define EE_VIF1_STAT     ((const volatile sps2uint32 *) 0x00013c00)
#define EE_VIF1_FBRST    ((volatile sps2uint32 *) 0x00013c10)
#define EE_VIF1_ERR      ((volatile sps2uint32 *) 0x00013c20)
#define EE_VIF1_MARK     ((volatile sps2uint32 *) 0x00013c30)
#define EE_VIF1_CYCLE    ((const volatile sps2uint32 *) 0x00013c40)
#define EE_VIF1_MODE     ((const volatile sps2uint32 *) 0x00013c50)
#define EE_VIF1_NUM      ((const volatile sps2uint32 *) 0x00013c60)
#define EE_VIF1_MASK     ((const volatile sps2uint32 *) 0x00013c70)
#define EE_VIF1_CODE     ((const volatile sps2uint32 *) 0x00013c80)
#define EE_VIF1_ITOPS    ((const volatile sps2uint32 *) 0x00013c90)
#define EE_VIF1_BASE     ((const volatile sps2uint32 *) 0x00013ca0)
#define EE_VIF1_OFST     ((const volatile sps2uint32 *) 0x00013cb0)
#define EE_VIF1_TOPS     ((const volatile sps2uint32 *) 0x00013cc0)
#define EE_VIF1_ITOP     ((const volatile sps2uint32 *) 0x00013cd0)
#define EE_VIF1_TOP      ((const volatile sps2uint32 *) 0x00013ce0)
#define EE_VIF1_R0       ((const volatile sps2uint32 *) 0x00013d00)
#define EE_VIF1_R1       ((const volatile sps2uint32 *) 0x00013d10)
#define EE_VIF1_R2       ((const volatile sps2uint32 *) 0x00013d20)
#define EE_VIF1_R3       ((const volatile sps2uint32 *) 0x00013d30)
#define EE_VIF1_C0       ((const volatile sps2uint32 *) 0x00013d40)
#define EE_VIF1_C1       ((const volatile sps2uint32 *) 0x00013d50)
#define EE_VIF1_C2       ((const volatile sps2uint32 *) 0x00013d60)
#define EE_VIF1_C3       ((const volatile sps2uint32 *) 0x00013d70)

/**
 * FIFO
 */
// TODO: Need to confirm the size and the read/write abilities of EE_VIF0_FIFO,
// EE_VIF1_FIFO and EE_GIF_FIFO
#define DPUT_EE_VIF0_FIFO(value)         SPS2_QPUT(0x00014000L,value)
#define DPUT_EE_VIF1_FIFO(value)         SPS2_QPUT(0x00015000L,value)
#define DGET_EE_VIF1_FIFO                SPS2_QGET(0x00015000L)
#define DPUT_EE_GIF_FIFO(value)          SPS2_QPUT(0x00016000L,value)
#define DGET_EE_IPU_out_FIFO             SPS2_QGET(0x00017000L)
#define DPUT_EE_IPU_in_FIFO(value)       SPS2_QPUT(0x00017010L,value)

/**
 * DMAC Channel 0
 */
#define EE_D0_CHCR       ((volatile sps2uint32 *) 0x00018000)
#define EE_D0_MADR       ((volatile sps2uint32 *) 0x00018010)
#define EE_D0_QWC        ((volatile sps2uint32 *) 0x00018020)
#define EE_D0_TADR       ((volatile sps2uint32 *) 0x00018030)
#define EE_D0_ASR0       ((volatile sps2uint32 *) 0x00018040)
#define EE_D0_ASR1       ((volatile sps2uint32 *) 0x00018050)

/**
 * DMAC Channel 1
 */
#define EE_D1_CHCR       ((volatile sps2uint32 *) 0x00019000)
#define EE_D1_MADR       ((volatile sps2uint32 *) 0x00019010)
#define EE_D1_QWC        ((volatile sps2uint32 *) 0x00019020)
#define EE_D1_TADR       ((volatile sps2uint32 *) 0x00019030)
#define EE_D1_ASR0       ((volatile sps2uint32 *) 0x00019040)
#define EE_D1_ASR1       ((volatile sps2uint32 *) 0x00019050)

/**
 * DMAC Channel 2
 */
#define EE_D2_CHCR       ((volatile sps2uint32 *) 0x0001a000)
#define EE_D2_MADR       ((volatile sps2uint32 *) 0x0001a010)
#define EE_D2_QWC        ((volatile sps2uint32 *) 0x0001a020)
#define EE_D2_TADR       ((volatile sps2uint32 *) 0x0001a030)
#define EE_D2_ASR0       ((volatile sps2uint32 *) 0x0001a040)
#define EE_D2_ASR1       ((volatile sps2uint32 *) 0x0001a050)

/**
 * DMAC Channel 3
 */
#define EE_D3_CHCR       ((volatile sps2uint32 *) 0x0001b000)
#define EE_D3_MADR       ((volatile sps2uint32 *) 0x0001b010)
#define EE_D3_QWC        ((volatile sps2uint32 *) 0x0001b020)

/**
 * DMAC Channel 4
 */
#define EE_D4_CHCR       ((volatile sps2uint32 *) 0x0001b400)
#define EE_D4_MADR       ((volatile sps2uint32 *) 0x0001b410)
#define EE_D4_QWC        ((volatile sps2uint32 *) 0x0001b420)
#define EE_D4_TADR       ((volatile sps2uint32 *) 0x0001b430)

/**
 * DMAC Channel 5
 */
#define EE_D5_CHCR       ((volatile sps2uint32 *) 0x0001c000)
#define EE_D5_MADR       ((volatile sps2uint32 *) 0x0001c010)
#define EE_D5_QWC        ((volatile sps2uint32 *) 0x0001c020)

/**
 * DMAC Channel 6 
 */
#define EE_D6_CHCR       ((volatile sps2uint32 *) 0x0001c400)
#define EE_D6_MADR       ((volatile sps2uint32 *) 0x0001c410)
#define EE_D6_QWC        ((volatile sps2uint32 *) 0x0001c420)
#define EE_D6_TADR       ((volatile sps2uint32 *) 0x0001c430)

/**
 * DMAC Channel 7
 */
#define EE_D7_CHCR       ((volatile sps2uint32 *) 0x0001c800)
#define EE_D7_MADR       ((volatile sps2uint32 *) 0x0001c810)
#define EE_D7_QWC        ((volatile sps2uint32 *) 0x0001c820)

/**
 * DMAC Channel 8
 */
#define EE_D8_CHCR       ((volatile sps2uint32 *) 0x0001d000)
#define EE_D8_MADR       ((volatile sps2uint32 *) 0x0001d010)
#define EE_D8_QWC        ((volatile sps2uint32 *) 0x0001d020)
#define EE_D8_SADR       ((volatile sps2uint32 *) 0x0001d080)

/**
 * DMAC Channel 9
 */
#define EE_D9_CHCR       ((volatile sps2uint32 *) 0x0001d400)
#define EE_D9_MADR       ((volatile sps2uint32 *) 0x0001d410)
#define EE_D9_QWC        ((volatile sps2uint32 *) 0x0001d420)
#define EE_D9_TADR       ((volatile sps2uint32 *) 0x0001d430)
#define EE_D9_SADR       ((volatile sps2uint32 *) 0x0001d480)

/**
 * DMAC
 */
#define EE_D_CTRL        ((volatile sps2uint32 *) 0x0001e000)
#define EE_D_STAT        ((volatile sps2uint32 *) 0x0001e010)
#define EE_D_PCR         ((volatile sps2uint32 *) 0x0001e020)
#define EE_D_SQWC        ((volatile sps2uint32 *) 0x0001e030)
#define EE_D_RBSR        ((volatile sps2uint32 *) 0x0001e040)
#define EE_D_RBOR        ((volatile sps2uint32 *) 0x0001e050)
#define EE_D_STADR       ((volatile sps2uint32 *) 0x0001e060)
#define EE_D_ENABLER     ((const volatile sps2uint32 *) 0x0001f520)
#define EE_D_ENABLEW     ((volatile sps2uint32 *) 0x0001f590)

/**
 * INTC
 */
#define EE_I_STAT        ((volatile sps2uint32 *) 0x0001f000)
#define EE_I_MASK        ((volatile sps2uint32 *) 0x0001f010)

/**
 * SIF
 */
// TODO: Need to confirm the size and the read/write abilities of EE_SB_SMFLG
#define EE_SB_SMFLG      ((volatile sps2uint32 *) 0x0001f230)


/**
 * OFFSET BASED DECLARATIONS
 * Use these when you're using a different area that you've mapped to the
 * EE registers.
 */

/**
 * Timer
 */
#define EE_T0_COUNT_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0000))
#define EE_T0_MODE_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0010))
#define EE_T0_COMP_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0020))
#define EE_T0_HOLD_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0030))

#define EE_T1_COUNT_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0800))
#define EE_T1_MODE_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0810))
#define EE_T1_COMP_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0820))
#define EE_T1_HOLD_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x0830))

#define EE_T2_COUNT_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x1000))
#define EE_T2_MODE_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x1010))
#define EE_T2_COMP_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x1020))

#define EE_T3_COUNT_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x1800))
#define EE_T3_MODE_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x1810))
#define EE_T3_COMP_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x1820))

/**
 * IPU
 */
#define EE_IPU_CMD_OFF(base)       ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x2000))
#define EE_IPU_CTRL_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x2010))
#define EE_IPU_BP_OFF(base)        ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x2020))
#define EE_IPU_TOP_OFF(base)       ((const volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x2030))

/**
 * GIF
 */
#define EE_GIF_CTRL_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3000))
#define EE_GIF_MODE_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3010))
#define EE_GIF_STAT_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3020))
#define EE_GIF_TAG0_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3040))
#define EE_GIF_TAG1_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3050))
#define EE_GIF_TAG2_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3060))
#define EE_GIF_TAG3_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3070))
#define EE_GIF_CNT_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3080))
#define EE_GIF_P3CNT_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3090))
#define EE_GIF_P3TAG_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x30a0))

/**
 * VIF0
 */
#define EE_VIF0_STAT_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3800))
#define EE_VIF0_FBRST_OFF(base)    ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3810))
#define EE_VIF0_ERR_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3820))
#define EE_VIF0_MARK_OFF(base)     ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3830))
#define EE_VIF0_CYCLE_OFF(base)    ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3840))
#define EE_VIF0_MODE_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3850))
#define EE_VIF0_NUM_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3860))
#define EE_VIF0_MASK_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3870))
#define EE_VIF0_CODE_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3880))
#define EE_VIF0_ITOPS_OFF(base)    ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3890))
#define EE_VIF0_ITOP_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x38d0))
#define EE_VIF0_R0_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3900))
#define EE_VIF0_R1_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3910))
#define EE_VIF0_R2_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3920))
#define EE_VIF0_R3_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3930))
#define EE_VIF0_C0_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3940))
#define EE_VIF0_C1_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3950))
#define EE_VIF0_C2_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3960))
#define EE_VIF0_C3_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3970))

#define EE_VIF1_STAT_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c00))
#define EE_VIF1_FBRST_OFF(base)    ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c10))
#define EE_VIF1_ERR_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c20))
#define EE_VIF1_MARK_OFF(base)     ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c30))
#define EE_VIF1_CYCLE_OFF(base)    ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c40))
#define EE_VIF1_MODE_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c50))
#define EE_VIF1_NUM_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c60))
#define EE_VIF1_MASK_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c70))
#define EE_VIF1_CODE_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c80))
#define EE_VIF1_ITOPS_OFF(base)    ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3c90))
#define EE_VIF1_BASE_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3ca0))
#define EE_VIF1_OFST_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3cb0))
#define EE_VIF1_TOPS_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3cc0))
#define EE_VIF1_ITOP_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3cd0))
#define EE_VIF1_TOP_OFF(base)      ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3ce0))
#define EE_VIF1_R0_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d00))
#define EE_VIF1_R1_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d10))
#define EE_VIF1_R2_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d20))
#define EE_VIF1_R3_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d30))
#define EE_VIF1_C0_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d40))
#define EE_VIF1_C1_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d50))
#define EE_VIF1_C2_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d60))
#define EE_VIF1_C3_OFF(base)       ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x3d70))

/**
 * FIFO
 */
// TODO: Need to confirm the size and the read/write abilities of EE_VIF0_FIFO,
// EE_VIF1_FIFO and EE_GIF_FIFO
#define DPUT_EE_VIF0_FIFO_OFF(base, value)   SPS2_QPUT(((sps2uint32) base +\
							0x4000),value)
#define DPUT_EE_VIF1_FIFO_OFF(base, value)   SPS2_QPUT(((sps2uint32) base +\
							0x5000),value)
#define DGET_EE_VIF1_FIFO_OFF(base)          SPS2_QGET(((sps2uint32) base +\
							0x5000))
#define DPUT_EE_GIF_FIFO_OFF(base, value)    SPS2_QPUT(((sps2uint32) base +\
							0x6000),value)
#define DGET_EE_IPU_out_FIFO_OFF(base)       SPS2_QGET(((sps2uint32) base +\
							0x7000))
#define DPUT_EE_IPU_in_FIFO_OFF(base, value) SPS2_QPUT(((sps2uint32) base +\
							0x7010),value)
/**
 * DMAC Channel 0
 */
#define EE_D0_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x8000))
#define EE_D0_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x8010))
#define EE_D0_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x8020))
#define EE_D0_TADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x8030))
#define EE_D0_ASR0_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x8040))
#define EE_D0_ASR1_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x8050))

/**
 * DMAC Channel 1
 */
#define EE_D1_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x9000))
#define EE_D1_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x9010))
#define EE_D1_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x9020))
#define EE_D1_TADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x9030))
#define EE_D1_ASR0_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x9040))
#define EE_D1_ASR1_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0x9050))

/**
 * DMAC Channel 2
 */
#define EE_D2_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xa000))
#define EE_D2_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xa010))
#define EE_D2_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xa020))
#define EE_D2_TADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xa030))
#define EE_D2_ASR0_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xa040))
#define EE_D2_ASR1_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xa050))

/**
 * DMAC Channel 3
 */
#define EE_D3_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xb000))
#define EE_D3_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xb010))
#define EE_D3_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xb020))

/**
 * DMAC Channel 4
 */
#define EE_D4_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xb400))
#define EE_D4_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xb410))
#define EE_D4_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xb420))
#define EE_D4_TADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xb430))

/**
 * DMAC Channel 5
 */
#define EE_D5_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc000))
#define EE_D5_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc010))
#define EE_D5_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc020))

/**
 * DMAC Channel 6 
 */
#define EE_D6_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc400))
#define EE_D6_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc410))
#define EE_D6_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc420))
#define EE_D6_TADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc430))

/**
 * DMAC Channel 7
 */
#define EE_D7_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc800))
#define EE_D7_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc810))
#define EE_D7_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xc820))

/**
 * DMAC Channel 8
 */
#define EE_D8_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd000))
#define EE_D8_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd010))
#define EE_D8_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd020))
#define EE_D8_SADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd080))

/**
 * DMAC Channel 9
 */
#define EE_D9_CHCR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd400))
#define EE_D9_MADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd410))
#define EE_D9_QWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd420))
#define EE_D9_TADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd430))
#define EE_D9_SADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xd480))

/**
 * DMAC
 */
#define EE_D_CTRL_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xe000))
#define EE_D_STAT_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xe010))
#define EE_D_PCR_OFF(base)         ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xe020))
#define EE_D_SQWC_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xe030))
#define EE_D_RBSR_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xe040))
#define EE_D_RBOR_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xe050))
#define EE_D_STADR_OFF(base)       ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xe060))
#define EE_D_ENABLER_OFF(base)     ((const volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xf520))
#define EE_D_ENABLEW_OFF(base)     ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xf590))

/**
 * INTC
 */
#define EE_I_STAT_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xf000))
#define EE_I_MASK_OFF(base)        ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xf010))

/**
 * SIF
 */
// TODO: Need to confirm the size and the read/write abilities of EE_SB_SMFLG
#define EE_SB_SMFLG_OFF(base)      ((volatile sps2uint32 *)\
                                    ((sps2uint32) base + 0xf230))


#else

/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                            EE REGISTERS                         *
 *                        KERNEL SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *******************************************************************/
#define SPS2_EE_REGISTERS_START 0x10000000L

/**
 * Timer
 */
#define EE_T0_COUNT      ((volatile sps2uint32 *) 0x10000000)
#define EE_T0_MODE       ((volatile sps2uint32 *) 0x10000010)
#define EE_T0_COMP       ((volatile sps2uint32 *) 0x10000020)
#define EE_T0_HOLD       ((volatile sps2uint32 *) 0x10000030)

#define EE_T1_COUNT      ((volatile sps2uint32 *) 0x10000800)
#define EE_T1_MODE       ((volatile sps2uint32 *) 0x10000810)
#define EE_T1_COMP       ((volatile sps2uint32 *) 0x10000820)
#define EE_T1_HOLD       ((volatile sps2uint32 *) 0x10000830)

#define EE_T2_COUNT      ((volatile sps2uint32 *) 0x10001000)
#define EE_T2_MODE       ((volatile sps2uint32 *) 0x10001010)
#define EE_T2_COMP       ((volatile sps2uint32 *) 0x10001020)

#define EE_T3_COUNT      ((volatile sps2uint32 *) 0x10001800)
#define EE_T3_MODE       ((volatile sps2uint32 *) 0x10001810)
#define EE_T3_COMP       ((volatile sps2uint32 *) 0x10001820)

/**
 * IPU
 */
#define EE_IPU_CMD       ((volatile sps2uint64 *) 0x10002000)
#define EE_IPU_CTRL      ((volatile sps2uint32 *) 0x10002010)
#define EE_IPU_BP        ((const volatile sps2uint32 *) 0x10002020)
#define EE_IPU_TOP       ((const volatile sps2uint64 *) 0x10002030)

/**
 * GIF
 */
#define EE_GIF_CTRL      ((volatile sps2uint32 *) 0x10003000)
#define EE_GIF_MODE      ((volatile sps2uint32 *) 0x10003010)
#define EE_GIF_STAT      ((const volatile sps2uint32 *) 0x10003020)
#define EE_GIF_TAG0      ((const volatile sps2uint32 *) 0x10003040)
#define EE_GIF_TAG1      ((const volatile sps2uint32 *) 0x10003050)
#define EE_GIF_TAG2      ((const volatile sps2uint32 *) 0x10003060)
#define EE_GIF_TAG3      ((const volatile sps2uint32 *) 0x10003070)
#define EE_GIF_CNT       ((const volatile sps2uint32 *) 0x10003080)
#define EE_GIF_P3CNT     ((const volatile sps2uint32 *) 0x10003090)
#define EE_GIF_P3TAG     ((const volatile sps2uint32 *) 0x100030a0)

/**
 * VIF0
 */
#define EE_VIF0_STAT     ((const volatile sps2uint32 *) 0x10003800)
#define EE_VIF0_FBRST    ((volatile sps2uint32 *) 0x10003810)
#define EE_VIF0_ERR      ((volatile sps2uint32 *) 0x10003820)
#define EE_VIF0_MARK     ((volatile sps2uint32 *) 0x10003830)
#define EE_VIF0_CYCLE    ((const volatile sps2uint32 *) 0x10003840)
#define EE_VIF0_MODE     ((const volatile sps2uint32 *) 0x10003850)
#define EE_VIF0_NUM      ((const volatile sps2uint32 *) 0x10003860)
#define EE_VIF0_MASK     ((const volatile sps2uint32 *) 0x10003870)
#define EE_VIF0_CODE     ((const volatile sps2uint32 *) 0x10003880)
#define EE_VIF0_ITOPS    ((const volatile sps2uint32 *) 0x10003890)
#define EE_VIF0_ITOP     ((const volatile sps2uint32 *) 0x100038d0)
#define EE_VIF0_R0       ((const volatile sps2uint32 *) 0x10003900)
#define EE_VIF0_R1       ((const volatile sps2uint32 *) 0x10003910)
#define EE_VIF0_R2       ((const volatile sps2uint32 *) 0x10003920)
#define EE_VIF0_R3       ((const volatile sps2uint32 *) 0x10003930)
#define EE_VIF0_C0       ((const volatile sps2uint32 *) 0x10003940)
#define EE_VIF0_C1       ((const volatile sps2uint32 *) 0x10003950)
#define EE_VIF0_C2       ((const volatile sps2uint32 *) 0x10003960)
#define EE_VIF0_C3       ((const volatile sps2uint32 *) 0x10003970)

#define EE_VIF1_STAT     ((const volatile sps2uint32 *) 0x10003c00)
#define EE_VIF1_FBRST    ((volatile sps2uint32 *) 0x10003c10)
#define EE_VIF1_ERR      ((volatile sps2uint32 *) 0x10003c20)
#define EE_VIF1_MARK     ((volatile sps2uint32 *) 0x10003c30)
#define EE_VIF1_CYCLE    ((const volatile sps2uint32 *) 0x10003c40)
#define EE_VIF1_MODE     ((const volatile sps2uint32 *) 0x10003c50)
#define EE_VIF1_NUM      ((const volatile sps2uint32 *) 0x10003c60)
#define EE_VIF1_MASK     ((const volatile sps2uint32 *) 0x10003c70)
#define EE_VIF1_CODE     ((const volatile sps2uint32 *) 0x10003c80)
#define EE_VIF1_ITOPS    ((const volatile sps2uint32 *) 0x10003c90)
#define EE_VIF1_BASE     ((const volatile sps2uint32 *) 0x10003ca0)
#define EE_VIF1_OFST     ((const volatile sps2uint32 *) 0x10003cb0)
#define EE_VIF1_TOPS     ((const volatile sps2uint32 *) 0x10003cc0)
#define EE_VIF1_ITOP     ((const volatile sps2uint32 *) 0x10003cd0)
#define EE_VIF1_TOP      ((const volatile sps2uint32 *) 0x10003ce0)
#define EE_VIF1_R0       ((const volatile sps2uint32 *) 0x10003d00)
#define EE_VIF1_R1       ((const volatile sps2uint32 *) 0x10003d10)
#define EE_VIF1_R2       ((const volatile sps2uint32 *) 0x10003d20)
#define EE_VIF1_R3       ((const volatile sps2uint32 *) 0x10003d30)
#define EE_VIF1_C0       ((const volatile sps2uint32 *) 0x10003d40)
#define EE_VIF1_C1       ((const volatile sps2uint32 *) 0x10003d50)
#define EE_VIF1_C2       ((const volatile sps2uint32 *) 0x10003d60)
#define EE_VIF1_C3       ((const volatile sps2uint32 *) 0x10003d70)

/**
 * FIFO
 */
// TODO: Need to confirm the size and the read/write abilities of EE_VIF0_FIFO,
// EE_VIF1_FIFO and EE_GIF_FIFO
#define EE_VIF0_FIFO     ((volatile sps2uint32 *) 0x10004000)
#define EE_VIF1_FIFO     ((volatile sps2uint32 *) 0x10005000)
#define EE_GIF_FIFO      ((volatile sps2uint32 *) 0x10006000)
#define EE_IPU_out_FIFO  ((const volatile sps2uint128 *) 0x10007000)
#define EE_IPU_in_FIFO   ((volatile sps2uint32 *) 0x10007010)

/**
 * DMAC Channel 0
 */
#define EE_D0_CHCR       ((volatile sps2uint32 *) 0x10008000)
#define EE_D0_MADR       ((volatile sps2uint32 *) 0x10008010)
#define EE_D0_QWC        ((volatile sps2uint32 *) 0x10008020)
#define EE_D0_TADR       ((volatile sps2uint32 *) 0x10008030)
#define EE_D0_ASR0       ((volatile sps2uint32 *) 0x10008040)
#define EE_D0_ASR1       ((volatile sps2uint32 *) 0x10008050)

/**
 * DMAC Channel 1
 */
#define EE_D1_CHCR       ((volatile sps2uint32 *) 0x10009000)
#define EE_D1_MADR       ((volatile sps2uint32 *) 0x10009010)
#define EE_D1_QWC        ((volatile sps2uint32 *) 0x10009020)
#define EE_D1_TADR       ((volatile sps2uint32 *) 0x10009030)
#define EE_D1_ASR0       ((volatile sps2uint32 *) 0x10009040)
#define EE_D1_ASR1       ((volatile sps2uint32 *) 0x10009050)

/**
 * DMAC Channel 2
 */
#define EE_D2_CHCR       ((volatile sps2uint32 *) 0x1000a000)
#define EE_D2_MADR       ((volatile sps2uint32 *) 0x1000a010)
#define EE_D2_QWC        ((volatile sps2uint32 *) 0x1000a020)
#define EE_D2_TADR       ((volatile sps2uint32 *) 0x1000a030)
#define EE_D2_ASR0       ((volatile sps2uint32 *) 0x1000a040)
#define EE_D2_ASR1       ((volatile sps2uint32 *) 0x1000a050)

/**
 * DMAC Channel 3
 */
#define EE_D3_CHCR       ((volatile sps2uint32 *) 0x1000b000)
#define EE_D3_MADR       ((volatile sps2uint32 *) 0x1000b010)
#define EE_D3_QWC        ((volatile sps2uint32 *) 0x1000b020)

/**
 * DMAC Channel 4
 */
#define EE_D4_CHCR       ((volatile sps2uint32 *) 0x1000b400)
#define EE_D4_MADR       ((volatile sps2uint32 *) 0x1000b410)
#define EE_D4_QWC        ((volatile sps2uint32 *) 0x1000b420)
#define EE_D4_TADR       ((volatile sps2uint32 *) 0x1000b430)

/**
 * DMAC Channel 5
 */
#define EE_D5_CHCR       ((volatile sps2uint32 *) 0x1000c000)
#define EE_D5_MADR       ((volatile sps2uint32 *) 0x1000c010)
#define EE_D5_QWC        ((volatile sps2uint32 *) 0x1000c020)

/**
 * DMAC Channel 6 
 */
#define EE_D6_CHCR       ((volatile sps2uint32 *) 0x1000c400)
#define EE_D6_MADR       ((volatile sps2uint32 *) 0x1000c410)
#define EE_D6_QWC        ((volatile sps2uint32 *) 0x1000c420)
#define EE_D6_TADR       ((volatile sps2uint32 *) 0x1000c430)

/**
 * DMAC Channel 7
 */
#define EE_D7_CHCR       ((volatile sps2uint32 *) 0x1000c800)
#define EE_D7_MADR       ((volatile sps2uint32 *) 0x1000c810)
#define EE_D7_QWC        ((volatile sps2uint32 *) 0x1000c820)

/**
 * DMAC Channel 8
 */
#define EE_D8_CHCR       ((volatile sps2uint32 *) 0x1000d000)
#define EE_D8_MADR       ((volatile sps2uint32 *) 0x1000d010)
#define EE_D8_QWC        ((volatile sps2uint32 *) 0x1000d020)
#define EE_D8_SADR       ((volatile sps2uint32 *) 0x1000d080)

/**
 * DMAC Channel 9
 */
#define EE_D9_CHCR       ((volatile sps2uint32 *) 0x1000d400)
#define EE_D9_MADR       ((volatile sps2uint32 *) 0x1000d410)
#define EE_D9_QWC        ((volatile sps2uint32 *) 0x1000d420)
#define EE_D9_TADR       ((volatile sps2uint32 *) 0x1000d430)
#define EE_D9_SADR       ((volatile sps2uint32 *) 0x1000d480)

/**
 * DMAC
 */
#define EE_D_CTRL        ((volatile sps2uint32 *) 0x1000e000)
#define EE_D_STAT        ((volatile sps2uint32 *) 0x1000e010)
#define EE_D_PCR         ((volatile sps2uint32 *) 0x1000e020)
#define EE_D_SQWC        ((volatile sps2uint32 *) 0x1000e030)
#define EE_D_RBSR        ((volatile sps2uint32 *) 0x1000e040)
#define EE_D_RBOR        ((volatile sps2uint32 *) 0x1000e050)
#define EE_D_STADR       ((volatile sps2uint32 *) 0x1000e060)
#define EE_D_ENABLER     ((const volatile sps2uint32 *) 0x1000f520)
#define EE_D_ENABLEW     ((volatile sps2uint32 *) 0x1000f590)

/**
 * INTC
 */
#define EE_I_STAT        ((volatile sps2uint32 *) 0x1000f000)
#define EE_I_MASK        ((volatile sps2uint32 *) 0x1000f010)

/**
 * SIF
 */
// TODO: Need to confirm the size and the read/write abilities of EE_SB_SMFLG
#define EE_SB_SMFLG      ((volatile sps2uint32 *) 0x1000f230)

#endif // __KERNEL__

#ifndef __KERNEL__
/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                            GS REGISTERS                         *
 *                          USER SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *******************************************************************/

#define SPS2_GS_REGISTERS_START 0x00020000

/**
 * GS Special
 */
#define _GS_PMODE         ((volatile sps2uint64 *) 0x00020000L)
#define _GS_SMODE1        ((volatile sps2uint64 *) 0x00020010L)
#define _GS_SMODE2        ((volatile sps2uint64 *) 0x00020020L)
#define _GS_SRFSH         ((volatile sps2uint64 *) 0x00020030L)
#define _GS_SYNCH1        ((volatile sps2uint64 *) 0x00020040L)
#define _GS_SYNCH2        ((volatile sps2uint64 *) 0x00020050L)
#define _GS_SYNCV         ((volatile sps2uint64 *) 0x00020060L)
#define _GS_DISPFB1       ((volatile sps2uint64 *) 0x00020070L)
#define _GS_DISPLAY1      ((volatile sps2uint64 *) 0x00020080L)
#define _GS_DISPFB2       ((volatile sps2uint64 *) 0x00020090L)
#define _GS_DISPLAY2      ((volatile sps2uint64 *) 0x000200a0L)
#define _GS_EXTBUF        ((volatile sps2uint64 *) 0x000200b0L)
#define _GS_EXTDATA       ((volatile sps2uint64 *) 0x000200c0L)
#define _GS_EXTWRITE      ((volatile sps2uint64 *) 0x000200d0L)
#define _GS_BGCOLOR       ((volatile sps2uint64 *) 0x000200e0L)
#define _GS_CSR           ((volatile sps2uint64 *) 0x00021000L)
#define _GS_IMR           ((volatile sps2uint64 *) 0x00021010L)
#define _GS_BUSDIR        ((volatile sps2uint64 *) 0x00021040L)
#define _GS_SIGBLID       ((volatile sps2uint64 *) 0x00021080L)


/**
 * GS Specials -- with 64bit put functions
 */
#define DPUT_GS_PMODE(value)         SPS2_DPUT(0x00020000L,value)
#define DPUT_GS_SMODE1(value)        SPS2_DPUT(0x00020010L,value)
#define DPUT_GS_SMODE2(value)        SPS2_DPUT(0x00020020L,value)
#define DPUT_GS_SRFSH(value)         SPS2_DPUT(0x00020030L,value)
#define DPUT_GS_SYNCH1(value)        SPS2_DPUT(0x00020040L,value)
#define DPUT_GS_SYNCH2(value)        SPS2_DPUT(0x00020050L,value)
#define DPUT_GS_SYNCV(value)         SPS2_DPUT(0x00020060L,value)
#define DPUT_GS_DISPFB1(value)       SPS2_DPUT(0x00020070L,value)
#define DPUT_GS_DISPLAY1(value)      SPS2_DPUT(0x00020080L,value)
#define DPUT_GS_DISPFB2(value)       SPS2_DPUT(0x00020090L,value)
#define DPUT_GS_DISPLAY2(value)      SPS2_DPUT(0x000200a0L,value)
#define DPUT_GS_EXTBUF(value)        SPS2_DPUT(0x000200b0L,value)
#define DPUT_GS_EXTDATA(value)       SPS2_DPUT(0x000200c0L,value)
#define DPUT_GS_EXTWRITE(value)      SPS2_DPUT(0x000200d0L,value)
#define DPUT_GS_BGCOLOR(value)       SPS2_DPUT(0x000200e0L,value)
#define DPUT_GS_CSR(value)           SPS2_DPUT(0x00021000L,value)
#define DPUT_GS_IMR(value)           SPS2_DPUT(0x00021010L,value)
#define DPUT_GS_BUSDIR(value)        SPS2_DPUT(0x00021040L,value)
#define DPUT_GS_SIGBLID(value)       SPS2_DPUT(0x00021080L,value)



/**
 * OFFSET BASED DECLARATIONS
 * Use these when you're using a different area that you've mapped to the
 * GS registers.
 */

/**
 * GS Special
 */
#define GS_PMODE_OFF(base)         ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0000))
#define GS_SMODE1_OFF(base)        ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0010))
#define GS_SMODE2_OFF(base)        ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0020))
#define GS_SRFSH_OFF(base)         ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0030))
#define GS_SYNCH1_OFF(base)        ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0040))
#define GS_SYNCH2_OFF(base)        ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0050))
#define GS_SYNCV_OFF(base)         ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0060))
#define GS_DISPFB1_OFF(base)       ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0070))
#define GS_DISPLAY1_OFF(base)      ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0080))
#define GS_DISPFB2_OFF(base)       ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x0090))
#define GS_DISPLAY2_OFF(base)      ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x00a0))
#define GS_EXTBUF_OFF(base)        ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x00b0))
#define GS_EXTDATA_OFF(base)       ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x00c0))
#define GS_EXTWRITE_OFF(base)      ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x00d0))
#define GS_BGCOLOR_OFF(base)       ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x00e0))
#define GS_CSR_OFF(base)           ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x1000))
#define GS_IMR_OFF(base)           ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x1010))
#define GS_BUSDIR_OFF(base)        ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x1040))
#define GS_SIGBLID_OFF(base)       ((volatile sps2uint64 *)\
                                    ((sps2uint32) base + 0x1080))


/**
 * GS Specials -- with 64bit put functions
 */
#define DPUT_GS_PMODE_OFF(base, value)         SPS2_DPUT(((sps2uint32) base +\
                                                          0x0000), value)
#define DPUT_GS_SMODE1_OFF(base, value)        SPS2_DPUT(((sps2uint32) base +\
                                                          0x0010), value)
#define DPUT_GS_SMODE2_OFF(base, value)        SPS2_DPUT(((sps2uint32) base +\
                                                          0x0020), value)
#define DPUT_GS_SRFSH_OFF(base, value)         SPS2_DPUT(((sps2uint32) base +\
                                                          0x0030), value)
#define DPUT_GS_SYNCH1_OFF(base, value)        SPS2_DPUT(((sps2uint32) base +\
                                                          0x0040), value)
#define DPUT_GS_SYNCH2_OFF(base, value)        SPS2_DPUT(((sps2uint32) base +\
                                                          0x0050), value)
#define DPUT_GS_SYNCV_OFF(base, value)         SPS2_DPUT(((sps2uint32) base +\
                                                          0x0060), value)
#define DPUT_GS_DISPFB1_OFF(base, value)       SPS2_DPUT(((sps2uint32) base +\
                                                          0x0070), value)
#define DPUT_GS_DISPLAY1_OFF(base, value)      SPS2_DPUT(((sps2uint32) base +\
                                                          0x0080), value)
#define DPUT_GS_DISPFB2_OFF(base, value)       SPS2_DPUT(((sps2uint32) base +\
                                                          0x0090), value)
#define DPUT_GS_DISPLAY2_OFF(base, value)      SPS2_DPUT(((sps2uint32) base +\
                                                          0x00a0), value)
#define DPUT_GS_EXTBUF_OFF(base, value)        SPS2_DPUT(((sps2uint32) base +\
                                                          0x00b0), value)
#define DPUT_GS_EXTDATA_OFF(base, value)       SPS2_DPUT(((sps2uint32) base +\
                                                          0x00c0), value)
#define DPUT_GS_EXTWRITE_OFF(base, value)      SPS2_DPUT(((sps2uint32) base +\
                                                          0x00d0), value)
#define DPUT_GS_BGCOLOR_OFF(base, value)       SPS2_DPUT(((sps2uint32) base +\
                                                          0x00e0), value)
#define DPUT_GS_CSR_OFF(base, value)           SPS2_DPUT(((sps2uint32) base +\
                                                          0x1000), value)
#define DPUT_GS_IMR_OFF(base, value)           SPS2_DPUT(((sps2uint32) base +\
                                                          0x1010), value)
#define DPUT_GS_BUSDIR_OFF(base, value)        SPS2_DPUT(((sps2uint32) base +\
                                                          0x1040), value)
#define DPUT_GS_SIGBLID_OFF(base, value)       SPS2_DPUT(((sps2uint32) base +\
                                                          0x1080), value)

#else

/*******************************************************************
 *                                                                 *
 *                                                                 *
 *                                                                 *
 *                            GS REGISTERS                         *
 *                        KERNEL SPACE VALUES                      *
 *                                                                 *
 *                                                                 *
 *******************************************************************/

#define SPS2_GS_REGISTERS_START 0x12000000L

/**
 * GS Special
 */
#define _GS_PMODE         ((volatile sps2uint64 *) 0x12000000L)
#define _GS_SMODE1        ((volatile sps2uint64 *) 0x12000010L)
#define _GS_SMODE2        ((volatile sps2uint64 *) 0x12000020L)
#define _GS_SRFSH         ((volatile sps2uint64 *) 0x12000030L)
#define _GS_SYNCH1        ((volatile sps2uint64 *) 0x12000040L)
#define _GS_SYNCH2        ((volatile sps2uint64 *) 0x12000050L)
#define _GS_SYNCV         ((volatile sps2uint64 *) 0x12000060L)
#define _GS_DISPFB1       ((volatile sps2uint64 *) 0x12000070L)
#define _GS_DISPLAY1      ((volatile sps2uint64 *) 0x12000080L)
#define _GS_DISPFB2       ((volatile sps2uint64 *) 0x12000090L)
#define _GS_DISPLAY2      ((volatile sps2uint64 *) 0x120000a0L)
#define _GS_EXTBUF        ((volatile sps2uint64 *) 0x120000b0L)
#define _GS_EXTDATA       ((volatile sps2uint64 *) 0x120000c0L)
#define _GS_EXTWRITE      ((volatile sps2uint64 *) 0x120000d0L)
#define _GS_BGCOLOR       ((volatile sps2uint64 *) 0x120000e0L)
#define _GS_CSR           ((volatile sps2uint64 *) 0x12001000L)
#define _GS_IMR           ((volatile sps2uint64 *) 0x12001010L)
#define _GS_BUSDIR        ((volatile sps2uint64 *) 0x12001040L)
#define _GS_SIGBLID       ((volatile sps2uint64 *) 0x12001080L)


/**
 * GS Specials -- with 64bit put functions
 */
#define DPUT_GS_PMODE(value)         SPS2_DPUT(0x12000000L,value)
#define DPUT_GS_SMODE1(value)        SPS2_DPUT(0x12000010L,value)
#define DPUT_GS_SMODE2(value)        SPS2_DPUT(0x12000020L,value)
#define DPUT_GS_SRFSH(value)         SPS2_DPUT(0x12000030L,value)
#define DPUT_GS_SYNCH1(value)        SPS2_DPUT(0x12000040L,value)
#define DPUT_GS_SYNCH2(value)        SPS2_DPUT(0x12000050L,value)
#define DPUT_GS_SYNCV(value)         SPS2_DPUT(0x12000060L,value)
#define DPUT_GS_DISPFB1(value)       SPS2_DPUT(0x12000070L,value)
#define DPUT_GS_DISPLAY1(value)      SPS2_DPUT(0x12000080L,value)
#define DPUT_GS_DISPFB2(value)       SPS2_DPUT(0x12000090L,value)
#define DPUT_GS_DISPLAY2(value)      SPS2_DPUT(0x120000a0L,value)
#define DPUT_GS_EXTBUF(value)        SPS2_DPUT(0x120000b0L,value)
#define DPUT_GS_EXTDATA(value)       SPS2_DPUT(0x120000c0L,value)
#define DPUT_GS_EXTWRITE(value)      SPS2_DPUT(0x120000d0L,value)
#define DPUT_GS_BGCOLOR(value)       SPS2_DPUT(0x120000e0L,value)
#define DPUT_GS_CSR(value)           SPS2_DPUT(0x12001000L,value)
#define DPUT_GS_IMR(value)           SPS2_DPUT(0x12001010L,value)
#define DPUT_GS_BUSDIR(value)        SPS2_DPUT(0x12001040L,value)
#define DPUT_GS_SIGBLID(value)       SPS2_DPUT(0x12001080L,value)


#endif // __KERNEL__

/**
 * This function makes sure to write a quad-word in one shot instead
 * of as four 32 bit writes.
 */
#define SPS2_QPUT(pointer, value) \
do {\
    union { \
        sps2uint128 di; \
        struct { \
                unsigned long  l0, l1, l2, l3; \
        } si; \
    } src;\
\
    src.di=value;\
    __asm__ __volatile__( \
        ".set push\n" \
        "	.set mips3\n" \
        "	pextlw         $8,%1,%0\n" \
		  "	pextlw			$9,%3,%2\n"	\
		  "	pcpyld			$8, $9, $8\n"	\
        "	sq             $8,(%4)\n" \
        "	.set   pop" \
        : : "r"(src.si.l0), "r"(src.si.l1), "r"(src.si.l2),\
            "r"(src.si.l3), "r" (pointer) : "$8", "$9"); \
} while (0);

#define SPS2_DPUT(pointer, value) \
do {\
    union { \
        unsigned long long di; \
        struct { \
                unsigned long  lo, hi; \
        } si; \
    } src;\
\
    src.di=value;\
    __asm__ __volatile__( \
        ".set push\n" \
        "	.set mips3\n" \
        "	pextlw         $8,%1,%0\n" \
        "	sd             $8,(%2)\n" \
        "	.set   pop" \
        : : "r"(src.si.lo), "r"(src.si.hi), "r" (pointer) : "$8"); \
} while (0);


/**
 * This is the counterpart to SPS2_QPUT.  It reads 128 bits in one shot.
 */
static inline sps2uint128 SPS2_QGET(sps2uint128 *pointer) {
  sps2uint128 value;
  
  do {
    union { 
      sps2uint128 di;
      struct {
	unsigned long  l0, l1, l2, l3;
      } si;
    } src;

    src.di=value;
    __asm__ __volatile__(
			 ".set push\n"
			 "	.set mips3\n"
			 "	lq			$8,(%4)\n"
			 "	pcpyud			$9, $8, $0\n"
			 "	dsrl			%1, $8, 32\n"
			 "	dsrl			%3, $9, 32\n"
			 "	dsll			$8, $8, 32\n"
			 "	dsll			$9, $9, 32\n"
			 "	dsrl			%0, $8, 32\n"
			 "	dsrl			%2, $9, 32\n"
			 "	.set   pop"
			 : "=&r"(src.si.l0), "=&r"(src.si.l1),
			   "=&r"(src.si.l2), "=&r"(src.si.l3)
			 : "r" (pointer) : "$8", "$9");
  } while (0);
  
  return value;
}

#endif
