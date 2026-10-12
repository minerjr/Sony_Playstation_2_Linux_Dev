#include "addregisters.h"

// Put all our functions for adding registers in here.
// C++ Tip: Usually you don't want to put functions inside a header file
// however these are inline functions. What this means is that the compiler
// treats these like macros. So these functions are not actually called,
// instead the compiler replaces each call to these functions with the actual
// code inside the function. The advantage of inline functions over
// macros is that the compiler will check the parameters for you to make
// sure that you don't try to pass something of the wrong type into the function.

// This function packs RGBA data into a quadword of data, then moves the pointer on 128 bits
// C++ Tip: *& means a reference to a pointer, this means that the actual pointer is passed
// into the function instead of a copy. If we left this out, pReg++ would increment the copy
// and the actual pointer we passed in would keep its original value, which is not what we want.

// This has been modified by David Froy 2004
// borrowed from Henry Fortuna 2004

sps2GIFTag_t AddGifTag(int NLOOP, int PRE, int EOP, int PRIM, GIF_FLG_t FLG, int NREG)
{
	sps2GIFTag_t pGIFTag;
	// Prepare the GIF tag  
  	pGIFTag.i128=0;       			// Blank it out
  	pGIFTag.s.NLOOP=NLOOP;    			// 0 entries.  We'll increment as we add entries
  	pGIFTag.s.PRE=PRE;      			// We are providing a valid PRIM value
	pGIFTag.s.EOP=EOP;      			// End of packet -- draw me please!
 	pGIFTag.s.PRIM=PRIM;   		 	// 3 = triangle | 8 = gourard shading
 	pGIFTag.s.FLG=FLG;	// We're using the packed format for data
 	pGIFTag.s.NREG=NREG;    			// Start off with zero registers, increment as we
                         			// populate the descriptions
	return pGIFTag;
}

sps2GIFPackedRegister_t AddColour(int R, int G, int B, int A)
{
	sps2GIFPackedRegister_t pReg;
	pReg.i128 = 0;
	pReg.RGBAQ.s.R = R;
	pReg.RGBAQ.s.G = G;
	pReg.RGBAQ.s.B = B;
	pReg.RGBAQ.s.A = A;
	return pReg;
}

sps2GIFPackedRegister_t AddSTQ( float S, float T, float Q)
{
	sps2GIFPackedRegister_t pReg;
	pReg.i128 = 0;
	pReg.ST.s.S = S;
	pReg.ST.s.T = T;
	pReg.ST.s.Q = Q;
	return pReg;
}

// Add a UV packed QWORD
sps2GIFPackedRegister_t AddUV(int U, int V)
{
	// UV coordinates are given in 12:4 fixed point, so we shift the value 4 bits left
	sps2GIFPackedRegister_t pReg;
	pReg.i128 = 0;
	pReg.UV.s.U = (U << 4);
	pReg.UV.s.V = (V << 4);
	return pReg;
}

// This function packs vertex data into a quadword of data
sps2GIFPackedRegister_t AddVertex(int X, int Y, int Z, int ADC)
{
	sps2GIFPackedRegister_t pReg;
	pReg.i128 = 0;

	// The center of the screen is at 2048, 2048
	// Also vertex positions must be given in 12:4 fixed point which is why
	// we shift the values left by 4 bits
	pReg.XYZ2.s.X = (2048 + X) << 4;
	pReg.XYZ2.s.Y = (2048 + Y) << 4;
	// The Z value can be any number between 1 and 2^32 - 1. The
	// higher the value, the closer to the screen
	pReg.XYZ2.s.Z = Z;

	pReg.XYZ2.s.ADC = ADC;

	return pReg;
}

sps2GIFPackedRegister_t AddVertexFP(int X, int Y, int Z, int ADC)
{
	sps2GIFPackedRegister_t pReg;
	pReg.i128 = 0;

	pReg.XYZ2.s.X = X + (2048 << 4);
	pReg.XYZ2.s.Y = Y + (2048 << 4);
	// The Z value can be any number between 1 and 2^24 - 1. The
	// higher the value, the closer to the screen
	pReg.XYZ2.s.Z = Z;

	pReg.XYZ2.s.ADC = ADC;

	return pReg;
}


sps2GIFPackedRegister_t AddVertexFPo(int X, int Y, int Z, int ADC)
{
	sps2GIFPackedRegister_t pReg;
	pReg.i128 = 0;

	pReg.XYZ2.s.X = X;
	pReg.XYZ2.s.Y = Y;
	// The Z value can be any number between 1 and 2^24 - 1. The
	// higher the value, the closer to the screen
	pReg.XYZ2.s.Z = Z;
	
	// 0=Draw    1=Clip
	pReg.XYZ2.s.ADC = ADC;

	return pReg;
}

// Add an Address and Data QWORD
sps2GIFPackedRegister_t AddA_D(uint64 DATA, GIF_A_D_REG_t iAddr)
{
	sps2GIFPackedRegister_t pReg;
	
	pReg.i128 = 0;
	pReg.A_D.s.ADDR = iAddr;
	pReg.A_D.s.DATA = DATA;
	return pReg;
}

