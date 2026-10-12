/** @file main.cpp
	Dump read stream of Joypad 1.
	This simple application is used to scan output data of pad, with
	these data we can construct a class to manage the pad that permits
	to fully access feature of the pad.
*/
#include <cstdio>
#include <linux/ps2/pad.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <iomanip>
#include <climits>
#include <bitset>

/** Dump data.
	 This accept an integer and dump its content subdivided in byte
	 dump it is done in binary and exadecimal form.
*/
void DumpInt(int Data) {

	//Write down a little mask to identify bit number
	std::cout << "87654321 87654321 87654321 87654321\n";
	unsigned char *D = (unsigned char *) &Data;
	std::bitset<8> BinData0(D[0]), BinData1(D[1]),
						BinData2(D[2]), BinData3(D[3]);
	std::cout << BinData0 << " " << BinData1 << " "
				 << BinData2 << " " << BinData3 << " " 
				 << '(' << std::setw(3) << (unsigned int) D[0] 
				 << ',' << std::setw(3) << (unsigned int) D[1] 
				 << ',' << std::setw(3) << (unsigned int) D[2] 
				 << ',' << std::setw(3) << (unsigned int) D[3] << ')' 
				 << "\n\n";
}

/** Simple routine to position output in a linux terminal  
*/
void gotoxy(int x, int y) {

	std::cout << "\E[0" << x << ';' << y << 'H';
}

/** Simple routine to clear screen in a linux terminal  
*/
void clrscr() {

	std::cout << "50\E[H\E[J";
}

int main() {

	//Open the pad and wait until pad is not busy
	int Joypad0 = open("/dev/ps2pad00", O_RDONLY);
	int PadStatus;
	ioctl(Joypad0, PS2PAD_IOCGETSTAT, &PadStatus);
	while(PadStatus == PS2PAD_STAT_BUSY) {
		
		ioctl(Joypad0, PS2PAD_IOCGETSTAT, &PadStatus);
	} 
	if (PadStatus == PS2PAD_STAT_NOTCON) {
	
		//Close application because no pad was found
		close(Joypad0);
		std::cout << "No joypad detected in port 1\n";
		exit(2);
	}
	if (PadStatus == PS2PAD_STAT_ERROR) {
		 
		//Close the application because errors occured while
		//the application try to open the pad
		close(Joypad0);
		std::cout << "Unable to initialize the pad, ERROR!!!!\n";
		exit(1);
	}

	//This example suppose that joypad used is DualShock pad 
	//Use analog button for the joypad
	ps2pad_mode PS2PMode;
	PS2PMode.offs = 1;
	PS2PMode.lock = 1;
	ioctl(Joypad0, PS2PAD_IOCSETMODE, &PS2PMode);
	ioctl(Joypad0, PS2PAD_IOCENTERPRESSMODE);
	//Prepare a structure in wich read data
	unsigned int PadData[PS2PAD_DATASIZE / 4] = {0};
	clrscr();
	for (; ;) {
		
		//Read and dump the data.
		read(Joypad0, PadData, PS2PAD_DATASIZE);
		gotoxy(1,1);
		DumpInt(PadData[0]);
		DumpInt(PadData[1]);
		DumpInt(PadData[2]);
		DumpInt(PadData[3]);
		DumpInt(PadData[4]);
		DumpInt(PadData[5]);
	}
	close(Joypad0);
}
