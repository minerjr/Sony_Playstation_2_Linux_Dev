#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <assert.h>
#include "PS2Defines.h"
#include "PS2Types.h"
#include "AddRegisters.h"


int BuildSprite2D(PS2Sprite_t * pSprite, char *& pMem)
{
	// Create a textured sprite
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pMem;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 1;	// Only one of them
	pGIFTag->s.PRE = 1;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;		// This is the last giftag in the packet
	 
	pGIFTag->s.PRIM = GS_PRIM(PRIM_SPRITE, 
							PRIM_IIP_FLAT, 
							PRIM_TME_ON,
							PRIM_FGE_OFF, 
							PRIM_ABE_ON, 
							PRIM_AA1_OFF, 
							PRIM_FST_UV, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
							
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 5; 
	pGIFTag->s.REGS = GIF_REG_RGBAQ 		| \
						(GIF_REG_UV << 4) 	| \
						(GIF_REG_XYZ2 << 8) | \
						(GIF_REG_UV << 12) 	| \
						(GIF_REG_XYZ2 << 16);

	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);
	
	// Add the register data
	AddColour(pRegister, pSprite->r, pSprite->g, pSprite->b, pSprite->a);
	AddUV(pRegister, pSprite->u, pSprite->v);
	AddVertex(pRegister, pSprite->x, pSprite->y, pSprite->z);
	AddUV(pRegister, pSprite->u+pSprite->w, pSprite->v+pSprite->h);
	AddVertex(pRegister,  pSprite->x+pSprite->w, pSprite->y+pSprite->h, pSprite->z);

	// Work out how much we just wrote
	int iWC = (int)pRegister - (int)pMem;
	
	// Assert that we haven't written more data than we allocated
	assert(iWC < 4096);
	
	//Set the memory pointer we used to the next free location
	pMem = (char *)pRegister;
	
	//return the number of bytes written to memory
	return iWC;
}

