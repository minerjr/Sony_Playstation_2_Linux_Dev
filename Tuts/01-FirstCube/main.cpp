/** @file main.cpp
	 Contains the real code that draws cube on the screen
*/
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <linux/ps2/dev.h>
#include <memory.h>

#include "ps2vpufile.h"
#include "ps2gs.h"
#include "ps2dma.h"
#include "PS2Math.h"

//! Some useful defines to make DMAPacket
#define MAKE_DMA_HEADER(QWC, PCE, ID, IRQ) \
	((__u64)(QWC) | ((__u64)(PCE) << 26) | ((__u64)(ID) << 28) | ((__u64)(IRQ) << 31))
#define SCE_GIF_SET_TAG(NLOOP, EOP, PRE, PRIM, FLG, NREG) \
	((__u64)(NLOOP) | ((__u64)(EOP)<<15) | ((__u64)(PRE) << 46) | \
	((__u64)(PRIM)<<47) | ((__u64)(FLG)<<58) | ((__u64)(NREG)<<60))
#define SCE_GS_SET_PRIM(PRIM, IIP, TME, FGE, ABE, AA1, FST, CTXT, FIX) \
	((__u32)(PRIM) | ((__u32)(IIP) << 3) | \
	((__u32)(TME) << 4) | ((__u32)(FGE) << 5) | \
	((__u32)(ABE) << 6)  | ((__u32)(AA1) << 7) | \
	((__u32)(FST) << 8) | ((__u32)(CTXT) << 9) | ((__u32)(FIX) << 10))

/**
	Used to access 128Bit Words of ps2.
*/
typedef union _tagQWord {
	__u128		ul128;
	__u64			ul64[2];
	__u32			ul32[4];
} Q_WORDDATA;

/**
	Structure used to build Gif packet.
*/
typedef struct _tagGifPack{
	Q_WORDDATA	*buf;
	u_int			size;
	u_int			*pBase;
} GIF_PACKET;

/**
	Utility for making giftag 
*/
typedef struct {
	__u32 NLOOP:15;
	__u32 EOP:1;
	__u32 PAD16:16;
	__u32 ID:14;
	__u32 PRE:1;
	__u32 PRIM:11;
	__u32 FLG:2;
	__u32 NREG:4;
	__u32 REGS0:4;
	__u32 REGS1:4;
	__u32 REGS2:4;
	__u32 REGS3:4;
	__u32 REGS4:4;
	__u32 REGS5:4;
	__u32 REGS6:4;
	__u32 REGS7:4;
	__u32 REGS8:4;
	__u32 REGS9:4;
	__u32 REGS10:4;
	__u32 REGS11:4;
	__u32 REGS12:4;
	__u32 REGS13:4;
	__u32 REGS14:4;
	__u32 REGS15:4;
} sceGifTag __attribute__((aligned(16)));

typedef int   ColorVector4[4];

int g_inter;
int g_out_mode;
int g_ff_mode;
int g_resolution;
int g_refresh_rate;
int g_psm;
int g_zpsm;
int g_zbits;

ps2_gs_dbuff	g_db;
ps2_gs_finish	g_finish;
struct ps2_image g_img;

int g_fd_gs;

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
										{255, 0, 0, 255},
										{255, 0, 0, 255},
										{0, 255, 0, 255},
										{0, 255, 0, 255},
										{0, 255, 0, 255},
										{255, 0, 0, 255},
										{255, 0, 0, 255},
										{255, 0, 0, 255},
										{255, 0, 0, 255},
										{0, 255, 0, 255},
										{0, 255, 0, 255},
										{0, 255, 0, 255},
										{255, 0, 0, 255},
										{255, 0, 0, 255},
									};

/** Release the console*/
int release(void)
{
	if (g_fd_gs >= 0) {
		ps2_gs_close();
		g_fd_gs = -1;
	}
	
	return PS2_GS_VC_REL_SUCCESS;
}

/** Acquire the console*/
int acquire(void)
{
	g_fd_gs = ps2_gs_open(-1);
	if (g_fd_gs < 0) {
	    return PS2_GS_VC_ACQ_FAILURE;
	}
	
	ps2_gs_reset(0, g_inter, g_out_mode, g_ff_mode, g_resolution,
				 g_refresh_rate);
	ps2_gs_start_display(1);
	
	return PS2_GS_VC_ACQ_SUCCESS;
}

//! This function creates DMA packet to send cube information to the GS
void MakeDMAPack(Q_WORDDATA *DmaPack, 
					  const PS2Math::Matrix44 &TransformationMatrix) {

	//DMA transfer type is END, it means: "transfer a number of QWC WORDS 
	//and end transfer".
	//Number of Word to be transferred is calculated by number of vertex
   //times size of a vertex
	//Remember to not forget the GIFTAG
	int VertexNum = sizeof(Cube) / sizeof(Cube[0]);
	int QWC = VertexNum * 2 + 1;

	//This is the index into DMAPack
	int DMAPackIndex = 0;

	//Now create the HEADER
	DmaPack[DMAPackIndex++].ul64[0] = MAKE_DMA_HEADER(QWC, 0, PS2_DMATAG_END ,0);
	
	//Now Gif tag to instruct GS of what we need to draw.
	//First of all must set prim register 
	__u32 PrimReg = SCE_GS_SET_PRIM(4, 1, 0, 0, 0, 0, 0, 0, 0);

	//Then build GIFTAG
	DmaPack[DMAPackIndex].ul64[0] = 
		SCE_GIF_SET_TAG(VertexNum, 1, 1, PrimReg, PS2_GIFTAG_FLG_PACKED, 2);
	sceGifTag *GifTag = (sceGifTag *) &DmaPack[DMAPackIndex];
	GifTag->REGS0 = PS2_GIFTAG_REGS_RGBAQ;
	GifTag->REGS1 = PS2_GIFTAG_REGS_XYZ2;
	DMAPackIndex++;

	//OK! Now it remains only to write down the values of transformed vertex
	PS2Math::Vector4 VOut;
	int FPVertex[4];
	for(int VertexIndex = 0; VertexIndex < VertexNum; ++VertexIndex) {
	
		VOut = TransformationMatrix * Cube[VertexIndex];
		VOut.ConvertToFixedPoint(FPVertex);

		DmaPack[DMAPackIndex++].ul128 = *(__u128 *) CubeColor[VertexIndex];
		DmaPack[DMAPackIndex++].ul128 = *(__u128 *) FPVertex;
	}
}

static __u8 __attribute__((aligned(16))) ScratchPad[16 * 1024]; // scratch pad emu



//! This stream is used for debugging purpose
std::ofstream Out("OutputDebug.txt");
FILE *OutDbg = fopen("Dump.txt", "wt");

void DumpDMAPack(Q_WORDDATA *Data, int Num) {

	for (int I = 0; I < Num; ++I) {
	
		fprintf(OutDbg, "%d:\t%X %X %X %X\n", I, 
			Data[I].ul32[3], Data[I].ul32[2], Data[I].ul32[1], Data[I].ul32[0]);
	}
}

//----------------------------------------------------------------------
int main(int argc, char *argv[])
{
	int frame, odev, next2k;
	float Increment = (float)M_PI / 180.0f;

	ps2_gs_gparam *gp = ps2_gs_get_gparam();

	//Set out mode to vesa, if the user specify -pal or -ntsc change 
	//video mode.
	g_out_mode = PS2_GS_VESA;
	if (argc > 1) { 
		if(strcmp(argv[1], "-pal") == 0)
			g_out_mode = PS2_GS_PAL;
		else if (strcmp(argv[1], "-ntsc") == 0)
			g_out_mode = PS2_GS_NTSC;
	}

	g_psm = PS2_GS_PSMCT32;
	g_zpsm = PS2_GS_PSMZ24;
	g_zbits = 24;

	ps2_gs_vc_graphicsmode();

	g_fd_gs = ps2_gs_open(-1);
	
	//This function init the GS of the PS2 (ps2gs_en.txt:Line 656)
	ps2_gs_reset(0, PS2_GS_NOINTERLACE, 
					  g_out_mode, 
					  PS2_GS_FRAME, 
					  PS2_GS_640x480,
					  PS2_GS_60Hz);
	
	next2k = ps2_gs_set_dbuff(&g_db, PS2_GS_PSMCT32, gp->width, gp->height,
							  PS2_GS_ZGREATER,
							  g_zpsm, 1);
	*(__u64 *)&g_db.clear0.rgbaq = PS2_GS_SETREG_RGBAQ(0x10, 0x10, 0x18, 0x80,
													   0x3f800000);
	*(__u64 *)&g_db.clear1.rgbaq = PS2_GS_SETREG_RGBAQ(0x10, 0x10, 0x18, 0x80,
													   0x3f800000);

	frame = 0;
	
	//Initialize drawing environment and clear the buffers
	ps2_gs_put_drawenv(&g_db.giftag1);
	
	//Wait for all operation to be finished
	ps2_gs_set_finish(&g_finish);
	ps2_gs_wait_finish(&g_finish);
	
	//Sync to V-Sync signal and start drawing in front buffer
	odev = !ps2_gs_sync_v(0);
	ps2_gs_start_display(1);
	
	//Enable the possibility to switch console from graphic mode to text mode
	//address of release and acquire function must be set by the user
	ps2_gs_vc_enablevcswitch(acquire, release);

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
	float xyAspectRatio = 
		static_cast<float>(gp->width * gp->pixel_ratio) / gp->height; 
	ProjMatrix.PerspectiveLH(1 * xyAspectRatio, 1, 1, 65536.0f);
	Out << "View Matrix\n" << ViewMatrix << endl << 
		"Projection Matrix\n" << ProjMatrix << endl;
	int xOffs = gp->width / 2;
	int yOffs = gp->height / 2;
	ProjMatrix.MapToViewPort(gp->center_x - xOffs,	gp->center_x + xOffs, 
									 gp->center_y + yOffs,	gp->center_y - yOffs, 
									 16777215.000000,			0);
	Out << "View Matrix\n" << ViewMatrix << endl << 
		"Projection Matrix\n" << ProjMatrix << endl;

   for (; ;) {

		//Lock the console for drawing
		ps2_gs_vc_lock();
		//Swap BackBuffer with front buffer, show previous frame and begin
		//to build current buffer
		ps2_gs_swap_dbuff(&g_db, frame);
		
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

		//Make packet to sent to GS, this is where data are stored in
		//Ps2 DMA Tag format to be sent from EE to GS.
		MakeDMAPack((Q_WORDDATA *) ScratchPad, TransformationMatrix);
		ps2_dma_start(g_fd_gs, NULL, (ps2_dmatag *)ScratchPad);
	
		DumpDMAPack((Q_WORDDATA *)ScratchPad, 10);

		//Increment frame and wait for vertical retrace to end this frame.
		frame++;
		odev = !ps2_gs_sync_v(0);
		ps2_gs_vc_unlock();
	}
	
	return 0;
}
