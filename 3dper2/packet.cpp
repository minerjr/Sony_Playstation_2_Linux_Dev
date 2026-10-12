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
#include "cpipe3d.h"

CPipe3d Pipe3d;

int BuildSprite2D(PS2Sprite_t * pSprite, char *& pMem)
{
	//Temp is the identity matrix used for rotation
	FMatrix Temp;
	IdentityMatrix(&Temp);
	
	//get the world positional matrix for the sprite
	
	//The Scale matrix
	ScaleMatrix(&pSprite->World, &Temp, pSprite->Scale);
	
	// The rotation
	RotMatrix(&pSprite->World, &pSprite->World, pSprite->RotZ, pSprite->RotY, pSprite->RotX);
	
	//now the position
	pSprite->World.m[0][3] = pSprite->Pos.v[0];
	pSprite->World.m[1][3] = pSprite->Pos.v[1];
	pSprite->World.m[2][3] = pSprite->Pos.v[2];	
	
	// Apply World matrix to the untransformed coordinates
	for(int i = 0; i < 4; i++)
		ApplyMatrix(&pSprite->Vertex[i].TVertex, &pSprite->World, &pSprite->Vertex[i].Vertex);

	// Apply Camera matrix
	for(int i = 0; i < 4; i++)
		ApplyMatrix(&pSprite->Vertex[i].TVertex, &Pipe3d.WC, &pSprite->Vertex[i].TVertex);

	// Apply perspective transformation matrix
	for(int i = 0; i < 4; i++)
		ApplyMatrix(&pSprite->Vertex[i].TVertex, &Pipe3d.VS, &pSprite->Vertex[i].TVertex);

	// Get Perspective correct texture coordinates
	for(int i = 0; i < 4; i++)
	{	
		pSprite->Vertex[i].Q = 1.0f/pSprite->Vertex[i].TVertex.v[3];
		pSprite->Vertex[i].S = pSprite->Vertex[i].s * pSprite->Vertex[i].Q;
		pSprite->Vertex[i].T = pSprite->Vertex[i].t * pSprite->Vertex[i].Q;
	}
	
	// Do Homogeneous divide 
	for(int i = 0; i < 4; i++)	
		Pipe3d.HomoDivide(&pSprite->Vertex[i].TVertex);


	// Clip Check the point and print out some stats
	for(int i = 0; i < 4; i++)
	{
		static int iClip = 0;	
		if(Pipe3d.ClipCheck(&pSprite->Vertex[i].TVertex))
		{
			printf("%d *********** Vertex %d has been clipped\n", iClip++, i);
			pSprite->Vertex[i].ADC = 1;
		}
		else
		{
			//printf("*********** This point will be drawn\n\n");
			pSprite->Vertex[i].ADC = 0;
		}
	}
		
	// Do Viewport transformation
	for(int i = 0; i < 4; i++)
		ApplyMatrix(&pSprite->Vertex[i].TVertex, &Pipe3d.VT, &pSprite->Vertex[i].TVertex);
	
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
							PRIM_FST_STQ, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
							
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 12; 
	// Since we are putting 12 registers, this is a faster way of setting
	// the REGS field (1 = RGBA, 3 = UV, 5 = XYZ2, 2 = ST)
	pGIFTag->s.REGS = (uint64(0x5125) << 32) | (uint64(0x12512512));
		
	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	for(int i = 0; i < 4; i++)
	{
		AddSTQ(pRegister, pSprite->Vertex[i].S, pSprite->Vertex[i].T, pSprite->Vertex[i].Q);
		AddColour(pRegister, pSprite->Vertex[i].r, pSprite->Vertex[i].g, 
							 pSprite->Vertex[i].b, pSprite->Vertex[i].a);
		AddVertexFPo(pRegister, (int)(pSprite->Vertex[i].TVertex.v[0]), 
								(int)(pSprite->Vertex[i].TVertex.v[1]), 
								(int)(pSprite->Vertex[i].TVertex.v[2]), 
								pSprite->Vertex[i].ADC);
	}

	// Work out how much we just wrote
	int iWC = (int)pRegister - (int)pMem;
	
	// Assert that we haven't written more data than we allocated
	assert(iWC < 4096);
	
	//Set the memory pointer we used to the next free location
	pMem = (char *)pRegister;
	
	//return the number of bytes written to memory
	return iWC;
}

