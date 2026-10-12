#include "sps2wrap.h"

CSPS2Manager::CSPS2Manager()
{
	m_Running = true;
	m_pMemCached = NULL;
	printf("Writing cache to HDD\n");
	system("sync");
}

CSPS2Manager::~CSPS2Manager()
{
	printf("Shutting down CSPS2Manager\n");
	sps2UScreenShutdown();
	if(m_pMemCached)sps2Free(m_pMemCached);
	sps2Release(m_iSPS2Desc);
}

void CSPS2Manager::Initialise(const int iAllocPages)
{
	// The minimum needed
	assert(iAllocPages >= 640);
	
	// Maximum of about 12MB although possibly more is ok
	assert(iAllocPages <= 3000);
	
	printf("Initilising CSPS2Manager with %d 4k Blocks (%d kBytes)\n", 
				iAllocPages, (iAllocPages*4));
	
	m_iNumPages = iAllocPages;

	m_iSPS2Desc = sps2Init();

	if(m_iSPS2Desc < 0)
	{
		fprintf(stderr, "Failed to initialise SPS2 library." \
				"Please check that the module is loaded.\n");
		exit(-1);
	}
	
	m_pMemCached = sps2Allocate(iAllocPages * PAGE4K,
			SPS2_MAP_BLOCK_4K | SPS2_MAP_CACHED, m_iSPS2Desc);
			
	if(m_pMemCached < 0)
	{
		fprintf(stderr, "Failed to Allocate SPS2 memory.\n");
		exit(-1);
	}
			
	m_pCached = (char *)m_pMemCached->pvStart;
	
	// This arrangement needs 640 4k blocks minimum
	
	m_FontDmaBase = (void *)(&m_pCached[(m_iNumPages - 32) * PAGE4K]);
	
	m_Tex1DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 1) * PAGE4K]);
	m_Tex2DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 2) * PAGE4K]);
	m_Tex3DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 3) * PAGE4K]);
	m_Tex4DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 4) * PAGE4K]);
	
	m_Prim1DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 5) * PAGE4K]);
	m_Prim2DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 6) * PAGE4K]);
	m_Prim3DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 7) * PAGE4K]);
	m_Prim4DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 8) * PAGE4K]);
	
	// Some free memory at this point
	
	m_FontTexBase = (void *)(&m_pCached[(m_iNumPages - 64 * 2) * PAGE4K]);
	
	m_Tex1Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 3) * PAGE4K]);
	m_Tex2Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 4) * PAGE4K]);
	m_Tex3Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 5) * PAGE4K]);
	m_Tex4Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 6) * PAGE4K]);
	
	m_Prim4Base = 	(void *)(&m_pCached[64 * 3 * PAGE4K]);
	m_Prim3Base = 	(void *)(&m_pCached[64 * 2 * PAGE4K]);
	m_Prim2Base = 	(void *)(&m_pCached[64 * 1 * PAGE4K]);
	m_Prim1Base = 	(void *)(&m_pCached[0]);
	
	sps2UScreenInit(0);
}

sps2Memory_t * CSPS2Manager::GetMemoryStruct(void)
{
	return m_pMemCached;
}

int CSPS2Manager::GetSPS2Handle(void)
{
	return m_iSPS2Desc;
}

unsigned long CSPS2Manager::GetPhysicalAddress(void * pMemory)
{
	return sps2GetPhysicalAddress(pMemory, m_pMemCached);	
}

int CSPS2Manager::GetScreenWidth()
{ 
	return sps2UScreenGetWidth(); 
}

int CSPS2Manager::GetScreenHeight()
{ 
	return sps2UScreenGetHeight(); 
}

void * CSPS2Manager::GetTex1Base(void)
{
	return m_Tex1Base;
}

void * CSPS2Manager::GetTex2Base(void)
{
	return m_Tex2Base;
}

void * CSPS2Manager::GetTex3Base(void)
{
	return m_Tex3Base;
}

void * CSPS2Manager::GetTex4Base(void)
{
	return m_Tex4Base;
}
		
void * CSPS2Manager::GetTex1DmaBase(void)
{ 
	return m_Tex1DmaBase; 
}

void * CSPS2Manager::GetTex2DmaBase(void)
{ 
	return m_Tex2DmaBase; 
}

void * CSPS2Manager::GetTex3DmaBase(void)
{ 
	return m_Tex3DmaBase; 
}

void * CSPS2Manager::GetTex4DmaBase(void)
{ 
	return m_Tex4DmaBase; 
}
		
void * CSPS2Manager::GetPrim1Base(void)
{
	return m_Prim1Base;
}

void * CSPS2Manager::GetPrim2Base(void)
{
	return m_Prim2Base;
}

void * CSPS2Manager::GetPrim3Base(void)
{
	return m_Prim3Base;
}

void * CSPS2Manager::GetPrim4Base(void)
{
	return m_Prim4Base;
}

void * CSPS2Manager::GetPrim1DmaBase(void)
{ 
	return m_Prim1DmaBase; 
}

void * CSPS2Manager::GetPrim2DmaBase(void)
{ 
	return m_Prim2DmaBase; 
}

void * CSPS2Manager::GetPrim3DmaBase(void)
{ 
	return m_Prim3DmaBase; 
}

void * CSPS2Manager::GetPrim4DmaBase(void)
{ 
	return m_Prim4DmaBase; 
}

void * CSPS2Manager::GetFontTexBase(void)
{ 
	return m_FontTexBase; 
}
		
void * CSPS2Manager::GetFontDmaBase(void)
{ 
	return m_FontDmaBase; 
}

unsigned int CSPS2Manager::GetVideoMode(void)
{ 
	return sps2UScreenGetVideoMode(); 
}

unsigned int CSPS2Manager::GetFirstFreeGSPage(void)
{
	return sps2UScreenGetFirstFreeGSPage();
}

void CSPS2Manager::FirePacketToGS(void * const pStart, void * const pEnd, void * const pDMA)
{
	Dn_CHCR_t chcr;				// channel control register 
	Dn_TADR_t tadr;				// Tag address register
	
	// Use a REF tags to point the DMAC to the data
	sps2DMATag_t DMARef;
	uint64 * pDMATag = (uint64 *)(pDMA);

	//calculate how many bytes in the packet
	int iWC = (int)pEnd - (int)pStart;
	
	//printf("Packet Size = %d\n", iWC); 
	
	// return if no data
	if(iWC == 0) return;

	// Assert that we haven't written more data than 256k
	assert(iWC < (1024*256));	
	
	// Check if the packet extends over a 4k Page
	if(iWC > PAGE4K)
	{	
		// Configure the Ref Tags to send 4k of data
		DMARef.i64 = 0;
		DMARef.s.QWC =  PAGE4K >> 4;	// Transfer 4K 
		DMARef.s.ID = DMA_ID_SOURCE_REF;
		
		// Now set up all the 4k pages	
		int i = 0;
		do
		{
			// Get the physical address of each 4K page, and put it into the ref tag.
			// **********  look at this again
			DMARef.s.ADDR = GetPhysicalAddress((void *)(((char *)pStart) + (i*PAGE4K)));
	
			// Add the ref tag to the DMA chain.
			*pDMATag = DMARef.i64;
			pDMATag += 2;
			i++;
			// Find out how much data is left
			iWC -= PAGE4K;
		}while(iWC > PAGE4K);
		
		// If there is remaining data, send it with a ref tag
		if(iWC > 0)
		{
			// Get the physical address of each 4K page, and put it into the ref tag.
			// **********  look at this again
			DMARef.s.ADDR = GetPhysicalAddress((void *)(((char *)pStart) + (i*PAGE4K)));
			DMARef.s.QWC =  iWC >> 4;	// Transfer 4K 
			// Add the ref tag to the DMA chain.
			*pDMATag = DMARef.i64;
			pDMATag += 2;
		}
	}
	// The Packet is less than 4k - send it with one ref tag
	else
	{
		DMARef.i64 = 0;
		DMARef.s.QWC = iWC >> 4;	// Transfer all the data
		DMARef.s.ID = DMA_ID_SOURCE_REF;
	
		// Get the physical address of the data and put it into the ref tag.
		DMARef.s.ADDR = GetPhysicalAddress(pStart);
	
		// Add the ref tag to the DMA chain.
		*pDMATag = DMARef.i64;
		pDMATag += 2;
	}

	// Now that we have uploaded the data, the DMA chain needs to be ended.
	sps2DMATag_t DMAEnd;

	DMAEnd.i64 = 0;
	DMAEnd.s.QWC = 0;	
	DMAEnd.s.ID = DMA_ID_SOURCE_END;

	// Add the DMA tag to the chain
	*pDMATag = DMAEnd.i64;

	// Importand - Try to remember this!!!!!!!!!!!!!!!!!!!!
	FlushCache();
			
	// Set the address of the first tag (right at the start of the chain)
	tadr.i32 = 0;
	tadr.s.SPR = 0;
	tadr.s.ADDR = GetPhysicalAddress(pDMA);
	*EE_D2_TADR = tadr.i32;

	// Don't transfer any data from MADR
	*EE_D2_QWC = 0;

	// Set the channel control register
	chcr.i32 = 0;
	chcr.s.MOD = CHCR_MOD_CHAIN;
	chcr.s.STR = 1;

	// Fire off the packet!
	*EE_D2_CHCR = chcr.i32;

	// Wait for it to complete
	WaitForDMA(2);
	
	//while((*EE_D2_CHCR)&0x0100); 

}

void CSPS2Manager::FirePacketToVIF(void * const pStart, void * const pEnd, void * const pDMA)
{
	Dn_CHCR_t chcr;				// channel control register 
	Dn_TADR_t tadr;				// Tag address register
	
	// Use a REF tags to point the DMAC to the data
	sps2DMATag_t DMARef;
	uint64 * pDMATag = (uint64 *)(pDMA);
		
	//calculate how many bytes in the packet
	int iWC = (int)pEnd - (int)pStart;
	
	//printf("Packet Size = %d\n", iWC); 
	
	// return if no data
	if(iWC == 0) return;

	// Assert that we haven't written more data than 256k
	assert(iWC < (1024*256));	
	
	// Check if the packet extends over a 4k Page
	if(iWC > PAGE4K)
	{	
		// Configure the Ref Tags to send 4k of data
		DMARef.i64 = 0;
		DMARef.s.QWC =  PAGE4K >> 4;	// Transfer 4K 
		DMARef.s.ID = DMA_ID_SOURCE_REF;
		
		// Now set up all the 4k pages	
		int i = 0;
		do
		{
			// Get the physical address of each 4K page, and put it into the ref tag.
			// **********  look at this again
			DMARef.s.ADDR = GetPhysicalAddress((void *)(((char *)pStart) + (i*PAGE4K)));
	
			// Add the ref tag to the DMA chain.
			*pDMATag = DMARef.i64;
			pDMATag += 2;
			i++;
			// Find out how much data is left
			iWC -= PAGE4K;
		}while(iWC > PAGE4K);
		
		// If there is remaining data, send it with a ref tag
		if(iWC > 0)
		{
			// Get the physical address of each 4K page, and put it into the ref tag.
			// **********  look at this again
			DMARef.s.ADDR = GetPhysicalAddress((void *)(((char *)pStart) + (i*PAGE4K)));
			DMARef.s.QWC =  iWC >> 4;	// Transfer 4K 
			// Add the ref tag to the DMA chain.
			*pDMATag = DMARef.i64;
			pDMATag += 2;
		}
	}
	// The Packet is less than 4k - send it with one ref tag
	else
	{
		DMARef.i64 = 0;
		DMARef.s.QWC = iWC >> 4;	// Transfer all the data
		DMARef.s.ID = DMA_ID_SOURCE_REF;
	
		// Get the physical address of the data and put it into the ref tag.
		DMARef.s.ADDR = GetPhysicalAddress(pStart);
	
		// Add the ref tag to the DMA chain.
		*pDMATag = DMARef.i64;
		pDMATag += 2;
	}

	// Now that we have uploaded the data, the DMA chain needs to be ended.
	sps2DMATag_t DMAEnd;

	DMAEnd.i64 = 0;
	DMAEnd.s.QWC = 0;	
	DMAEnd.s.ID = DMA_ID_SOURCE_END;

	// Add the DMA tag to the chain
	*pDMATag = DMAEnd.i64;

	// Importand - Try to remember this!!!!!!!!!!!!!!!!!!!!
	FlushCache();
			
	// Set the address of the first tag (right at the start of the chain)
	tadr.i32 = 0;
	tadr.s.SPR = 0;
	tadr.s.ADDR = GetPhysicalAddress(pDMA);
	*EE_D1_TADR = tadr.i32;

	// Don't transfer any data from MADR
	*EE_D1_QWC = 0;

	// Set the channel control register
	chcr.i32 = 0;
	chcr.s.MOD = CHCR_MOD_CHAIN;
	chcr.s.STR = 1;
	chcr.s.DIR = 1;			// We are going FROM main memory
	chcr.s.TTE = 0; 		// Don't transfer the tag

	// Fire off the packet!
	*EE_D1_CHCR = chcr.i32;

	// Wait for it to complete
	WaitForDMA(1);
}