#include "sps2wrap.h"

CSPS2Manager::CSPS2Manager()
{
	m_Running = true;
	m_pMemCached = NULL;
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
	
	m_pMemCached = sps2Allocate(iAllocPages * 4096,
			SPS2_MAP_BLOCK_4K | SPS2_MAP_CACHED, m_iSPS2Desc);
			
	if(m_pMemCached < 0)
	{
		fprintf(stderr, "Failed to Allocate SPS2 memory.\n");
		exit(-1);
	}
			
	m_pCached = (char *)m_pMemCached->pvStart;
	
	// This arrangement needs 640 4k blocks minimum
	
	m_FontDmaBase = (void *)(&m_pCached[(m_iNumPages - 32) * 4096]);
	
	m_Tex1DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 1) * 4096]);
	m_Tex2DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 2) * 4096]);
	m_Tex3DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 3) * 4096]);
	m_Tex4DmaBase = (void *)(&m_pCached[(m_iNumPages - 32 - 4) * 4096]);
	
	// Some free memory at this point
	
	m_FontTexBase = (void *)(&m_pCached[(m_iNumPages - 64 * 2) * 4096]);
	
	m_Tex1Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 3) * 4096]);
	m_Tex2Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 4) * 4096]);
	m_Tex3Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 5) * 4096]);
	m_Tex4Base = 	(void *)(&m_pCached[(m_iNumPages - 64 * 6) * 4096]);
	
	m_Prim4Base = 	(void *)(&m_pCached[64 * 3 * 4096]);
	m_Prim3Base = 	(void *)(&m_pCached[64 * 2 * 4096]);
	m_Prim2Base = 	(void *)(&m_pCached[64 * 1 * 4096]);
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

void * CSPS2Manager::GetFontTexBase(void)
{ 
	return m_FontTexBase; 
}
		
void * CSPS2Manager::GetFontDmaBase(void)
{ 
	return m_FontDmaBase; 
}

