/** @file main.cpp
	 @author Ricci Gian Maria.
	 @version 1.0
	This code shows how to draw a simple textured cube on the screen
	using Vector Unit 1 to transform the vertices.
*/
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
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
typedef float			TexCoordinateSTQ[4];

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
	{16, 16, 16, 127},
	{16, 16, 16, 127},
	{128, 128, 128, 127},
	{128, 128, 128, 127},
	{128, 128, 128, 127},
	{16, 16, 16, 127},
	{16, 16, 16, 127},
	{16, 16, 16, 127},
	{16, 16, 16, 127},
	{128, 128, 128, 127},
	{128, 128, 128, 127},
	{128, 128, 128, 127},
	{16, 16, 16, 127},
	{16, 16, 16, 127},
};

//! Texture coordinate of the cube in STQ format.
TexCoordinateSTQ TextureCoordinates[] = {
	{1.0, 0.0, 1.0, 0.0},
	{1.0, 1.0, 1.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 1.0, 1.0, 0.0},
	{1.0, 1.0, 1.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{1.0, 0.0, 1.0, 0.0},
	{0.0, 1.0, 1.0, 0.0},
	{1.0, 1.0, 1.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{1.0, 0.0, 1.0, 0.0},
	{1.0, 1.0, 1.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 1.0, 1.0, 0.0},
};

//! This is used to make the user choose texture function. Only decal and modulate can be chosen
int TextureFunction			= GS_TEX0_TFX_MODULATE;
//! Set Filtering used
int TextureFilteringMode	= GS_TEX1_MMAG_LINEAR;
//! Set clamp function
int TextureClampMode			= GS_CLAMP_MODE_REPEAT;
//! This is used to modify texture coordinates, at the beginning does not modify
float TetureCoordinateModifier = 1.0f;

/** This function build DMA packet to send data to the VIF and then to VU1.
	 Remember that this is not enough to render the cube, even if all vertex
	 operations are now executed by vector unit, GS settings must me send to GS
	 so rememeber to build also appropriate GS Packet.
	 @param pvBase		Address of sps2 allocated memory to build the packet
	 @param TransformationMatrix PS2Math::Matrix44 object that represent
			  transformation matrix, this is used by VU1 to transform 
			  vertices coordinates
    @param TexCoordinateMode Texture coordinate used, this is necessary
		     here because VIF packet contain GIF tag that contain PRIM
			  setting. In this example (U,V) coordinate system are used
			  to perform texture perspective correction.
*/
void MakeVIFPacket(void *pvBase, 
						 const PS2Math::Matrix44 &TransformationMatrix, 
						 int   TexCoordinateMode) {

   //Cast to QWORD to help building the packet
	PS2_QWORD *DMAPacket = (PS2_QWORD *) pvBase;

	//Packet contain 4 QWORD represeting transformation matrix, then
	//QWORD representing GIFTag, then 3 QWORD for each vertex because
	//each vertex is composed by coordinate, color and texturecoordinate
	int VertexNum		= sizeof(Cube) / sizeof(*Cube);
	int VIFPacketSize = 4 + 1 + VertexNum * 3;

	//Does not increment because we will transfer TAG data and so VIF
	//Code is to be included into DMA HEADER upper 64 bits.
	DMAPacket->ul128 = PS2_DMA_SET_HEADER(VIFPacketSize + 1, 0, 
													  DMA_ID_SOURCE_END, 0, 0, 0);
	//Shift by 96 bit will make this vif code to fit in the last 
	//word of VIF header
	DMAPacket->ul128 |= (sps2uint128) PS2_DMA_SET_VIFCODE_UNPACK(0, 
															VIF_UNPACK_SIGNED, 
															VIF_UNPACK_TO_ADDR, 
															VIFPacketSize, 
															V4_32) << 96;
	DMAPacket++;
	//Put matrix into the packet
	DMAPacket++->ul128 = *(sps2uint128 *) TransformationMatrix[0];
	DMAPacket++->ul128 = *(sps2uint128 *) TransformationMatrix[1];
	DMAPacket++->ul128 = *(sps2uint128 *) TransformationMatrix[2];
	DMAPacket++->ul128 = *(sps2uint128 *) TransformationMatrix[3];

	//then GIFTag
	sceGifTag *pGIFTag = (sceGifTag *)(DMAPacket);
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

	//Now all the vertex data
	for (int I = 0; I < VertexNum; ++I) 	
		DMAPacket++->ul128 = *(const sps2uint128 *) Cube[I];
	for (int I = 0; I < VertexNum; ++I) 
		DMAPacket++->ul128 = *(sps2uint128 *) CubeColor[I];
	for (int I = 0; I < VertexNum; ++I) 
		DMAPacket++->ul128 = *(sps2uint128 *) TextureCoordinates[I];

	//Now Flush and execute microprogram, remember that this actions are
	//not executed now, but only when this packet will be sent to the 
	//Vif
	DMAPacket->ul128		 = 0;
	DMAPacket->ul32[0]	 = PS2_DMA_SET_VIFCODE_FLUSH;
	DMAPacket->ul32[1]	 = PS2_DMA_SET_VIFCODE_MSCAL(0);
}

/** Build GIF Packet to set texturing and other settings of PS2.
    This is the same as the first part of the packet of preceding examples, 
	 texture coordinate mode and uploaded texture is chosen.
*/
void SetGSEnv(sps2Memory_t *DMAPackAddress, 
				  const TextureManager &TM, 
				  int iSPS2Descriptor) { 

   //Cast to QWORD to help build the packet
	PS2_QWORD *DMAPacket = (PS2_QWORD *) DMAPackAddress->pvStart;
	DMAPacket++->ul128 = PS2_DMA_SET_HEADER(4, 0, DMA_ID_SOURCE_END, 0, 0, 0);
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

	//Do DMA tranfer
	*EE_D2_QWC	= 0;
	*EE_D2_TADR = PS2_DMA_SET_TADR( 
		sps2GetPhysicalAddress(DMAPackAddress->pvStart, DMAPackAddress), 0);

	*EE_D2_CHCR = PS2_DMA_SET_CHCR(DMA_CHCR_DIR_TOMEMORY,
									DMA_CHCR_MOD_CHAIN,
									DMA_CHCR_ASP_NOADDRESS,
									DMA_CHCR_TTE_NOTRANSFER_DMATAG,
									DMA_CHCR_TIE_INTDISABLE,
									DMA_CHCR_STR_STARTTRANSFER,
									0);
	sps2WaitForDMA(2, iSPS2Descriptor);
}

/** Send code to VU1 using a DMA transfer.
	 This is a simple function to build a MPG VIFcode to upload code
	 into VU1, since the code is very small there is no need to do 
	 stitching and all DMA packet fits into a single page of memory.
	 Remember that code can be directly copied into VU1 memory because
	 VU micro memory is memory mapped in address space of main memory.
	 @param DMAPackAddress pointer to sps2Memory_t structure used
			  to buld dma packet.
    @param FileName Name of the file that contains VU1 code.
	 @param iSPS2Descriptor Descriptor to wait for end of tranfer
*/
bool SendMicrocodeToVU1(sps2Memory_t	*DMAPackAddress, 
								char				*FileName, 
								int				iSPS2Descriptor) {

	//open the file that contain binary code 
	FILE *VU1Code;
	VU1Code = fopen(FileName, "rb");
	if (NULL == VU1Code) {
		
		return false;
	}

	//Create a pointer to the memory, begin to load file data starting
	//with the second qword because the first is used to set DMAHeader
	//and vifcode
	PS2_QWORD *DMAHeader = (PS2_QWORD *) DMAPackAddress->pvStart;
	PS2_QWORD *DMAPacket = DMAHeader + 1; //Point 64 bytes after the start

	//Read all file content in a shot, store number of bytes read and close
	//the file
	int CodeSize = fread(DMAPacket, 1, 4092, VU1Code);
	cout << "READ = " << CodeSize << "\n";
	fclose(VU1Code);

	//Now all data is read into memory, build DMA header and
	//VIFcode included in it.
	DMAHeader->ul128 = PS2_DMA_SET_HEADER((CodeSize + 15) >> 4, 
		0, DMA_ID_SOURCE_END, 0, 0, 0);
	DMAHeader->ul32[2] = 0;
	DMAHeader->ul32[3] = PS2_DMA_SET_VIFCODE(0, CodeSize >> 3, 
		VIF_CMD_MPG);

	//Fire the transfer with a simple chain mode with only a END ID
	*EE_D1_QWC	= 0;
	*EE_D1_TADR = PS2_DMA_SET_TADR( 
			sps2GetPhysicalAddress(DMAPackAddress->pvStart, DMAPackAddress), 0);
	FILE *tmpOutput = fopen("test.txt", "w");
	fprintf(tmpOutput, "0x%.8x\n", PS2_DMA_SET_CHCR(DMA_CHCR_DIR_FROMMEMORY,
									DMA_CHCR_MOD_CHAIN,
									DMA_CHCR_ASP_NOADDRESS,
									DMA_CHCR_TTE_TRANSFER_DMATAG,
									DMA_CHCR_TIE_INTDISABLE,
									DMA_CHCR_STR_STARTTRANSFER,
									0));
	fprintf(tmpOutput, "0x%.8x 0x%.8x 0x%.8x 0x%.8x\n", DMAHeader->ul32[0], DMAHeader->ul32[1], DMAHeader->ul32[2], DMAHeader->ul32[3]);

	fclose(tmpOutput);


	*EE_D1_CHCR = PS2_DMA_SET_CHCR(DMA_CHCR_DIR_FROMMEMORY,
									DMA_CHCR_MOD_CHAIN,
									DMA_CHCR_ASP_NOADDRESS,
									DMA_CHCR_TTE_TRANSFER_DMATAG,
									DMA_CHCR_TIE_INTDISABLE,
									DMA_CHCR_STR_STARTTRANSFER,
									0);

	//Wait for DMA to complete the transfer.
	sps2WaitForDMA(1, iSPS2Descriptor);
	return true;
}
//----------------------------------------------------------------------
int main(int argc, char *argv[]) {

	int iSPS2Descriptor;      // The handle we use for sps2
	sps2Memory_t *pMemory;    // Will store information about memory we allocate

	int VU1, VU0; 
	VU1 = open("/dev/ps2vpu1", O_RDWR);
	VU0 = open("/dev/ps2vpu0", O_RDWR);
	if (VU1 < 0) {
	
		cout << "Impossible to open VU1\n";
		exit(3);
	}
	if (VU0 < 0) {
	
		cout << "Impossible to open VU0\n";
		exit(3);
	}
		
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
		4095.0, 0);

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

	//Load VU1 code, This code shows how easy is to upload code to micro mem
	//directly with a memcpy instruction. code is commented because is more
	//correct and instructive sending code with a regular VIFpacket.
	/*
	FILE *VU1Code;
	VU1Code = fopen("BasicTransform.bin", "rb");
	if (NULL == VU1Code) {
		
		cout << "Unable to open BasicTransform.bin...Exiting\n";
		exit(5);
	}
	int CodeSize = fread(VU1_MICRO_MEM, 1, 4096, VU1Code);
	fclose(VU1Code);
	cout << "Read " << CodeSize << " Bytes of code data\n";
	*/

	//Send microcode to VU1
	SendMicrocodeToVU1(pMemory, "BasicTransform.bin", iSPS2Descriptor);

	//Init screenshot class.
	ScreenShot SH(iSPS2Descriptor);
	//Set defalult Addressing mode to STQ.
	int TexturingAddressMode = GS_PRIM_FST_STQ;
	//Set GS Environment register, to choose texturing parameters
	SetGSEnv(pMemory, TM, iSPS2Descriptor); 
 
	for (; ;) {

		//Give moviment to the cube
		Joy.ReadPadData();

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

		//Build view matrix.
		ViewMatrix.ViewLH(EyePos, AtPos, UpPos);

		//Remember the correct order right to left because column vector
		//notation is used.
		TransformationMatrix = ProjMatrix * ViewMatrix * WorldMatrix;

		//Make Packet, this time DMA header is not necessary because we
		//directly instruct DMAC what to do.
		TransformationMatrix.Transpose();
		MakeVIFPacket(pMemory->pvStart, TransformationMatrix, TexturingAddressMode); 
		//DumpDMAPack(pMemory->pvStart, 50);

		//Black background
		sps2UScreenClear(0x0, 0, 0x0);

		//Transfer data with source chain mode.
		sps2FlushCache(iSPS2Descriptor);
		
		//Transferring VIF packet to Vector unit 1 will send vertices data to 
		//VU1 memory and also start the execution of microcode.
		*EE_D1_QWC	= 0;
		*EE_D1_TADR = PS2_DMA_SET_TADR( 
			sps2GetPhysicalAddress(pMemory->pvStart, pMemory), 0);
		*EE_D1_CHCR = PS2_DMA_SET_CHCR(DMA_CHCR_DIR_FROMMEMORY,
									DMA_CHCR_MOD_CHAIN,
									DMA_CHCR_ASP_NOADDRESS,
									DMA_CHCR_TTE_TRANSFER_DMATAG,
									DMA_CHCR_TIE_INTDISABLE,
									DMA_CHCR_STR_STARTTRANSFER,
									0);
		
		sps2WaitForDMA(1, iSPS2Descriptor);
		
		sps2UScreenSwap();

		//grab a screenshot if necessary
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

  close(VU1);
  close(VU0);
return 0;
}
