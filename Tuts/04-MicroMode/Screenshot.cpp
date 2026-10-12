/** @file Screenshot.cpp
	 Original file taken from sps2demo freely downloadable from linux kit project pages.
	 http://playstation2-linux.com/projects/sps2demo/
	 This file is only an adaptation to work separately from other file in sps2demo
*/

#include <linux/ps2/dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <sps2lib.h>
#include <sps2util.h>

#include "Screenshot.h"
/** .
	 Constructor needs to initialize the screenshot object, it needs SPS2descriptor
	 to allocate memory to do grabbing of the image.
*/
NablaPS2Utils::ScreenShot::ScreenShot(int SPS2Descriptor) : m_SPS2Descriptor(SPS2Descriptor) {

	//Allocate a block of memory to grab the image.
	m_memory = NULL;
	m_memory	= sps2Allocate(4096, SPS2_MAP_BLOCK_4K | SPS2_MAP_UNCACHED,
									 SPS2Descriptor);
}

/** .
*/
NablaPS2Utils::ScreenShot::~ScreenShot() {

	//Allocate a block of memory to grab the image.
	sps2Free(m_memory);
}


/** .
	 This is the only interface function of the class used to grab a shot of the
	 screen since this is mainly a debug routine performance is not necessary.
*/
void NablaPS2Utils::ScreenShot::TakeShot(char *FileName, EScreenType ScreenType = NTSC) {

	//First of all open the output file to write data into.
	FILE *f = fopen(FileName, "wb");

	// write a constant targa header for a requested size
	unsigned char header[18] = {

		0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x80, 2, 0, 0, 32, 8
	};
	int BufferHeight;
	switch (ScreenType) {

		case NTSC:
			header[14] = 0xC0;
			header[15] = 0x01;
			BufferHeight = 448;
		break;
		case PAL:
			header[14] = 0x00;
			header[15] = 0x02;
			BufferHeight = 512;
		break;
	}
	
	fwrite(header, 18, 1, f);

	//Now scan one row at a time to grab the whole screen
	int PyhisicalAddress = sps2GetPhysicalAddress(m_memory->pvStart, m_memory);
	//open event devices to be accessed with ioctl, refer to section  4.2 (GS manual version
	//3.1) Transmission process to understand settings.
	int eventfd = open(PS2_DEV_EVENT, O_RDWR);
	for(int y = 0; y < BufferHeight; y++) {

		DownloadVram(m_memory->pvStart, PyhisicalAddress, 
			sps2UScreenGetDrawBuffCurrent(), 10, 0, BufferHeight - 1 - y, 640, 1, eventfd);

		// convert RGBA to BGRA0
		unsigned char *data;
		unsigned char tmp;
		data = (unsigned char *)m_memory->pvStart;
		for(int x = 0; x < 640; x++) {

			//simply swap R and B using a temp variable, swapping is to be done for
			//the entrire row read.
			tmp = data[0];
			data[0] = data[2];
			data[2] = tmp;
			data += 4;
		}
		//write this row to output file
		fwrite(m_memory->pvStart, 4, 640, f);
	}
	close(eventfd);
	fclose(f);
	printf("Shot taken into file %s\n", FileName);
}

/** .
	 @param virtPage Virtual page of ram in witch grab the row of the image.
	 @param physPage Physical address of the page to be passed to DMAC and
						  GS engine.
    @param addr Is GS memory address from witch to grab data, zero is the 
					 start of frame buffer area
	 @param bufw Buffer width in unit of width / 64, since width of frame buffer
					 is 640 this value is usually 10.
	 @param x Starting x to begin the grab, usually this value is set to 0 
			  because a single line is grabbed. Remember that the image is 
			  addressed like a texture so (0, 0) is the upper left part of the
			  image itself.
    @param y Starting y to grab, this represent row index number.
	 @param w Width of the image to grab, usually 640 to take whole line.
	 @param h ?
	 @param eventfd The event file to permit to handle the events
*/
void NablaPS2Utils::ScreenShot::DownloadVram(void *virtPage, 
															int physPage, 
															int addr, 
															int bufw, 
															int x, 
															int y, 
															int w, 
															int h,
															int eventfd) {

	sps2uint32 *pDmaMem = (sps2uint32 *) virtPage;//<-- point to sps2 allocated unswapable memory.

	sps2uint32 uSrcPtr = addr << 5;
	//Size of data to be transferred is width (in unit of pixel) * 4 because since pixel format
	//is RGBA every pixel use 4 byte of space. h maybe is the heigth of the area to capture,
	//remember finally that all size are given in qword unit and since a qword is 16 bytes it is
	//necessary divide by 16
	sps2uint32 uQSize = (w*h*4) / 16; // assuming 24/32bit framebuffer

	// Since we are dealing with transfer to V01 a VIF code must be 
	//placed at the header of the paket
	//DIRECT instruct to trasfer data to GS, in this situation 6 qwords are transferred
	pDmaMem[0] = 0; //NOP;
	pDmaMem[1] = 0x06008000; //MSKPATH3(0x8000); Disable path 3
	pDmaMem[2] = 0x13000000; //FLUSHA;	
	pDmaMem[3] = 0x50000006; //DIRECT(6);

	//Access physical memory
	void *pPhysDmaMem = (void*)physPage;

	// Add GifTag Data, cast memory to u64 to easy build the packet
	sps2uint64 *pDmaMem64 = (sps2uint64 *) (pDmaMem + 4);
	pDmaMem64[0] = (PS2_GIFTAG_SET_TOPHALF(5, 1, 0, 0, 0, 1)); // GIFTAG(NLOOP, EOP, PRE, PRIM, FLG, NREG)
	pDmaMem64[1] = (0xe);

	pDmaMem64[2] = (PS2_GS_SETREG_BITBLTBUF( uSrcPtr, bufw, 0, 0, 0, 0 ));
	pDmaMem64[3] = (PS2_GS_BITBLTBUF);

	pDmaMem64[4] = (PS2_GS_SETREG_TRXPOS(x, y, 0, 0, 0)); // SSAX, SSAY, DSAX, DSAY, DIR
	pDmaMem64[5] = (PS2_GS_TRXPOS);

	pDmaMem64[6] = (PS2_GS_SETREG_TRXREG(w, h)); // RRW, RRh
	pDmaMem64[7] = (PS2_GS_TRXREG);

	pDmaMem64[8] = (0);
	pDmaMem64[9] = (PS2_GS_FINISH);

	pDmaMem64[10] = (PS2_GS_SETREG_TRXDIR(1)); // XDIR
	pDmaMem64[11] = (PS2_GS_TRXDIR);

	//Store back actual value of EE_D1_CHCR DMAC register, to restore at the end of the 
	//function.
	sps2uint32 prev_chcr = *EE_D1_CHCR;

	// vif1 must be available
	if ((*EE_D1_CHCR & 0x0100) != 0)	{

		//! Vif is not avaliable....why the assert(true?)
		assert(false);
	}	
	
	// set the FINISH event, refer to the manual that for transmission from local 
	// buffer to host state that first of all FINISH register must be accessed
	// this is made in gif packet.
	// and then wait until the FINISH field of CSR become 1
	// PS2IOC_ENABLEEVENT enable the code to read a particular event
	ioctl(eventfd, PS2IOC_ENABLEEVENT, PS2EV_FINISH);
	// PS2IOC_GETEVENT Reset to zero the event,
	ioctl(eventfd, PS2IOC_GETEVENT, PS2EV_FINISH);

	// DMA from memory and start DMA transfer
	sps2FlushCache(m_SPS2Descriptor);

	//7 qwords, first is VIF tag next are GIFtag and 5 A+d register setting
	*EE_D1_QWC = 0x7;
	*EE_D1_MADR = ( unsigned int ) pPhysDmaMem & 0x0FFFFFFF ;
	*EE_D1_CHCR = (1<<0) | (0<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);

	asm __volatile__( " sync.l " );
	// check if DMA is complete (STR=0)
	while ( *EE_D1_CHCR & 0x0100 );

	//Remember that 4 register of A+D list set a value in FINISH register, 
	//the value is not important because documentation states that register
	//has only to be accessed, now we must wait for the FINISH field of CSR become 1
	//this mean waiting for the event PS2EV_FINISH.
	ioctl(eventfd, PS2IOC_WAITEVENT, PS2EV_FINISH);
	
	// wait for VIF1-FIFO to empty and become idle
	while ( *EE_VIF1_STAT & 0x1f000003 );
	*((sps2uint32 *) EE_VIF1_STAT) = (1<<23);//(*EE_VIF1_STAT) | (1<<23);

	//Now following the instruction of the manual point 5, reverse the order
	//of FIFO of host I/F, then flush cache
	DPUT_GS_BUSDIR( ( sps2uint64 ) 0x00000001 );
	sps2FlushCache(m_SPS2Descriptor);

	// now do the real transfer, all is ready to move uQSize qword of data
	// channel 1 is used because we want to transfer from VIF to local.
	*EE_D1_QWC = uQSize;
	// Address to transfer is physical one, it is the same area referred by
	//logical one.
	*EE_D1_MADR = ( sps2uint32 ) physPage & 0x0FFFFFFF ;
	// start transfer, DIR flag is zero it means "to memory" 
	*EE_D1_CHCR = 0 | (0<<2) | (0<<4) | (0<<6) | (0<<7) | (1<<8);

	asm __volatile__( " sync.l " );

	// check if DMA is complete (STR=0)
	while ( *EE_D1_CHCR & 0x0100 );
	*EE_D1_CHCR = prev_chcr;
	asm __volatile__( " sync.l " );

	// wait for VIF1-FIFO to empty and become idle
	while ( *EE_VIF1_STAT & 0x1f000003 );
	//((tVIF1_STAT*)VIF1_STAT)->FDR = 0;
	*((sps2uint32 *) EE_VIF1_STAT) = 0;//(*EE_VIF1_STAT) & (~(1<<23));
	DPUT_GS_BUSDIR( ( sps2uint64 ) 0 );

	//enable again path 3 that was disabled at the beginning of the function
	static sps2uint32 enable_path3[4] __attribute__((aligned(16))) = {
					0x06000000, //MSKPATH3(0),
					0,
					0,
					0};

	DPUT_EE_VIF1_FIFO( *( sps2uint128 * ) enable_path3 );

	// back to the VBSTART event
	ioctl(eventfd, PS2IOC_ENABLEEVENT, PS2EV_VBSTART);
}
