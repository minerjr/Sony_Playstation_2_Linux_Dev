/** @file JoypadConst.h
*/

#ifndef _JOY_CONST__
#define _JOY_CONST__

/** Used to determine both present and past status of the button.
*/
enum ButtonState {

	Unpressed = 0, //!< Button is unpressed, last call was unpressed also.
	Pressed,			//!< Button is pressed, last call was unpressed
	MPressed,		//!< Button is mantained pressed, last call was also pressed
	Released			//!< Button is not pressed, last call was pressed
};

/**
	These value are the index in internal structure array of the 
	JoypadManager class, but are also the shift used to find
	corresponding bit in data read from playstation pad.
*/
#define SQUARE		0x000F
#define CROSS		0x000E
#define CIRCLE		0x000D
#define TRIANGLE	0x000C
#define R1			0x000B
#define L1			0x000A
#define R2			0x0009
#define L2			0x0008
#define LEFT		0x0007
#define DOWN		0x0006
#define RIGHT		0x0005
#define UP			0x0004
#define START		0x0003
#define R3			0x0002
#define L3			0x0001
#define SELECT		0x0000


inline char *ButtonNameFromIndex(int ButtonIndex) {

	switch (ButtonIndex) {
	
		case L2:
		return "L2";
		case R2:
		return "R2";
		case L1:
		return "L1";
		case R1:
		return "R1";
		case TRIANGLE:
		return "TRIANGLE";
		case CIRCLE:
		return "CIRCLE";
		case CROSS:
		return "CROSS";
		case SQUARE:
		return "SQUARE";

		case LEFT:
		return "LEFT";
		case DOWN:
		return "DOWN";
		case RIGHT:
		return "RIGHT";
		case UP:
		return "UP";
		case START:
		return "START";
		case SELECT:
		return "SELECT";
		case L3:
		return "L3";
		case R3:
		return "R3";
	}
	return "";
}

//!Index to refer to analog stick
#define RX			0x0000
#define RY			0x0001
#define LX			0x0002
#define LY			0x0003
#endif