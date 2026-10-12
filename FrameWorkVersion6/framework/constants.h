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

#define GOURAUD(x) (x)
#define TEXTURED(x) (x)
#define EOP(x) (x)
#define PRE(x) (x)
#define NREG(x) (x)
#define NLOOP(x) (x)

#define FLOAT_ONE (0x3f800000)

#define GIF_PACKED  (0x00)

#define V2_32 (0x64)
#define V3_32 (0x68)
#define V4_32 (0x6C)

#define VIF1_SET_MSCAL(vuaddr, irq) ((u_int)(vuaddr) | ((u_int)0x14 << 24) | ((u_int)(irq) << 31))
#define VIF1_SET_MSCNT(irq) (((u_int)0x17 << 24) | ((u_int)(irq) << 31))
#define VIF1_SET_MSCALF(vuaddr, irq) ((u_int)(vuaddr) | ((u_int)0x15 << 24) | ((u_int)(irq) << 31))

#define VIF1_SET_STCYCL(wl, cl, irq) ((u_int)(cl) | ((u_int)(wl) << 8)  | ((u_int)0x01 << 24) | ((u_int)(irq) << 31))
#define VIF1_SET_UNPACK(vuaddr, num, cmd, irq) ((u_int)(vuaddr) | ((u_int)(num) << 16) | ((u_int)(0x60 | (cmd)) << 24) | ((u_int)(irq) << 31))

#endif

