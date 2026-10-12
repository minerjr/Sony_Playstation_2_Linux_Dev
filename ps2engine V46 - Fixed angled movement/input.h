#include "PS2Pad.h"

#ifndef INPUT_H
#define INPUT_H

class Input_Engine
{

private:
	PS2Pad* joypad;

public:

	Input_Engine();
	~Input_Engine();

	int readInput();
	PS2Pad *getInputDevice();
};

#endif