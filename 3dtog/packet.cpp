#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>
#include <assert.h>
#include <math.h>
#include "PS2Defines.h"
#include "AddRegisters.h"
#include "vecmaths.h"
#include "packet.h"

extern Camera_t Cam;
extern FMatrix ViewScreen, ViewPortTrans;

int BuildSprite2D(PS2Sprite_t * pSprite, char *& pMem)
{
	//Temp is the identity matrix used for rotation
	FMatrix Temp;
	IdentityMatrix(&Temp);
	//There are the transformed coordinates of the sprite
	FVector TR, TL, BR, BL;

	// Build the world positional matrix for the sprite
	
	//The Scale matrix
	ScaleMatrix(&pSprite->World, &Temp, 1.0f);
	
	// The rotation
	RotMatrix(&pSprite->World, &pSprite->World, pSprite->RotZ, pSprite->RotY, pSprite->RotX);
	
	//now the position
	pSprite->World.m[0][3] = pSprite->Pos.v[0];
	pSprite->World.m[1][3] = pSprite->Pos.v[1];
	pSprite->World.m[2][3] = pSprite->Pos.v[2];	
	
	//apply World matrix to the untransformed coordinates
	ApplyMatrix(&TR, &pSprite->World, &pSprite->TR);
	ApplyMatrix(&TL, &pSprite->World, &pSprite->TL);
	ApplyMatrix(&BR, &pSprite->World, &pSprite->BR);
	ApplyMatrix(&BL, &pSprite->World, &pSprite->BL);
	
	//print out some stats
	/*
	DumpFVector("Top Left", &TL);
	DumpFVector("Top Right", &TR);
	DumpFVector("Bottom Left", &BL);
	DumpFVector("Bottom Right", &BR);
	DumpMatrix("Sprite", &pSprite->World);
	*/	
	
	// Apply Camera matrix
	ApplyMatrix(&TR, &Cam.WC, &TR);
	ApplyMatrix(&TL, &Cam.WC, &TL);
	ApplyMatrix(&BR, &Cam.WC, &BR);
	ApplyMatrix(&BL, &Cam.WC, &BL);
	
	/*
	DumpFVector("Top Left", &TL);
	DumpFVector("Top Right", &TR);
	DumpFVector("Bottom Left", &BL);
	DumpFVector("Bottom Right", &BR);
	DumpMatrix("Cam", &Cam.WS);
	*/
		
	// Apply perspective transformation matrix
	ApplyMatrix(&TR, &ViewScreen, &TR);
	ApplyMatrix(&TL, &ViewScreen, &TL);
	ApplyMatrix(&BR, &ViewScreen, &BR);
	ApplyMatrix(&BL, &ViewScreen, &BL);
		
	// Do Homogeneous divide 
	HomoDivide(&TR);
	HomoDivide(&TL);
	HomoDivide(&BR);
	HomoDivide(&BL);
	
	static int iClip = 0;
	// Clip Check the point and print out some stats
	if(ClipCheck(&TR))
	{
		printf("%d *********** TR had been clipped\n", iClip++);
		pSprite->TRC = 1;
	}
	else
	{
		//printf("*********** This point will be drawn\n\n");
		pSprite->TRC = 0;
	}
	// Clip Check the point and print out some stats
	if(ClipCheck(&TL))	
	{
		printf("%d *********** TL had been clipped\n", iClip++);
		pSprite->TLC = 1;
	}	
	else
	{
		//printf("*********** This point will be drawn\n\n");
		pSprite->TLC = 0;
	}	
	// Clip Check the point and print out some stats
	if(ClipCheck(&BL))	
	{
		printf("%d *********** BL had been clipped\n", iClip++);
		pSprite->BLC = 1;
	}
	else
	{
		//printf("*********** This point will be drawn\n\n");
		pSprite->BLC = 0;
	}
	// Clip Check the point and print out some stats
	if(ClipCheck(&BR))	
	{
		printf("%d *********** BR had been clipped\n", iClip++);
		pSprite->BRC = 1;
	}
	else
	{
		//printf("*********** This point will be drawn\n\n");
		pSprite->BRC = 0;
	}
	
	
	/*			
	DumpFVector("Top Left", &TL);
	DumpFVector("Top Right", &TR);
	DumpFVector("Bottom Left", &BL);
	DumpFVector("Bottom Right", &BR);
	*/	
	
	// Do Viewport transformation
	ApplyMatrix(&TR, &ViewPortTrans, &TR);
	ApplyMatrix(&TL, &ViewPortTrans, &TL);
	ApplyMatrix(&BR, &ViewPortTrans, &BR);
	ApplyMatrix(&BL, &ViewPortTrans, &BL);

	/*			
	DumpFVector("Top Left", &TL);
	DumpFVector("Top Right", &TR);
	DumpFVector("Bottom Left", &BL);
	DumpFVector("Bottom Right", &BR);
	*/
	
	// Create a textured sprite
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pMem;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 1;	// Only one of them
	pGIFTag->s.PRE = 1;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;		// This is the last giftag in the packet
	 
	pGIFTag->s.PRIM = GS_PRIM(PRIM_TRI_STRIP, 
							PRIM_IIP_FLAT, 
							PRIM_TME_ON,
							PRIM_FGE_OFF, 
							PRIM_ABE_ON, 
							PRIM_AA1_OFF, 
							PRIM_FST_UV, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
							
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 9; 
	// Since we are putting 9 registers, this is a faster way of setting
	// the REGS field (1 = RGBA, 3 = UV, 5 = XYZ2)
	pGIFTag->s.REGS = (uint64(0x5) << 32) | (uint64(0x35353531));
	
	
	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	AddColour(pRegister, pSprite->r, pSprite->g, pSprite->b, pSprite->a);
	

	//top left
	AddUV(pRegister, pSprite->u, pSprite->v);
	AddVertexFPo(pRegister, (int)(TL.v[0]), (int)(TL.v[1]), (int)(TL.v[2]), pSprite->TLC);
	
	//bottom left
	AddUV(pRegister, pSprite->u, pSprite->v + pSprite->h);
	AddVertexFPo(pRegister,  (int)(BL.v[0]), (int)(BL.v[1]), (int)(BL.v[2]), pSprite->BLC);
	
	//top right
	AddUV(pRegister, pSprite->u + pSprite->w, pSprite->v);
	AddVertexFPo(pRegister,  (int)(TR.v[0]), (int)(TR.v[1]), (int)(TR.v[2]), pSprite->TRC);
	
	//bottom right
	AddUV(pRegister, pSprite->u + pSprite->w, pSprite->v + pSprite->h);
	AddVertexFPo(pRegister,  (int)(BR.v[0]), (int)(BR.v[1]), (int)(BR.v[2]), pSprite->BRC);

	// Work out how much we just wrote
	int iWC = (int)pRegister - (int)pMem;
	//int iQWC = iWC >> 4;

	// Assert that we haven't written more data than we allocated
	assert(iWC < 4096);
	
	//Set the memory pointer we used to the next free location
	pMem = (char *)pRegister;
	
	//return the number of bytes written to memory
	return iWC;
}

