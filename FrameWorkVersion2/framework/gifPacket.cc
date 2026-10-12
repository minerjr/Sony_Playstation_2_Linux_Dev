/****************************************************************************\
|* GIF packet definitions
|* ======================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG, various others
|* First created			: Sun Jul 21 12:45:40 BST 2002
|* Modification history		: See header file
|*
\****************************************************************************/
#include "gifPacket.h"

inline static u_long128 * align128(u_int* pPtr)
	{
	while((u_int)pPtr & 0xc)
		*pPtr++ = 0;
	return (u_long128*)pPtr;
	}
	
__inline__ static u_int CV2P(void* const pPtr)
	{
	return (u_int)pPtr & 0x9fffffff;
	}

/****************************************************************************\
|* Constructor
\****************************************************************************/
GifPacket::GifPacket()
	{
	}
	
/****************************************************************************\
|* Destructor
\****************************************************************************/
GifPacket::~GifPacket()
	{
	}
	

/****************************************************************************\
|* Method - initialise the GIF packet
\****************************************************************************/
void GifPacket::init(u_long128 *buffer)
	{	
	base	= align128((u_int*)buffer);
	current	= (u_int*)base;
	dmatag	= NULL;
	}

/****************************************************************************\
|* Method - reset the GIF packet
\****************************************************************************/
void GifPacket::reset()
	{
	current	= (u_int*)base;
	dmatag	= NULL;
	}


/****************************************************************************\
|* Method - terminate and return the GIF packet
\****************************************************************************/
u_long128 *GifPacket::terminate()
	{
	u_long128* const dma_current = align128(current);
	if(dmatag)
		{
		u_int const count    = (dma_current - dmatag) - 1;
		((u_int*)dmatag)[0] += count;
		}
	current = (u_int*) dma_current;
	dmatag  = NULL;
	return dma_current;
	}

/****************************************************************************\
|* Method - return the packet size
\****************************************************************************/
int GifPacket::size()
	{
	return ((u_int)current - (u_int)base) / sizeof(u_long128);
	}


/****************************************************************************\
|* Method - Add data to the packet
\****************************************************************************/
void GifPacket::add(u_long64 data)
	{
	*((u_long64*) current) = data;
	current               += sizeof(u_long64)/sizeof(u_int);
	}


/****************************************************************************\
|* Method - Add (packed) data to the packet
\****************************************************************************/
void GifPacket::add2(u_long128 data)
	{
	*((u_long128*) current) = data;
	current                += sizeof(u_long128) / sizeof(u_int);
	}

/****************************************************************************\
|* Method - Dump the packet
\****************************************************************************/
void GifPacket::dump()
	{
	unsigned int *dat=(unsigned int *)base;

	for (int i=0; i<size(); i++)
		{
		printf("-gifpacket: 0x%.8x 0x%.8x 0x%.8x 0x%.8x\n",
						dat[0], dat[1], dat[2], dat[3]);
		dat+=4;
		}
	}




