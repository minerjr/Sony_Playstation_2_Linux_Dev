#ifndef __SPS2WRAP_H__
#define __SPS2WRAP_H__

#include <sps2lib.h>
#include <sps2util.h>
#include <sps2tags.h>
#include <sps2regstructs.h>
#include <assert.h>

// Set up SPS2 and do initial allocation of memory block
// 1 Page = 4k Bytes
class CSPS2Manager
{
	public:
	
		CSPS2Manager();
		
		~CSPS2Manager();

		void Initialise(const int iAllocPages);

		void BeginScene(int R, int G, int B);
		
		void EndScene();

		void FlushCache();
		
		void WaitForDMA(int Channel);

		int GetScreenWidth();
		
		int GetScreenHeight();
				
		unsigned long GetPhysicalAddress(void * pMemory);
		
		void * GetTex1Base(void);
		void * GetTex2Base(void);
		void * GetTex3Base(void);
		void * GetTex4Base(void);
				
		void * GetTex1DmaBase(void);
		void * GetTex2DmaBase(void);
		void * GetTex3DmaBase(void);
		void * GetTex4DmaBase(void);
				
		void * GetPrim1Base(void);
		void * GetPrim2Base(void);
		void * GetPrim3Base(void);
		void * GetPrim4Base(void);
				
		void * GetFontTexBase(void);
		void * GetFontDmaBase(void);
		
		bool GetRunningStatus(void);
		void SetRunningStatus(bool Status);
		
		sps2Memory_t * GetMemoryStruct(void);
		int GetSPS2Handle(void);

	
	private:
	
		// Pointer to memory structure used to define memory allocated by SPS2
		sps2Memory_t * m_pMemCached;
		
		// Pointer to the start of allocated sps2 memory		
		char * m_pCached;
		
		// The SPS2 Kernel Module handle
		int m_iSPS2Desc;
		
		// Number of 4k Pages allocated
		int m_iNumPages;
		
		void * m_Tex1Base;
		void * m_Tex2Base;
		void * m_Tex3Base;
		void * m_Tex4Base;
		
		void * m_Tex1DmaBase;
		void * m_Tex2DmaBase;
		void * m_Tex3DmaBase;
		void * m_Tex4DmaBase;
		
		void * m_FontTexBase;
		void * m_FontDmaBase;
		
		void * m_Prim1Base;
		void * m_Prim2Base;
		void * m_Prim3Base;
		void * m_Prim4Base;
		
		// Running status of program
		bool m_Running;
		
};

inline void CSPS2Manager::BeginScene(int R, int G, int B)
{
	sps2UScreenClear(R, G, B);
}

inline void CSPS2Manager::EndScene()
{
	sps2UScreenSwap();
}

inline void CSPS2Manager::FlushCache()
{
	sps2FlushCache(m_iSPS2Desc);
}

inline void CSPS2Manager::WaitForDMA(int Channel)
{
	sps2WaitForDMA(Channel, m_iSPS2Desc);
}

inline void CSPS2Manager::SetRunningStatus(bool Status)
{
	m_Running = Status;
}

inline bool CSPS2Manager::GetRunningStatus(void)
{
	return m_Running;
}


#endif
