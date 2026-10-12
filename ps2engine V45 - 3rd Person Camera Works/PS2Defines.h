#ifndef __PS2DEFINES_H__
#define __PS2DEFINES_H__

#include "structs.h"

#define KILOBYTES(x) ((x) * 1024)
#define MEGABYTES(x) (KILOBYTES(x) * 1024)

#define ALIGN_QW	__attribute__((aligned(16)))
#define ALIGN_4QW	__attribute__((aligned(64)))
#define ALIGN_16QW	__attribute__((aligned(128)))

//---------------------------------------------------------------------------
// PRIM Register - Setup Drawing Primitive
//   PRI  - Primitive type
//   IIP  - Shading method (0=flat, 1=gouraud)
//   TME  - Texture mapping (0=off, 1=on)
//   FGE  - Fog (0=off, 1=on)
//   ABE  - Alpha Blending (0=off, 1=on)
//   AA1  - Antialiasing (0=off,1=on)
//   FST  - Texture coordinate specification (0=use ST/RGBAQ register, 1=use UV register)
//				(UV means no perspective correction, good for 2D)
//   CTXT - Drawing context (0=1, 1=2)
//   FIX  - ?? Fragment value control (use 0)
//---------------------------------------------------------------------------
#define PRIM_POINT			0
#define PRIM_LINE			1
#define PRIM_LINE_STRIP		2
#define PRIM_TRI			3
#define PRIM_TRI_STRIP		4
#define PRIM_TRI_FAN		5
#define PRIM_SPRITE			6
#define PRIM_IIP_FLAT		0
#define PRIM_IIP_GOURAUD	1
#define PRIM_TME_OFF		0
#define PRIM_TME_ON			1
#define PRIM_FGE_OFF		0
#define PRIM_FGE_ON			1
#define PRIM_ABE_OFF		0
#define PRIM_ABE_ON			1
#define PRIM_AA1_OFF		0
#define PRIM_AA1_ON			1
#define PRIM_FST_STQ		0
#define PRIM_FST_UV			1
#define PRIM_CTXT_CONTEXT1	0
#define PRIM_CTXT_CONTEXT2	1
#define PRIM_FIX_NOFIXDDA	0
#define PRIM_FIX_FIXDDA		1

typedef enum GS_DRAWING_PRIMITIVES
{
	GS_POINT,		// 0
	GS_LINE,		// 1
	GS_LINESTRIP,	// 2
	GS_TRIANGLE,	// 3
	GS_TRISTRIP,	// 4
	GS_TRIFAN,		// 5
	GS_SPRITE		// 6
}GS_DRAWING_PRIMITIVES_t;

#define GS_GIFTAG(NLOOP, EOP, PRE, PRIM, FLG, NREG) \
	((uint64(NREG)<<60) | \
	(uint64(FLG)<<58) 	| \
	(uint64(PRIM)<<47) 	| \
	(uint64(PRE)<<46) 	| \
	(EOP<<15) 			| \
	(NLOOP<<0))

#define GS_PRIM(PRIM, IIP, TME, FGE, ABE, AA1, FST, CTXT, FIX) \
	((sps2uint128)((FIX<<10)|\
	(CTXT<<9)|\
	(FST<<8)|\
	(AA1<<7)|\
	(ABE<<6)|\
	(FGE<<5)|\
	(TME<<4)|\
	(IIP<<3)|\
	(PRIM)))

#define ALPHA_SET(A, B, C, D, FIX) \
	((uint64) (uint64(FIX)<<32) | \
	(D<<6) 						| \
	(C<<4) 						| \
	(B<<2) 						| \
	(A<<0))

#define TEST_SET(ATE, ATST, AREF, AFAIL, DATE, DATM, ZTE, ZTST) \
	((uint64(ZTST)<<17) | \
	(uint64(ZTE)<<16) 	| \
	(uint64(DATM)<<15) 	| \
	(uint64(DATE)<<14) 	| \
	(uint64(AFAIL)<<12) | \
	(uint64(AREF)<<4) 	| \
	(uint64(ATST)<<1) 	| \
	(uint64(ATE)<<0))

#define TEXA_SET(TA0, AEM, TA1) \
	((uint64(TA1)<<32) 	| \
	(uint64(AEM)<<15) 	| \
	(uint64(TA0)<<0))

#define BITBLTBUF_SET(SBP, SBW, SPSM, DBP, DBW, DPSM) \
	((uint64(DPSM)<<56) | \
	(uint64(DBW)<<48)	| \
	(uint64(DBP)<<32) 	| \
	(uint64(SPSM)<<24) 	| \
	(uint64(SBW)<<16) 	| \
	(uint64(SBP)<<0))

#define CLAMP_SET(WMS, WMT, MINU, MAXU, MINV, MAXV) \
	((uint64) (uint64(MAXV)<<34) | \
	(uint64(MINV)<<24) 	| \
	(uint64(MAXU)<<14) 	| \
	(uint64(MINU)<<4) 	| \
	(uint64(WMT)<<2) 	| \
	(uint64(WMS)<<0))

#define COLCLAMP_SET(CLAMP) ((uint64) CLAMP&0x1)

#define FBA_SET(FBA) ((uint64) FBA&0x1)

#define FOGCOL_SET(FCR, FCG, FCB) \
	((uint64(FCB&0xff)<<16) | \
	(uint64(FCG&0xff)<<8) 	| \
	(uint64(FCR&0xff)<<0))

#define FRAME_SET(FBP, FBW, PSM, FBMSK) \
	((uint64(FBMSK)<<32) | \
	(uint64(PSM)<<24) 	 | \
	(uint64(FBW)<<16) 	 | \
	uint64(FBP))

#define TEX0_SET(TBP0, TBW, PSM, TW, TH, TCC, TFX, CBP, CPSM, CSM, CSA, CLD) \
	((uint64(CLD)<<61) 	| \
	(uint64(CSA)<<56) 	| \
	(uint64(CSM)<<55) 	| \
	(uint64(CPSM)<<51) 	| \
	(uint64(CBP)<<37) 	| \
	(uint64(TFX)<<35) 	| \
	(uint64(TCC)<<34) 	| \
	(uint64(TH)<<30) 	| \
	(uint64(TW)<<26) 	| \
	(uint64(PSM)<<20) 	| \
	(uint64(TBW)<<14) 	| \
	(uint64(TBP0)<<0))

#define TEX1_SET(LCM, MXL, MMAG, MMIN, MTBA, L, K) \
	((uint64(K)<<32) | \
	(uint64(L)<<19) | \
	(uint64(MTBA)<<9) | \
	(uint64(MMIN)<<6) | \
	(uint64(MMAG)<<5) | \
	(uint64(MXL)<<2) | \
	(uint64(LCM)<<0))

#define TEX2_SET(PSM, CBP, CPSM, CSM, CSA, CLD) \
	((uint64(CLD)<<61) | \
	(uint64(CSA)<<56) | \
	(uint64(CSM)<<55) | \
	(uint64(CPSM)<<51) | \
	(uint64(CBP)<<37) | \
	(uint64(PSM)<<20))

#define TEXCLUT_SET(CBW, COU, COV) \
	((uint64(COV)<<12) | \
	(uint64(COU)<<6) | \
	(uint64(CBW)<<0))

#define TRXDIR_SET(XDIR) ((uint64) XDIR&0x3)

#define TRXPOS_SET(SSAX, SSAY, DSAX, DSAY, DIR) \
	((uint64(DIR)<<59) | \
	(uint64(DSAY)<<48) | \
	(uint64(DSAX)<<32) | \
	(uint64(SSAY)<<16) | \
	(uint64(SSAX)<<0))

#define TRXREG_SET(RRW, RRH) ((uint64(RRH)<<32) | (uint64(RRW)<<0))

#define MIPTBP1_SET(TBP1, TBW1, TBP2, TBW2, TBP3, TBW3) \
	((uint64(TBW3)<<54) | \
	(uint64(TBP3)<<40) | \
	(uint64(TBW2)<<34) | \
	(uint64(TBP2)<<20) | \
	(uint64(TBW1)<<14) | \
	(uint64(TBP1)<<0))
	
#define MIPTBP2_SET(TBP4, TBW4, TBP5, TBW5, TBP6, TBW6)	\
		MIPTBP1_SET(TBP4, TBW4, TBP5, TBW5, TBP6, TBW6)

#define SCISSOR_SET(SCAX0, SCAX1, SCAY0, SCAY1) \
	((uint64(SCAY1)<<48) | \
	(uint64(SCAY0)<<32) | \
	(uint64(SCAX1)<<16) | \
	(uint64(SCAX0)<<0))

#define XYOFFSET_SET(OFX, OFY) ((uint64(OFY)<<32) | (uint64(OFX)<<0))

#define RGBAQ_SET(R,G,B,A,Q) \
	((uint64(Q)<<32) | \
	(uint64(A)<<24) | \
	(uint64(B)<<16) | \
	(uint64(G)<<8) | \
	(uint64(R)<<0))

#define ST_SET(S,T) ((uint64(T)<<32) | (uint64(S)<<0))

#define UV_SET(U,V) ((uint64(V)<<16) | (uint64(U)<<0))

#define XYZ2_SET(X, Y, Z) ((uint64(Z)<<32) | (uint64(Y)<<16) | (uint64(X)<<0))

#define XYZ3_SET(X, Y, Z) XYZ2_SET(X,Y,Z)

#define XYZF2_SET(X, Y, Z, F) \
	((uint64(F)<<56) | \
	(uint64(Z)<<32) | \
	(uint64(Y)<<16) | \
	(uint64(X)<<0))

#define XYZF3_SET(X, Y, Z, F) XYZF2_SET(X,Y,Z,F)

#define PABE_SET(PABE) ((uint64) PABE&0x1)

#define PRMODECONT_SET(AC) ((uint64) AC&0x1)

#define PRMODE_SET(IIP, TME, FGE, ABE, AA1, FST, CTXT, FIX)	\
	((uint64) ((FIX<<10)|\
	(CTXT<<9)|\
	(FST<<8)|\
	(AA1<<7)|\
	(ABE<<6)|\
	(FGE<<5)|\
	(TME<<4)|\
	(IIP<<3)))

#define LABEL_SET(ID, IDMSK) ((uint64(IDMSK)<<32) | uint64(ID))

#define SIGNAL_SET(ID, IDMSK) ((uint64(IDMSK)<<32) | uint64(ID))

#define ZBUF_SET(ZBP, PSM, ZMSK) \
	((uint64(ZMSK)<<32) | \
	(uint64(PSM)<<24) | \
	(uint64(ZBP)<<0))

#define HWREG_SET(DATA) ((uint64) DATA)

#define SCANMSK_SET(MSK) ((uint64) (MSK&0x3))

#define DTHE_SET(DTHE) ((uint64) DTHE&0x1)

#define DIMX_SET(DM00, DM01, DM02, DM03, DM10, DM11, DM12, DM13, DM20, DM21, \
										DM22, DM23, DM30, DM31, DM32, DM33) \
	((uint64(DM33&0x7)<<60) | \
	(uint64(DM32&0x7)<<56) 	| \
	(uint64(DM31&0x7)<<52) 	| \
	(uint64(DM30&0x7)<<48) 	| \
	(uint64(DM23&0x7)<<44) 	| \
	(uint64(DM22&0x7)<<40) 	| \
	(uint64(DM21&0x7)<<36) 	| \
	(uint64(DM20&0x7)<<32) 	| \
	(uint64(DM13&0x7)<<28) 	| \
	(uint64(DM12&0x7)<<24) 	| \
	(uint64(DM11&0x7)<<20) 	| \
	(uint64(DM10&0x7)<<16) 	| \
	(uint64(DM03&0x7)<<12) 	| \
	(uint64(DM02&0x7)<<8) 	| \
	(uint64(DM01&0x7)<<4) 	| \
	(uint64(DM00&0x7)<<0))

#define BGCOLOR_SET(R, G, B) ((uint64(B)<<16) | (uint64(G)<<8) | (uint64(R)<<0))

#define PMODE_SET(EN1, EN2, CRTMD, MMOD, AMOD, SLBG, ALP) \
	((uint64(ALP)<<8) 	| \
	(uint64(SLBG)<<7) 	| \
	(uint64(AMOD)<<6) 	| \
	(uint64(MMOD)<<5) 	| \
	(uint64(CRTMD)<<2) 	| \
	(uint64(EN2)<<1) 	| \
	(uint64(EN1)<<0))	

#define SMODE2_SET(INT, FFMD, DPMS) \
	((uint64(DPMS)<<2) 	| \
	(uint64(FFMD)<<1) 	| \
	(uint64(INT)<<0))	

#define DISPFB_SET(FBP, FBW, PSM, DBX, DBY) \
	((uint64(DBY)<<43) | \
	(uint64(DBX)<<32)  | \
	(uint64(PSM)<<15)  | \
	(uint64(FBW)<<9)   | \
	(uint64(FBP)<<0))	

#define DISPLAY_SET(DX, DY, MAGH, MAGV, DW, DH) \
	((uint64(DH)<<44)  | \
	(uint64(DW)<<32)   | \
	(uint64(MAGV)<<27) | \
	(uint64(MAGH)<<23) | \
	(uint64(DY)<<12)   | \
	(uint64(DX)<<0))	

#endif

