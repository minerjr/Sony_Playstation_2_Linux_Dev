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

#include "PS2Utils.h"
#include "PS2Math.h"
#include "Joypad.h"
#include "TextureManager.h"
#include "Screenshot.h"

using namespace NablaPS2Utils;

typedef int				ColorVector4[4];
typedef float			TexCoordinateSTQ[3];

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
	{0, 0, 128, 127},
	{0, 0, 128, 127},
	{128, 0, 0, 127},
	{128, 0, 0, 127},
	{128, 0, 0, 127},
	{0, 0, 128, 127},
	{0, 0, 128, 127},
	{0, 0, 128, 127},
	{0, 0, 128, 127},
	{128, 0, 0, 127},
	{128, 0, 0, 127},
	{128, 0, 0, 127},
	{0, 0, 128, 127},
	{0, 0, 128, 127},
};

//! Texture coordinate of the cube in STQ format.
TexCoordinateSTQ TextureCoordinates[] = {
	{1.0, 0.0, 1.0},
	{1.0, 1.0, 1.0},
	{0.0, 0.0, 1.0},
	{0.0, 1.0, 1.0},
	{1.0, 1.0, 1.0},
	{0.0, 0.0, 1.0},
	{1.0, 0.0, 1.0},
	{0.0, 1.0, 1.0},
	{1.0, 1.0, 1.0},
	{0.0, 0.0, 1.0},
	{1.0, 0.0, 1.0},
	{1.0, 1.0, 1.0},
	{0.0, 0.0, 1.0},
	{0.0, 1.0, 1.0},
};

//! This is used to make the user choose texture function. Only decal and modulate can be chosen
int TextureFunction			= GS_TEX0_TFX_MODULATE;
//! Set Filtering used
int TextureFilteringMode	= GS_TEX1_MMAG_LINEAR;
//! Set clamp function
int TextureClampMode			= GS_CLAMP_MODE_REPEAT;
//! This is used to modify texture coordinates, at the beginning does not modify
float TetureCoordinateModifier = 1.0f;

/** This function creates DMA packet to send cube information to the GS.
	 This routine build DMA in this way:
	 1) Build a pack that set texture values to be used, this is done
	 by TextureManager class, then cube data are sent.
	 @param pvBase Address to use to build the packet
	 @param TransformationMatrix Matrix that stores world, view and 
			  projection transform, used to transform vertex of the cube.
*/
void MakeDMAPack(void *pvBase, 
					 const PS2Math::Matrix44 &TransformationMatrix, 
					 const TextureManager &TM, 
					 int   TexCoordinateMode) { 

   //Cast to QWORD to help build the packet
	PS2_QWORD *DMAPacket = (PS2_QWORD *) pvBase;
	DMAPacket++->ul128 = PS2_DMA_SET_HEADER(4, 0, DMA_ID_SOURCE_CNT, 0, 0, 0);
	//First of all cast void memory area to specific GIFTag structure
   sceGifTag *pGIFTag = (sceGifTag *)(DMAPacket);
	memset(pGIFTag, 0, sizeof(sceGifTag));
	//Then specify that only two registers TEX0 and TEX1 are to be set.
	DMAPacket++->ul64[0] = PS2_GIF_SET_TAG(3, 0, 0, 0, GIF_FLG_PACKED, 1);
	pGIFTag->REGS0 = GIF_REG_A_D;
	//Choose texture function: modulate, decal, etc, this parameter has to be 
	//passed to TextureManager BuildTex0 function.
	DMAPacket->ul64[0] = TM.BuildTEX0(TextureFunction);
	DMAPacket++->ul64[1] = GIF_A_D_REG_TEX0_1;
	DMAPacket->ul64[0] = TM.BuildTEX1(TextureFilteringMode);
	DMAPacket++->ul64[1] = GIF_A_D_REG_TEX1_1;
	DMAPacket->ul64[0] = PS2_GIF_SET_CLAMP(TextureClampMode, TextureClampMode, 0, 0, 0, 0);
	DMAPacket++->ul64[1] = GIF_A_D_REG_CLAMP_1;

	//Remember that every vertex is composed by 3 component, RGBQ, STQ, XYZ2, this last
	//packet is of type END because it ends transfer with GS
	int VertexNum = sizeof(Cube) / sizeof(*Cube);
	DMAPacket++->ul128 = PS2_DMA_SET_HEADER(VertexNum * 3 + 1, 0, DMA_ID_SOURCE_END, 0, 0, 0);
	pGIFTag = (sceGifTag *)(DMAPacket);
	memset(pGIFTag, 0, sizeof(*pGIFTag));
	DMAPacket++->ul64[0] = PS2_GIF_SET_TAG(VertexNum, 1, 1, 0, GIF_FLG_PACKED, 3);
	//Sets prim accordingly
	pGIFTag->PRIM	= PS2_GS_SET_PRIM(GS_PRIM_PRIM_TRISTRIP, 
												GS_PRIM_IIP_GOURAUD,
												GS_PRIM_TME_TEXON,
												GS_PRIM_FGE_FOGOFF,
												GS_PRIM_ABE_ALPHAOFF,
												GS_PRIM_AA1_AAOFF,
												TexCoordinateMode,
												GS_PRIM_CTXT_1,
												GS_PRIM_FIX_UNFIXED);

	//First of all texture coordinates, 
	if (GS_PRIM_FST_STQ == TexCoordinateMode) {
		
		pGIFTag->REGS0 = GIF_REG_ST;
	} 
	else {
	
		pGIFTag->REGS0 = GIF_REG_UV;
	}
	//then color and at last vertex that fires rendering of the triangle
	pGIFTag->REGS1 = GIF_REG_RGBAQ;
	pGIFTag->REGS2 = GIF_REG_XYZ2;

	//Build cube data
	PS2Math::Vector4 VOut;
	int	FPVertex[4];
	float	STQ[4]			= {0.0f};
	int	UV[4]				= {0};
	for(int VertexIndex = 0; VertexIndex < VertexNum; ++VertexIndex) {

		//First of all transform the vertices, this because we need 1/w value
		//to apply texture perspective correction.
		VOut = TransformationMatrix * Cube[VertexIndex];
		float RecW = VOut.ConvertToFixedPoint(FPVertex);

		//User chan choose with L3 wich coordinate system are to be used.
		if (GS_PRIM_FST_STQ == TexCoordinateMode) {
		

			//Calculate S/W, T/W, 1/W, and store value into DMAPACK
			STQ[0] = TextureCoordinates[VertexIndex][0] * TetureCoordinateModifier * RecW;
			STQ[1] = TextureCoordinates[VertexIndex][1] * TetureCoordinateModifier * RecW;
			STQ[2] = RecW;
			DMAPacket++->ul128 = *(sps2uint128 *) STQ;
		} else {
		
			//Calculate texture in UV format, remember that UV is fixed point 12:4
			UV[0] = (int) (TextureCoordinates[VertexIndex][0] * TetureCoordinateModifier * TM.Width()) << 4;
			UV[1] = (int) (TextureCoordinates[VertexIndex][1] * TetureCoordinateModifier * TM.Height()) << 4;
			DMAPacket++->ul128 = *(sps2uint128 *) UV;
		}
		DMAPacket++->ul128 = *(sps2uint128 *) CubeColor[VertexIndex];
		DMAPacket++->ul128 = *(sps2uint128 *) FPVertex;
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

	//Black background
	sps2UScreenClear(0x0, 0, 0x0);
	sps2UScreenSwap();

	//Value to store rotation angle of the cube
	float RotX = 0; 
	float RotY = 0; 

	//MAtrix used to work with geometry
	PS2Math::Matrix44 ViewMatrix;
	PS2Math::Matrix44 ProjMatrix;

	//Build ViewMatrix to look at the cube
	PS2Math::Vector4 EyePos(0, 0, 20);
	PS2Math::Vector4 AtPos(0, 0, 0);
	PS2Math::Vector4 UpPos(0, 1, 0);

	//Now build Projection Matrix
	float xyAspectRatio = 4.0f / 3.0f; 
	ProjMatrix.PerspectiveLH(1 * xyAspectRatio, 1, 1, 65536.0f);
	int wpWidth  = sps2UScreenGetWidth();
	int wpHeight = sps2UScreenGetHeight();
	printf("wpWidth = %d wpHeight = %d\n", wpWidth, wpHeight);
	ProjMatrix.MapToViewPort(2048-  wpWidth / 2, 2048 + wpWidth / 2, 
		2048 + wpHeight / 2, 2048 - wpHeight / 2, 
		16777215.000000, 0);

	//Init the joypad
	JoypadManager Joy(0);
	if (!Joy)  {
	
		std::cout << "Unable to access joypad 0\n";
		exit(1);
	}

	//!Enable analog mode and actuator functions.
	Joy.SetAnalogMode(true);
	if (Joy.AnalogButtonSupported()) {
	
		Joy.SetAnalogButtonStatus(true);
	}
	Joy.EnableActuators();

	//Create a matrix object that will represent cube position in space
	PS2Math::Matrix44 WorldMatrix(true);

	//Load the texture from file, and check if all went ok.
	TextureManager TM("mammuth.ps2ntx", iSPS2Descriptor);
	if (!TM) {
		
		std::cout << "Unable to access " << "mammuth.ps2ntx" << "\n";
		exit(2);
	}
	else {
	
		//If loading went good, upload the texture to GS, this can be done
		//olny once because this simple example will use only one texture.
		TM.SendTexture(sps2UScreenGetFirstFreeGSPage() << 5, pMemory);
	}
	//DumpDMAPack(pMemory->pvStart, 40); //Dump pack if needed,

	//Init screenshot class.
	ScreenShot SH(iSPS2Descriptor);
	//Set defalult Addressing mode to STQ.
	int TexturingAddressMode = GS_PRIM_FST_STQ;

	for (; ;) {

		//Give moviment to the cube
		Joy.ReadPadData();

		//!L3 change texturing address mode from STQ to UV, remember that these paramters
		//are respectively of int value 0 and 1 so it is simple swap between one and another.
		if (Joy.Button(L3).State == Pressed) TexturingAddressMode = TexturingAddressMode ? 0 : 1;
		if (Joy.Button(TRIANGLE).State == Pressed) TextureFunction = TextureFunction ? 0 : 1;
		if (Joy.Button(SQUARE).State == Pressed) TextureFilteringMode = TextureFilteringMode ? 0 : 1;
		if (Joy.Button(CROSS).State == Pressed) TextureClampMode = TextureClampMode ? 0 : 1;

		//Check for texture coordinate modifiers.
		if (Joy.Button(R1).State == MPressed) {
		
			TetureCoordinateModifier += 0.02f;
		}
		else if (Joy.Button(R2).State == MPressed) {
			
			TetureCoordinateModifier -= 0.02f;
		}

		//pressing of start button makes the program end.
		if (Joy.Button(START).State == Pressed) break;

		//Apply movement to the cube
		float Increment = (float)M_PI / 50.0f;
		RotX = -Increment * Joy.AnalogStick(LY);
		RotY = Increment * Joy.AnalogStick(LX);
		
		//Remember to add this rotation value to current world matrix
		WorldMatrix.AddRotationX(RotX);
		WorldMatrix.AddRotationY(RotY);
		PS2Math::Matrix44 TransformationMatrix;

		//Set view At vector, this is done to give sense of vibration
		AtPos[0] = 0;
		AtPos[1] = 0;
		if (Joy.Button(CIRCLE).State != Unpressed) {	
				
				float PL = Joy.Button(CIRCLE).PressureLevel;
				AtPos.x = (rand() / (float) RAND_MAX - 0.5f) * PL;
				AtPos.y = (rand() / (float) RAND_MAX - 0.5f) * PL;
				Joy.SetActuatorVariable(
					(unsigned char) (Joy.Button(CIRCLE).PressureLevel * 255));
		}

		//Handle L1 and L2 to make camera move in Z axys and then move on and over
		//the cube
		float ZShift = 0;
		if (Joy.Button(L1).State == MPressed) {
		
			ZShift = -Joy.Button(L1).PressureLevel;
		}
		else if (Joy.Button(L2).State == MPressed) {
			
			ZShift = Joy.Button(L2).PressureLevel;
		}
		EyePos.z += ZShift;

		//Build view matrix.
		ViewMatrix.ViewLH(EyePos, AtPos, UpPos);

		//Remember the correct order right to left because column vector
		//notation is used.
		TransformationMatrix = ProjMatrix * ViewMatrix * WorldMatrix;

		//Make Packet, this time DMA header is not necessary because we
		//directly instruct DMAC what to do.
		MakeDMAPack(pMemory->pvStart, TransformationMatrix, TM, TexturingAddressMode); 
		//DumpDMAPack(pMemory->pvStart, 30);

		//Black background
		sps2UScreenClear(0x0, 0, 0x0);

		//Transfer data with source chain mode.
		*EE_D2_QWC	= 0;
		*EE_D2_TADR = PS2_DMA_SET_TADR( 
			sps2GetPhysicalAddress(pMemory->pvStart, pMemory), 0);
		*EE_D2_CHCR = PS2_DMA_SET_CHCR(DMA_CHCR_DIR_TOMEMORY,
									DMA_CHCR_MOD_CHAIN,
									DMA_CHCR_ASP_NOADDRESS,
									DMA_CHCR_TTE_NOTRANSFER_DMATAG,
									DMA_CHCR_TIE_INTDISABLE,
									DMA_CHCR_STR_STARTTRANSFER,
									0);
		sps2WaitForDMA(2, iSPS2Descriptor);
		sps2UScreenSwap();
		if (Joy.Button(R3).State == Pressed) {
			
			static int	ShotIndex = 1;
			char			buffer[255];
			sprintf(buffer, "./Shot%3d.tga", ShotIndex++);
			SH.TakeShot(buffer, NTSC);
		}
	}

	//! Shut down the actuators.
	Joy.SetActuatorVariable(0);
	Joy.SetActuatorConst(false);

  //Shut down the screen
  sps2UScreenShutdown();

  //Free memory used to store DMA packet
  sps2Free(pMemory);

  //Close library
  sps2Release(iSPS2Descriptor);

	return 0;
}
