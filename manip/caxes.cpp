#include "sps2.h"
#include "PS2Defines.h"
#include "AddRegisters.h"
#include "caxes.h"

CAxes::CAxes(void)
{
	// Set a default length
	m_Length = 25.0f;
	
	// Set the world matrix to Identity
	IdentityMatrix(&m_World);
	
	// Untransformed positions
	SetVector(&Line[0].P[0].Vertex, 0.0f, 0.0f, 0.0f, 1.0f);
	SetVector(&Line[1].P[0].Vertex, 0.0f, 0.0f, 0.0f, 1.0f);
	SetVector(&Line[2].P[0].Vertex, 0.0f, 0.0f, 0.0f, 1.0f);
	
	SetVector(&Line[0].P[1].Vertex, m_Length, 0.0f, 0.0f, 1.0f);
	SetVector(&Line[1].P[1].Vertex, 0.0f, m_Length, 0.0f, 1.0f);
	SetVector(&Line[2].P[1].Vertex, 0.0f, 0.0f, m_Length, 1.0f);
	
	
	// Vertex colours
	// Red for x axis
	Line[0].P[0].r = 0x80; Line[0].P[0].g = 0x00; Line[0].P[0].b = 0x00; Line[0].P[0].a = 0x80;
	Line[0].P[1].r = 0x80; Line[0].P[1].g = 0x00; Line[0].P[1].b = 0x00; Line[0].P[1].a = 0x80;
	
	// Green for y asis
	Line[1].P[0].r = 0x00; Line[1].P[0].g = 0x80; Line[1].P[0].b = 0x00; Line[1].P[0].a = 0x80;
	Line[1].P[1].r = 0x00; Line[1].P[1].g = 0x80; Line[1].P[1].b = 0x00; Line[1].P[1].a = 0x80;
	
	// Blue for z axis
	Line[2].P[0].r = 0x00; Line[2].P[0].g = 0x00; Line[2].P[0].b = 0x80; Line[2].P[0].a = 0x80;
	Line[2].P[1].r = 0x00; Line[2].P[1].g = 0x00; Line[2].P[1].b = 0x80; Line[2].P[1].a = 0x80;
	
}

CAxes::~CAxes(void)
{
}

int CAxes::BuildGSPacket(void *& pMem, CPipe3d * pPipe3d)
{
	//	Apply World Matrix
	for(int i = 0; i < 3; i++)
	{
		ApplyMatrix(&Line[i].P[0].TVertex, &m_World, &Line[i].P[0].Vertex);
		ApplyMatrix(&Line[i].P[1].TVertex, &m_World, &Line[i].P[1].Vertex);
	}
	
	// Apply Camera matrix
	for(int i = 0; i < 3; i++)
	{
		ApplyMatrix(&Line[i].P[0].TVertex, &pPipe3d->WC, &Line[i].P[0].TVertex);
		ApplyMatrix(&Line[i].P[1].TVertex, &pPipe3d->WC, &Line[i].P[1].TVertex);
	}

	// Apply perspective transformation matrix
	for(int i = 0; i < 3; i++)
	{
		ApplyMatrix(&Line[i].P[0].TVertex, &pPipe3d->VS, &Line[i].P[0].TVertex);
		ApplyMatrix(&Line[i].P[1].TVertex, &pPipe3d->VS, &Line[i].P[1].TVertex);
	}

	// Do Homogeneous divide 
	for(int i = 0; i < 3; i++)	
	{
		pPipe3d->HomoDivide(&Line[i].P[0].TVertex);
		pPipe3d->HomoDivide(&Line[i].P[1].TVertex);
	}	


	// Clip Check the point and print out some stats
	for(int i = 0; i < 3; i++)
	{
		//static int iClip = 0;	
		if(pPipe3d->ClipCheck(&Line[i].P[0].TVertex))
		{
			//printf("%d *********** Vertex %d has been clipped\n", iClip++, i);
			Line[i].P[0].ADC = 1;
		}
		else
		{
			//printf("*********** This point will be drawn\n\n");
			Line[i].P[0].ADC = 0;
		}
		
		//static int iClip = 0;	
		if(pPipe3d->ClipCheck(&Line[i].P[1].TVertex))
		{
			//printf("%d *********** Vertex %d has been clipped\n", iClip++, i);
			Line[i].P[1].ADC = 1;
		}
		else
		{
			//printf("*********** This point will be drawn\n\n");
			Line[i].P[1].ADC = 0;
		}
	}
		
			
	// Apply view port transformation matrix
	for(int i = 0; i < 3; i++)
	{
		ApplyMatrix(&Line[i].P[0].TVertex, &pPipe3d->VT, &Line[i].P[0].TVertex);
		ApplyMatrix(&Line[i].P[1].TVertex, &pPipe3d->VT, &Line[i].P[1].TVertex);
	}
	
	// Create the Axes Packet
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pMem;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = 3;	// Only one of them
	pGIFTag->s.PRE = 1;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;		// This is the last giftag in the packet
	 
	pGIFTag->s.PRIM = GS_PRIM(PRIM_LINE, 
							PRIM_IIP_FLAT, 
							PRIM_TME_OFF,
							PRIM_FGE_OFF, 
							PRIM_ABE_OFF, 
							PRIM_AA1_OFF, 
							PRIM_FST_STQ, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
							
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 4; 
	// Since we are putting 12 registers, this is a faster way of setting
	// the REGS field (1 = RGBA, 3 = UV, 5 = XYZ2, 2 = ST)
	pGIFTag->s.REGS = (uint64)(0x5151);
		
	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);

	for(int i = 0; i < 3; i++)
	{
		AddColour(pRegister, Line[i].P[0].r,Line[i].P[0].g, Line[i].P[0].b, Line[i].P[0].a);
		AddVertexFPo(pRegister, (int)(Line[i].P[0].TVertex.v[0]), 
								(int)(Line[i].P[0].TVertex.v[1]), 
								(int)(Line[i].P[0].TVertex.v[2]), 
								Line[i].P[0].ADC);
								
		AddColour(pRegister, Line[i].P[1].r,Line[i].P[1].g, Line[i].P[1].b, Line[i].P[1].a);
		AddVertexFPo(pRegister, (int)(Line[i].P[1].TVertex.v[0]), 
								(int)(Line[i].P[1].TVertex.v[1]), 
								(int)(Line[i].P[1].TVertex.v[2]), 
								Line[i].P[1].ADC);
	}

	// Work out how much we just wrote
	int iWC = (int)pRegister - (int)pMem;
	
	//Set the memory pointer we used to the next free location
	pMem = (char *)pRegister;
	
	//return the number of bytes written to memory
	return iWC;	
}


void CAxes::SetWorldMatrix(FMatrix World)
{
	m_World = World;	
	
}