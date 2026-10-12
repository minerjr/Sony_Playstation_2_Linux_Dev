/** @file PS2Utils.h
	 Contain useful macro to build DMA packets.
*/
#ifndef _PS2_UTILS__
#define _PS2_UTILS__

#include <sps2util.h>
#include <memory.h>

/** @name RegisterDefines
	 Used to give meaningful value to register setting instead of putting
	 0 and 1.
	 @{
*/
#define DMA_CHCR_DIR_TOMEMORY				0x0
#define DMA_CHCR_DIR_FROMMEMORY			0x1
#define DMA_CHCR_MOD_NORMAL				0x0
#define DMA_CHCR_MOD_CHAIN					0x1
#define DMA_CHCR_MOD_INERLEAVE			0x2
#define DMA_CHCR_ASP_NOADDRESS			0x0
#define DMA_CHCR_ASP_1ADDRESS				0x1
#define DMA_CHCR_ASP_2ADDRESS				0x2
#define DMA_CHCR_TTE_NOTRANSFER_DMATAG		0x0
#define DMA_CHCR_TTE_TRANSFER_DMATAG		0x1
#define DMA_CHCR_TIE_INTDISABLE			0x0
#define DMA_CHCR_TIE_INTENABLE			0x1
#define DMA_CHCR_STR_STOPTRANSFER		0x0
#define DMA_CHCR_STR_STARTTRANSFER		0x1

#define GS_ALPHA_A_SOURCE					0x0
#define GS_ALPHA_A_FRAMEBUFFER			0x1
#define GS_ALPHA_A_ZERO						0x2
#define GS_ALPHA_B_SOURCE					0x0
#define GS_ALPHA_B_FRAMEBUFFER			0x1
#define GS_ALPHA_B_ZERO						0x2
#define GS_ALPHA_C_SOURCE					0x0
#define GS_ALPHA_C_FRAMEBUFFER			0x1
#define GS_ALPHA_C_FIX						0x2
#define GS_ALPHA_D_SOURCE					0x0
#define GS_ALPHA_D_FRAMEBUFFER			0x1
#define GS_ALPHA_D_ZERO						0x2

#define GS_CLAMP_MODE_REPEAT				0x0
#define GS_CLAMP_MODE_CLAMP				0x1
#define GS_CLAMP_MODE_REGION_CLAMP		0x2
#define GS_CLAMP_MODE_REGION_REPEAT		0x3

#define GS_GIF_EOP_WITHFOLLOWINGPRIM		0x0
#define GS_GIF_EOP_WITHOUTFOLLOWINGPRIM	0x1
#define GS_GIF_PRE_IGNOREPRIM					0x0
#define GS_GIF_EOP_USEPRIM						0x1

#define GS_PRIM_PRIM_POINT					0x0
#define GS_PRIM_PRIM_LINE					0x1
#define GS_PRIM_PRIM_LINESTRIP			0x2
#define GS_PRIM_PRIM_TRIANGLE				0x3
#define GS_PRIM_PRIM_TRISTRIP				0x4
#define GS_PRIM_PRIM_TRIFAN				0x5
#define GS_PRIM_PRIM_SPRITE				0x6
#define GS_PRIM_IIP_FLAT					0x0
#define GS_PRIM_IIP_GOURAUD				0x1
#define GS_PRIM_TME_TEXOFF					0x0
#define GS_PRIM_TME_TEXON					0x1
#define GS_PRIM_FGE_FOGOFF					0x0
#define GS_PRIM_FGE_FOGON					0x1
#define GS_PRIM_ABE_ALPHAOFF				0x0
#define GS_PRIM_ABE_ALPHAON				0x1
#define GS_PRIM_AA1_AAOFF					0x0
#define GS_PRIM_AA1_AAON					0x1
#define GS_PRIM_FST_STQ						0x0
#define GS_PRIM_FST_UV						0x1
#define GS_PRIM_CTXT_1						0x0
#define GS_PRIM_CTXT_2						0x1
#define GS_PRIM_FIX_UNFIXED				0x0
#define GS_PRIM_FIX_FIXED					0x1

#define GS_TEX0_TCC_RGB						0x0
#define GS_TEX0_TCC_RGBA					0x1
#define GS_TEX0_TFX_MODULATE				0x0
#define GS_TEX0_TFX_DECAL					0x1
#define GS_TEX0_TFX_HIGHLIGHT				0x2
#define GS_TEX0_TFX_HIGHLIGHT2			0x3
#define GS_TEX1_LCM_FIXED					0x1
#define GS_TEX1_LCM_LODFORMULA			0x0
#define GS_TEX1_MMAG_NEAREST				0x0
#define GS_TEX1_MMAG_LINEAR				0x1
#define GS_TEX1_MMIN_NEAREST				0x0
#define GS_TEX1_MMIN_LINEAR				0x1
#define GS_TEX1_MMIN_NEAREST_MIPMAP_NEAREST		0x2
#define GS_TEX1_MMIN_NEAREST_MIPMAP_LINEAR		0x3
#define GS_TEX1_MMIN_LINEAR_MIPMAP_NEAREST		0x4
#define GS_TEX1_MMIN_LINEAR_MIPMAP_LINEAR			0x5

//! @}

//! Create DMA header (64 bit )
#define PS2_DMA_SET_HEADER(QWC, PCE, ID, IRQ, ADDR, SPR) \
	((sps2uint128)(QWC) | ((sps2uint128)(PCE) << 26) | ((sps2uint128)(ID) << 28) | \
	((sps2uint128)(IRQ) << 31) | ((sps2uint128)(ADDR) << 32) | \
	((sps2uint128)(SPR) << 63))

//! Create CHCR
#define PS2_DMA_SET_CHCR(DIR, MOD, ASP, TTE, TIE, STR, TAG) \
	((sps2uint32)(DIR) | ((sps2uint32)(MOD) << 2) | ((sps2uint32)(ASP) << 4) | \
	((sps2uint32)(TTE) << 6) | ((sps2uint32)(TIE) << 7) | ((sps2uint32)(STR) << 8) | \
	((sps2uint32)(TAG) << 16))

//! Create TADR
#define PS2_DMA_SET_TADR(ADDR, SPR) \
	((sps2uint32)(ADDR) | ((sps2uint32)(SPR) << 31))

//! Set alpha register for texture image.
#define PS2_GIF_SET_ALPHA(A, B, C, D, FIX) \
	((sps2uint64)(A) | ((sps2uint64)(B) << 2) | ((sps2uint64)(C) << 4) | \
	((sps2uint64)(D) << 6) | ((sps2uint64)(FIX) << 32))

//! Set BitBlt register used to manage image transfer from and to the GS
#define PS2_GIF_SET_BITBLTBUF(SBP, SBW, SPSM, DBP, DBW, DPSM) \
	((sps2uint64)(SBP) | ((sps2uint64)(SBW) << 16) | ((sps2uint64)(SPSM) << 24) | \
	((sps2uint64)(DBP) << 32) | ((sps2uint64)(DBW) << 48) | ((sps2uint64)(DPSM) << 56)) 

#define PS2_GIF_SET_CLAMP(WMS, WMT, MINU, MAXU, MINV, MAXV) \
	((sps2uint64)(WMS) | ((sps2uint64)(WMT) << 2) | ((sps2uint64)(MINU) << 4) | \
	((sps2uint64)(MAXU) << 14) | ((sps2uint64)(MINV) << 24) | ((sps2uint64)(MAXV) << 34))

//! Create Gif TAG (64 bits, because upper 64 are used to specify Regs type)
#define PS2_GIF_SET_TAG(NLOOP, EOP, PRE, PRIM, FLG, NREG) \
	((sps2uint64)(NLOOP) | ((sps2uint64)(EOP) << 15) | ((sps2uint64)(PRE) << 46) | \
	((sps2uint64)(PRIM) << 47) | ((sps2uint64)(FLG) << 58) | ((sps2uint64)(NREG) << 60))

//! Create GS register TEX0
#define PS2_GIF_SET_TEX0(TBP0, TBW, PSM, TW, TH, TCC, TFX, CBP, CPSM, CSM, CSA, CLD) \
	((sps2uint64)(TBPO) | ((sps2uint64)(TBW) << 14) | ((sps2uint64)(PSM) << 20) | \
	((sps2uint64)(TW) << 26) | ((sps2uint64)(TH) << 30) | ((sps2uint64)(TCC) << 34) | \
	((sps2uint64)(TFX) << 35) | ((sps2uint64)(CBP) << 37) | ((sps2uint64)(CPSM) << 51) | \
	((sps2uint64)(CSM) << 55) | ((sps2uint64)(CSA) << 56))

//! Set GS TEX1 register.
#define PS2_GIF_SET_TEX1(LCM, MXL, MMAG, MMIN, MTBA, L, K) \
	((sps2uint64)(LCM) | ((sps2uint64)(MXL) << 2) | ((sps2uint64)(MMAG) << 5) | \
	((sps2uint64)(MMIN) << 6) | ((sps2uint64)(MTBA) << 9) | ((sps2uint64)(L) << 19) | \
	((sps2uint64)(K) << 32)

//! Create PRIM register (32 bit register)
#define PS2_GS_SET_PRIM(PRIM, IIP, TME, FGE, ABE, AA1, FST, CTXT, FIX) \
	((sps2uint32)(PRIM) | ((sps2uint32)(IIP) << 3) | \
	((sps2uint32)(TME) << 4) | ((sps2uint32)(FGE) << 5) | \
	((sps2uint32)(ABE) << 6)  | ((sps2uint32)(AA1) << 7) | \
	((sps2uint32)(FST) << 8) | ((sps2uint32)(CTXT) << 9) | ((sps2uint32)(FIX) << 10))

//! Create TRXDir Setting
#define PS2_GIF_SET_TRXDIR(XDIR) ((sps2uint64) XDIR)

//! Create TRXPOS Register
#define PS2_GIF_SET_TRXPOS(SSAX, SSAY, DSAX, DSAY, DIR) \
	((sps2uint64)(SSAX) | ((sps2uint64)(SSAY) << 15) | ((sps2uint64)(DSAX) << 32) | \
	((sps2uint64)(DSAY) << 48) | ((sps2uint64)(DIR) << 59))

//! Create TRXREG Register
#define PS2_GIF_SET_TRXREG(RRW, RRH) \
	((sps2uint64)(RRW) | ((sps2uint64)(RRH) << 32))

/** Used to access 128Bit QWord of PS2.
	 When building DMA packet it is very useful the possibility to address raw
	 memory in unit of 32, 64 or 128 bits.
*/
typedef union _tagQWord {
	sps2uint128		ul128;
	sps2uint64		ul64[2];
	sps2uint32		ul32[4];
} PS2_QWORD;

#define DECLARE_ZEROMEMORY_CONSTRUCTOR(CNAME) CNAME() {memset(this, 0, sizeof(*this));}
/** Used to build GIF Tag.
*/
typedef struct sceGifTag_t {

	sps2uint32 NLOOP:15;
	sps2uint32 EOP:1;
	sps2uint32 PAD16:16;
	sps2uint32 ID:14;
	sps2uint32 PRE:1;
	sps2uint32 PRIM:11;
	sps2uint32 FLG:2;
	sps2uint32 NREG:4;
	sps2uint32 REGS0:4;
	sps2uint32 REGS1:4;
	sps2uint32 REGS2:4;
	sps2uint32 REGS3:4;
	sps2uint32 REGS4:4;
	sps2uint32 REGS5:4;
	sps2uint32 REGS6:4;
	sps2uint32 REGS7:4;
	sps2uint32 REGS8:4;
	sps2uint32 REGS9:4;
	sps2uint32 REGS10:4;
	sps2uint32 REGS11:4;
	sps2uint32 REGS12:4;
	sps2uint32 REGS13:4;
	sps2uint32 REGS14:4;
	sps2uint32 REGS15:4;
	DECLARE_ZEROMEMORY_CONSTRUCTOR(sceGifTag_t)
} sceGifTag;

/** structure to easily use the TEX0 register.
*/
typedef struct sce_TEX0_tag {

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
	DECLARE_ZEROMEMORY_CONSTRUCTOR(sce_TEX0_tag);
} sce_TEX0;

//! Used to release SPS2 memory
#define SPS2_SAFE_FREE(pVoid) if (pVoid != NULL) sps2Free(pVoid);

/** Simple function to calculate int paryt of log2.
*/
int inline IntLog2(unsigned int Value) {

	unsigned int Pow2 = 1;
	int Exp2 = 0;
	while (Value > Pow2) {
	
		Pow2 *= 2;
		Exp2 += 1;
	}
	return Exp2;
}
/** Simple function to dump DMA pack.
	 To avoid crashing console there is the need to dump DMA packet before
	 actually send to the DMAC. This routine simply take a memory address
	 and dump in hex format a number of Num 16Byte Words.
	 
	 @param Data Pointer to memory to dump.
	 @param Num  Number of 128 bit word to dump
*/
void inline DumpDMAPack(void *Data, int Num) {

	//D is a pointer to sps2uint32[4] element.
	sps2uint32 (*D)[4] = (sps2uint32 (*)[4]) Data;
	for (int I = 0; I < Num; ++I) {
	
		printf("%d:\t%X %X %X %X\n", I, 
			D[I][3], D[I][2], D[I][1], D[I][0]);
	}
}
#endif