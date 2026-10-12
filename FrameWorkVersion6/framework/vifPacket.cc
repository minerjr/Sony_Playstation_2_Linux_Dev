/****************************************************************************\
|* VIF packet definitions
|* ======================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG, various others
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/
#include "vifPacket.h"

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
VifPacket::VifPacket()
	{
	}
	
/****************************************************************************\
|* Destructor
\****************************************************************************/
VifPacket::~VifPacket()
	{
	}
	
/****************************************************************************\
|* Method - Align the packet to a position & bit
\****************************************************************************/
void VifPacket::align(u_int bit, u_int pos)
	{
	u_int const mask = 0xffffffff >> (32 - ((bit + 2) & 0x1f));
	u_int target 	 = ((u_int)current & ~mask) + pos * sizeof(u_int);
	if(target < (u_int)current)
		target += mask + 1;
		
	while(target > (u_int)current)
		*current ++ = 0;
	}

/****************************************************************************\
|* Method - Initialise the packet
\****************************************************************************/
void VifPacket::init(u_long128 *buffer)
	{
	base    	= buffer;
	current 	= (u_int*)base;
	dmatag  	= NULL;
	vifcodes	= NULL;
	len			= 0;
	giftag		= NULL;
	pad2 		= 0;
	pad3		= 0;
	}

/****************************************************************************\
|* Method - Reset the packet
\****************************************************************************/
void VifPacket::reset()
	{
	current = (u_int*)base;
	dmatag  = NULL;
	}

/****************************************************************************\
|* Method - terminate the packet and return the packet data
\****************************************************************************/
u_long128 * VifPacket::terminate()
	{
	u_long128 * dma_current = align128(current);
	if (dmatag != NULL)
		{
		u_int count          = ((u_int) dma_current - (u_int) dmatag)/16 -1;
		((u_int*)dmatag)[0] += count;
		}
	
	current=(u_int*) dma_current;

	dmatag = NULL;
	return dma_current;
	}


/****************************************************************************\
|* Method - return the size of the packet
\****************************************************************************/
int VifPacket::size()
	{
	return ((u_int)current - (u_int)base) / sizeof(u_long128);
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::cnt(u_int flag)
	{
	dmatag     = terminate();
	*current++ = (1 << 28) | flag;
	*current++ = 0;
	*current++ = 0;
	*current++ = 0;
	vifcodes   = NULL;
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::next(u_long128 *next, u_int flag)
	{
	dmatag     = terminate();
	*current++ = (2 << 28) | flag;
	*current++ = CV2P(next);
	*current++ = 0;
	*current++ = 0;
	vifcodes   = NULL;
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::end(u_int flag)
	{
	dmatag     = terminate();
	*current++ = (7 << 28) | flag;
	*current++ = 0;
	*current++ = 0;
	*current++ = 0;
	vifcodes   = NULL;
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::ref(u_long128 *ref, u_int size, u_int opt1, u_int opt2,
                    u_int flag)
	{
	terminate();
	*current++ = (0 << 28) | flag | size;
	*current++ = CV2P(ref);
	*current++ = opt1;
	*current++ = opt2;
	}


/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::openDirectCode(int stall)
	{
	align(2, 3);
	vifcodes    = current;
	*current ++ = ((stall ? 0x80 : 0) | 0x50) << 24;
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::closeDirectCode()
	{
	u_int const count = (current - (vifcodes + 1)) /
	                    (sizeof(u_long128) / sizeof(u_int));
	*vifcodes += count;
	vifcodes   = NULL;
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::addCode(u_int code)
	{
	*current++ = code;
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::openUpkCode(u_short vuaddr, u_int upkcmd, u_int cl, u_int wl)
	{
	*current++ = (0x01 << 24) | (wl << 8) | cl;
	vifcodes   = current;
	*current++ = (upkcmd << 24) | vuaddr;

	u_int const clVal = (cl == 0) ? 256 : cl;
	u_int const wlVal = (wl == 0) ? 256 : wl;
	u_int length      = (32 >> (upkcmd & 0x03)) * (((upkcmd >> 2) & 0x03) +1);
	if(wlVal <= clVal)
		len = (1 << 16) | length;
	else
		{
		length *= clVal;
		len     = (wlVal << 16) | length;
		}
	}

/****************************************************************************\
|* Method - ???
\****************************************************************************/
void VifPacket::closeUpkCode()
	{
	u_int const bitcount = (current - (vifcodes +1)) * sizeof(u_int) * 8;
	u_int const numlen   = len;
	u_int const length   = numlen  & 0x0000ffff;
	u_int const num      = (numlen & 0xffff0000) * ( bitcount / length);
	
	*vifcodes           += num;
	vifcodes             = NULL;
	}

/****************************************************************************\
|* Method - add data to the packet
\****************************************************************************/
void VifPacket::add(u_int data)
	{
  	*current++ = data;
	}

/****************************************************************************\
|* Method - add data to the packet
\****************************************************************************/
void VifPacket::addq(u_long64 data)
	{
	*current++ = (u_int)data;
	*current++ = (u_int)(data >> 32);
	}

/****************************************************************************\
|* Method - add data to the packet
\****************************************************************************/
void VifPacket::addq(u_long64* pData, u_int count)
	{
	while(count--)
		{
		u_long64 const data = *pData++;
		*current++ = (u_int)data;
		*current++ = (u_int)(data >> 32);
		}
	}

/****************************************************************************\
|* Method - add data to the packet
\****************************************************************************/
void VifPacket::add2q(u_long128 data )
	{
	u_long64 const low = ((u_long64*)  &data)[0];
	u_long64 const high = ((u_long64*) &data)[1];
	
	*current++ = (u_int)low;
	*current++ = (u_int)(low >> 32);
	*current++ = (u_int)high;
	*current++ = (u_int)(high >> 32);
	}

/****************************************************************************\
|* Method - add data to the packet
\****************************************************************************/
void VifPacket::add2q(u_long128* pData, u_int count)
	{
	while(count--)
		{
		u_long64 const low  = ((u_long64*) pData)[0];
		u_long64 const high = ((u_long64*) pData)[1];
		pData++;
		
		*current++ = (u_int)low;
		*current++ = (u_int)(low >> 32);
		*current++ = (u_int)high;
		*current++ = (u_int)(high >> 32);
		}
	}

/****************************************************************************\
|* Method - dump the packet
\****************************************************************************/
void VifPacket::dump()
	{
	unsigned int *dat=(unsigned int *)base;

	for (int i=0; i<size(); i++)
		{
		_log.printf("vifpacket: 0x%.8x 0x%.8x 0x%.8x 0x%.8x\n",
		                  dat[0], dat[1], dat[2], dat[3]);
		dat+=4;
		}
	}



