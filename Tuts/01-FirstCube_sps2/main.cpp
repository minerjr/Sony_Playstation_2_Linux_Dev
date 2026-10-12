/** @file main.cpp
Contains the real code that draws cube on the screen
*/
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <memory.h>

#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>

#include "PS2Math.h"

#define SCE_GS_SET_PRIM(PRIM, IIP, TME, FGE, ABE, AA1, FST, CTXT, FIX) \
	((sps2uint32)(PRIM) | ((sps2uint32)(IIP) << 3) | \
	((sps2uint32)(TME) << 4) | ((sps2uint32)(FGE) << 5) | \
	((sps2uint32)(ABE) << 6)  | ((sps2uint32)(AA1) << 7) | \
	((sps2uint32)(FST) << 8) | ((sps2uint32)(CTXT) << 9) | ((sps2uint32)(FIX) << 10))

typedef int				ColorVector4[4];

typedef struct {
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
} sceGifTag;

//! This is the vector containing vertices of the cube
PS2Math::Vector4 Cube[] = {	
	PS2Math::Vector4(-5.0, -5.0, 5.0, 1.0),
	PS2Math::Vector4(-5.0, -5.0, -5.0, 1.0),
	PS2Math::Vector4(-5.0, 5.0, 5.0, 1.0),
	PS2Math::Vector4(-5.0, 5.0, -5.0, 1.0),
	PS2Math::Vector4(5.0, 5.0, -5.0, 1.0),
	PS2Math::Vector4(-5.0, -5.0, -5.0, 1.0),
	PS2Math::Vector4(5.0, -5.0, -5.0, 1.0),
	PS2Math::Vector4(-5.0, -5.0, 5.0, 1.0),
	PS2Math::Vector4(5.0, -5.0, 5.0, 1.0),
	PS2Math::Vector4(-5.0, 5.0, 5.0, 1.0),
	PS2Math::Vector4(5.0, 5.0, 5.0, 1.0),
	PS2Math::Vector4(5.0, 5.0, -5.0, 1.0),
	PS2Math::Vector4(5.0, -5.0, 5.0, 1.0),
	PS2Math::Vector4(5.0, -5.0, -5.0, 1.0),
};

//! This is the vector containing color for all the vertex of the cube
ColorVector4 CubeColor[] = {
	{255, 0, 0, 127},
	{255, 0, 0, 127},
	{0, 255, 0, 127},
	{0, 255, 0, 127},
	{0, 255, 0, 127},
	{255, 0, 0, 127},
	{255, 0, 0, 127},
	{255, 0, 0, 127},
	{255, 0, 0, 127},
	{0, 255, 0, 127},
	{0, 255, 0, 127},
	{0, 255, 0, 127},
	{255, 0, 0, 127},
	{255, 0, 0, 127},
};

/** This function creates DMA packet to send cube information to the GS.
	 This is different from the routine used with libps2dev because there
	 is no need to make DMA tag. This because in this example DMAC is 
	 directly controlled by the application.
*/
void MakeDMAPack(void *pvBase, 
					  const PS2Math::Matrix44 &TransformationMatrix) {

	//First of all cast void memory area to specific GIFTag structure
	sceGifTag *pGIFTag= static_cast<sceGifTag *>(pvBase);
	sps2uint128	 *VertexData=
		reinterpret_cast<sps2uint128 *>((int) pvBase + sizeof(sceGifTag));
	
	int VertexNum = sizeof(Cube) / sizeof(Cube[0]);

	//Now the header of the gif Tag is to be prepared
	memset(pGIFTag, 0, sizeof(*pGIFTag)); // First of all Reset all content of the tag
	pGIFTag->NLOOP	= VertexNum;		// 0 entries.  We'll increment as we add entries
	pGIFTag->PRE	= 1;					// Gif Tag contain valid PRIM settings
	pGIFTag->EOP	= 1;				   // Last primitive to be sent. 
	pGIFTag->PRIM	= SCE_GS_SET_PRIM(4, 1, 0, 0, 0, 0, 0, 0, 0);
	pGIFTag->FLG	= GIF_FLG_PACKED; // Packet data
	pGIFTag->NREG	= 2;					// RGBAQ + XYZW2
	
	pGIFTag->REGS0 = GIF_REG_RGBAQ;
	pGIFTag->REGS1 = GIF_REG_XYZ2;

	//OK! Now it remains only to write down the values of transformed vertex
	PS2Math::Vector4 VOut;
	int FPVertex[4];
	int DMAPackIndex = 0;
	for(int VertexIndex = 0; VertexIndex < VertexNum; ++VertexIndex) {

		VOut = TransformationMatrix * Cube[VertexIndex];
		VOut.ConvertToFixedPoint(FPVertex);
		VertexData[DMAPackIndex++] = *(sps2uint128 *) CubeColor[VertexIndex];
		VertexData[DMAPackIndex++] = *(sps2uint128 *) FPVertex;
	}
}



/** Simple function to dump DMA pack.
	 To avoid crashing console there is the need to dump DMA packet before
	 actually send to the DMAC. This routine simply take a memory address
	 and dump in hex format a number of Num 16Byte Words.
	 
	 @param Data Pointer to memory to dump.
	 @param Num  Number of 128 bit word to dump
*/
void DumpDMAPack(void *Data, int Num) {

	//D is a pointer to sps2uint32[4] element.
	sps2uint32 (*D)[4] = (sps2uint32 (*)[4]) Data;
	for (int I = 0; I < Num; ++I) {
	
		printf("%d:\t%X %X %X %X\n", I, 
			D[I][3], D[I][2], D[I][1], D[I][0]);
	}
}

//----------------------------------------------------------------------
int main(int argc, char *argv[]) {

	int iSPS2Descriptor;      // The handle we use for sps2
	sps2Memory_t *pMemory;    // Will store information about memory we allocate
	Dn_CHCR_t chcrValue;      // This is used to control the DMA transfer

  //CHCR register is used to start DMA transfer, prepare a simple 
  //tranfer in normal mode.
  chcrValue.i32=0;                  // Clear content of the register
  chcrValue.s.MOD=CHCR_MOD_NORMAL;  // Select Normal Transfer
  chcrValue.s.STR=1;                // STR=1 makes transfer start
	
  //Init sps2 Library, this function return a handle that will be used
  //to access sps2 functions.
  iSPS2Descriptor=sps2Init();       

  //Allocate enough memory to fit the DMA packet, remember that allocation
  //granularity is 4096 byte. For simple cube a single block is enough.
  pMemory=sps2Allocate(4096, SPS2_MAP_BLOCK_4K | SPS2_MAP_UNCACHED,
		       iSPS2Descriptor);

  // Initialize the screen.  Zero tells sps2UScreenInit to exit gracefully
  // when it receives a signal.
  sps2UScreenInit(0);

	//Value to store rotation angle of the cube
	float RotX = 0; 
	float RotY = 0; 
	float RotZ = 0;

	//MAtrix used to work with geometry
	PS2Math::Matrix44 ViewMatrix;
	PS2Math::Matrix44 ProjMatrix;

	//Build ViewMatrix to look at the cube
	PS2Math::Vector4 EyePos(0, 0, 20);
	PS2Math::Vector4 AtPos(0, 0, 0);
	PS2Math::Vector4 UpPos(0, 1, 0);
	ViewMatrix.ViewLH(EyePos, AtPos, UpPos);

	//Now build Projection Matrix
	float xyAspectRatio = 4.0f / 3.0f; 
	ProjMatrix.PerspectiveLH(1 * xyAspectRatio, 1, 1, 65536.0f);
	int wpWidth  = sps2UScreenGetWidth();
	int wpHeight = sps2UScreenGetHeight();
	printf("wpWidth = %d wpHeight = %d\n", wpWidth, wpHeight);
	ProjMatrix.MapToViewPort(2048-  wpWidth / 2, 2048 + wpWidth / 2, 
		2048 + wpHeight / 2, 2048 - wpHeight / 2, 
		16777215.000000, 0);

	float Increment = (float)M_PI / 180.0f;
	for (int O = 0; ;O++) {

		//Give moviment to the cube
		RotX += Increment;
		RotY += Increment * 2;
		RotZ += Increment * 3;

		PS2Math::Matrix44 WorldMatrix(RotX, PS2Math::X_AXIS);
		WorldMatrix.AddRotationY(RotY);
		WorldMatrix.AddRotationZ(RotZ);
		
		PS2Math::Matrix44 TransformationMatrix;
		//Remember the correct order right to left because column vector
		//notation is used.
		TransformationMatrix = ProjMatrix * ViewMatrix * WorldMatrix;

		//Make Packet, this time DMA header is not necessary because we
		//directly instruct DMAC what to do.
		MakeDMAPack(pMemory->pvStart, TransformationMatrix); 

		//Black background
		sps2UScreenClear(0x0, 0, 0x0);

		//Begin DMA transfer, set in QWC register number of Word
		//to send
		*EE_D2_QWC = (sizeof(Cube) / sizeof(Cube[0])) * 2 + 1;

		//Set the address of the buffer to transfer, remember that
		//real memory address must be calculated
		*EE_D2_MADR = sps2GetPhysicalAddress(pMemory->pvStart, pMemory);

		//Make dump of the first 10 word, 
		DumpDMAPack(pMemory->pvStart, 10); 
		   
		//Start the DMA transfer
		*EE_D2_CHCR = chcrValue.i32;

		//Wait for the DMA transfer to finish
		sps2WaitForDMA(2, iSPS2Descriptor);

		//Swap displays now that we're done
		sps2UScreenSwap();

	}

  //Shut down the screen
  sps2UScreenShutdown();

  //Free memory used to store DMA packet
  sps2Free(pMemory);

  //Close library
  sps2Release(iSPS2Descriptor);

	return 0;
}
