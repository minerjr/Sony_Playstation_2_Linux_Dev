#include "sps2inc.h"
#include "structs.h"

//#ifndef __ADDREGISTERS_H__
//#define __ADDREGISTERS_H__

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

//This has been modified by David Froy 2004
//borrowed from Henry Fortuna 2004

extern sps2GIFTag_t AddGifTag(int NLOOP, int PRE, int EOP, int PRIM, GIF_FLG_t FLG, int NREG);

extern sps2GIFPackedRegister_t AddColour(int R, int G, int B, int A);

extern sps2GIFPackedRegister_t AddSTQ( float S, float T, float Q);

// Add a UV packed QWORD
extern sps2GIFPackedRegister_t AddUV(int U, int V);

// This function packs vertex data into a quadword of data
extern sps2GIFPackedRegister_t AddVertex(int X, int Y, int Z, int ADC);

extern sps2GIFPackedRegister_t AddVertexFP(int X, int Y, int Z, int ADC);

extern sps2GIFPackedRegister_t AddVertexFPo(int X, int Y, int Z, int ADC);

// Add an Address and Data QWORD
extern sps2GIFPackedRegister_t AddA_D(uint64 DATA, GIF_A_D_REG_t iAddr);

//#endif
