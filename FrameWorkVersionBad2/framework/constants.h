 /****************************************************************************\
|* PS2 constants definitions
|* =========================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/

#ifndef __framework__constants__header__
#define __framework__constants__header__

#include <math.h> // for M_PI

typedef unsigned int u_long128 __attribute__ ((mode(TI)));
typedef unsigned int u_long64  __attribute__ ((mode(DI)));
typedef unsigned char byte;

#define GIF_SET_TAG(nloop, eop, pre, prim, flg, nreg) \
	((u_long128)(nloop) | ((u_long128)(eop)<<15) | ((u_long128)(pre) << 46) | \
	 ((u_long128)(prim)<<47) | ((u_long128)(flg)<<58) | ((u_long128)(nreg)<<60))

#define GIF_SET_REGS(tag1, tag2, tag3, tag4, tag5, tag6, tag7, tag8, tag9, tag10, tag11, tag12, tag13, tag14, tag15, tag16) \
	( (u_long64)(tag1) | ((u_long64)(tag2)<<4) | ((u_long64)(tag3)<<8) | ((u_long64)(tag4)<<12) | ((u_long64)(tag5)<<16) | ((u_long64)(tag6)<<20) | ((u_long64)(tag7)<<24) | ((u_long64)(tag8)<<28) | ((u_long64)(tag9)<<32) | ((u_long64)(tag10)<<36 | ((u_long64)(tag11)<<40) | ((u_long64)(tag12)<<44) | ((u_long64)(tag13)<<48) | ((u_long64)(tag14)<<52) | ((u_long64)(tag15)<<56 | (u_long64)(tag16)<<60) )

#define GIF_SET_REGS6(tag1, tag2, tag3, tag4, tag5, tag6) \
	( (u_long64)(tag1) | ((u_long64)(tag2)<<4) | ((u_long64)(tag3)<<8) | ((u_long64)(tag4)<<12) | ((u_long64)(tag5)<<16) | ((u_long64)(tag6)<<20) )

#define GIF_SET_REGS5(tag1, tag2, tag3, tag4, tag5) \
	( (u_long64)(tag1) | ((u_long64)(tag2)<<4) | ((u_long64)(tag3)<<8) | ((u_long64)(tag4)<<12) | ((u_long64)(tag5)<<16) )

#define GIF_SET_REGS4(tag1, tag2, tag3, tag4) \
	( (u_long64)(tag1) | ((u_long64)(tag2)<<4) | ((u_long64)(tag3)<<8) | ((u_long64)(tag4)<<12) )

#define GIF_SET_REGS3(tag1, tag2, tag3) \
	( (u_long64)(tag1) | ((u_long64)(tag2)<<4) | ((u_long64)(tag3)<<8))

#define GIF_SET_REGS2(tag1, tag2) \
	( (u_long64)(tag1) | ((u_long64)(tag2)<<4) )


#define GOURAUD(x) (x)
#define TEXTURED(x) (x)
#define EOP(x) (x)
#define PRE(x) (x)
#define NREG(x) (x)
#define NLOOP(x) (x)

#define FLOAT_ONE (0x3f800000)

#define GIF_PACKED  (0x00)

#define PS2_GS_AD (0x0e)

#define S_32 (0x60)
#define S_16 (0x61)
#define S_8 (0x62)
#define V2_32 (0x64)
#define V2_16 (0x65)
#define V2_8 (0x66)
#define V3_32 (0x68)
#define V3_16 (0x69)
#define V3_8 (0x6A)
#define V4_32 (0x6C)
#define V4_16 (0x6D)
#define V4_8 (0x6E)
#define V4_5 (0x6F)

#define VIF1_SET_MSCAL(vuaddr, irq) ((u_int)(vuaddr) | ((u_int)0x14 << 24) | ((u_int)(irq) << 31))
#define VIF1_SET_MSCNT(irq) (((u_int)0x17 << 24) | ((u_int)(irq) << 31))
#define VIF1_SET_MSCALF(vuaddr, irq) ((u_int)(vuaddr) | ((u_int)0x15 << 24) | ((u_int)(irq) << 31))

#define VIF1_SET_STCYCL(wl, cl, irq) ((u_int)(cl) | ((u_int)(wl) << 8)  | ((u_int)0x01 << 24) | ((u_int)(irq) << 31))
#define VIF1_SET_UNPACK(vuaddr, num, cmd, irq) ((u_int)(vuaddr) | ((u_int)(num) << 16) | ((u_int)(0x60 | (cmd)) << 24) | ((u_int)(irq) << 31))

#endif

