#include "input.h"


Input_Engine::Input_Engine()
{
	PS2PadManager::Initialize();
	joypad = PS2PadManager::GetPad(0);
}

Input_Engine::~Input_Engine()
{
	PS2PadManager::Shutdown();
}

int Input_Engine::readInput()
{
	PS2PadManager::Update();

	if (joypad->IsPluggedIn() && joypad->GetType() == PS2PAD_TYPE_DUALSHOCK)return 1;
	else return 0;
}

PS2Pad *Input_Engine::getInputDevice()
{
	return joypad;
}