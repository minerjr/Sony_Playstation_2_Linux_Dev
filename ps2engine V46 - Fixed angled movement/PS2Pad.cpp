/*
FILE: PS2Pad.cpp
DESCRIPTION: Implementation file for the PS2PadVibrationEffect, PS2Pad and PS2PadManager classes
WRITTEN BY: Derek Nedelman (nedelman@netzero.net)
DATE: August 12, 2002
HOME PAGE: http://www.playstation2-linux.com/projects/joypadlib 
*/


//Includes---------------------------------------------------------------------
#include "PS2Pad.h"


//Implementation---------------------------------------------------------------


//PS2Pad-----------------------------------------------------------------------
bool PS2Pad::WasPreviouslyPressed(unsigned long buttons) const
{
	return (buttons & previousState.buttons) == buttons;
}

bool PS2Pad::IsPressed(unsigned long buttons) const
{
	return (buttons & state.buttons) == buttons;
}	

bool PS2Pad::WasPressed(unsigned long buttons) const
{
	return !WasPreviouslyPressed(buttons) && IsPressed(buttons);
}

bool PS2Pad::IsHeld(unsigned long buttons) const
{
	return WasPreviouslyPressed(buttons) && IsPressed(buttons);
}

bool PS2Pad::WasReleased(unsigned long buttons) const
{
	return WasPreviouslyPressed(buttons) && !IsPressed(buttons);
}

bool PS2Pad::WasButtonPreviouslyPressed(int button) const
{
	return ((1L << button) & previousState.buttons) != 0;
}

bool PS2Pad::IsButtonPressed(int button) const
{
	return ((1L << button) & state.buttons) != 0;
}	

bool PS2Pad::WasButtonPressed(int button) const
{
	return !WasButtonPreviouslyPressed(button) && IsButtonPressed(button);
}

bool PS2Pad::IsButtonHeld(int button) const
{
	return WasButtonPreviouslyPressed(button) && IsButtonPressed(button);
}

bool PS2Pad::WasButtonReleased(int button) const
{
	return WasButtonPreviouslyPressed(button) && !IsButtonPressed(button);
}

void PS2Pad::GetSticks(float leftStick[2], float rightStick[2])
{
	leftStick[0] = state.leftStick[0];
	leftStick[1] = state.leftStick[1];

	rightStick[0] = state.rightStick[0];
	rightStick[1] = state.rightStick[1];
}

void PS2Pad::SetVibrationEffect(PS2PadVibrationEffect* effect)
{		
	if (effect == 0)
		joypadlib_set_joypad_actuators(index, 0, 0);

	vibrationEffect = effect;
}

void PS2Pad::Update(float elapsedTime)
{
	int i;
	unsigned long buttons;

	previousState = state;

	state.isPluggedIn = joypadlib_get_joypad_status(index) == PS2PAD_STAT_READY;
	state.type = joypadlib_get_joypad_type(index);
			
	buttons = joypadlib_get_joypad_buttons(index, state.buttonPressures, PRESSURE_TYPE_FLOAT);
	state.buttons = 0;
	if (buttons & PS2PAD_BUTTON_RIGHT)		state.buttons |= BUTTON_RIGHT_BIT;
	if (buttons & PS2PAD_BUTTON_LEFT)		state.buttons |= BUTTON_LEFT_BIT;
	if (buttons & PS2PAD_BUTTON_UP)			state.buttons |= BUTTON_UP_BIT;
	if (buttons & PS2PAD_BUTTON_DOWN)		state.buttons |= BUTTON_DOWN_BIT;
	if (buttons & PS2PAD_BUTTON_TRIANGLE)		state.buttons |= BUTTON_TRIANGLE_BIT;
	if (buttons & PS2PAD_BUTTON_CIRCLE)		state.buttons |= BUTTON_CIRCLE_BIT;
	if (buttons & PS2PAD_BUTTON_CROSS)		state.buttons |= BUTTON_CROSS_BIT;
	if (buttons & PS2PAD_BUTTON_SQUARE)		state.buttons |= BUTTON_SQUARE_BIT;
	if (buttons & PS2PAD_BUTTON_L1)			state.buttons |= BUTTON_L1_BIT;
	if (buttons & PS2PAD_BUTTON_R1)			state.buttons |= BUTTON_R1_BIT;
	if (buttons & PS2PAD_BUTTON_L2)			state.buttons |= BUTTON_L2_BIT;
	if (buttons & PS2PAD_BUTTON_R2)			state.buttons |= BUTTON_R2_BIT;
	if (buttons & PS2PAD_BUTTON_L3)			state.buttons |= BUTTON_L3_BIT;
	if (buttons & PS2PAD_BUTTON_R3)			state.buttons |= BUTTON_R3_BIT;
	if (buttons & PS2PAD_BUTTON_SELECT)		state.buttons |= BUTTON_SELECT_BIT;
	if (buttons & PS2PAD_BUTTON_START)		state.buttons |= BUTTON_START_BIT;

	joypadlib_get_joypad_sticks(index, state.leftStick, state.rightStick);

	if (elapsedTime > 0.0f)
	{
		float oneOverElapsedTime = 1.0f/elapsedTime;

		for (i = 0; i < PS2PAD_NUM_PRESSURE_BUTTONS; i++)
		{
			buttonVelocities[i] = (state.buttonPressures[i] - previousState.buttonPressures[i]) * oneOverElapsedTime;
		}

		buttonVelocities[BUTTON_L3] = WasButtonPressed(BUTTON_L3) ? oneOverElapsedTime : 0.0f;
		buttonVelocities[BUTTON_R3] = WasButtonPressed(BUTTON_R3) ? oneOverElapsedTime : 0.0f;
		buttonVelocities[BUTTON_SELECT] = WasButtonPressed(BUTTON_SELECT) ? oneOverElapsedTime : 0.0f;
		buttonVelocities[BUTTON_START] = WasButtonPressed(BUTTON_START) ? oneOverElapsedTime : 0.0f;
	}

	if (vibrationEffect)
	{
		vibrationEffect->Update(elapsedTime);
		if (vibrationEffect->IsDone())
		{
			SetVibrationEffect(0);
		}
		else
		{
			unsigned char smallIntensity = 0, 
				      bigIntensity = 0;
			vibrationEffect->GetIntensities(smallIntensity, bigIntensity);
		
			joypadlib_set_joypad_actuators(index, smallIntensity, bigIntensity);
		}
	}
}

void PS2PadManager::Initialize()
{	
	if (!isInitialized)
	{
		int i;

		joypadlib_initialize();
		numPads = joypadlib_num_supported_joypads();

		for (i = 0; i < numPads; i++)
			pads[i].index = i;

		isInitialized = true;
	}

	//We have to Update() twice to completely wipe out original values
	Update(0.0f);
	Update(0.0f);
}


//PS2PadManager----------------------------------------------------------------

//Static initialization
int PS2PadManager::numPads = 0;
PS2Pad PS2PadManager::pads[PS2PadManager::MAX_JOYPADS];	
bool PS2PadManager::isInitialized = false;

void PS2PadManager::Shutdown()
{
	if (isInitialized)
	{
		joypadlib_shutdown();
		isInitialized = false;
	}
}

void PS2PadManager::Update(float elapsedTime)
{
	int i;

	joypadlib_update();

	for (i = 0; i < numPads; i++)
	{
		pads[i].Update(elapsedTime);
	}
}

PS2Pad* PS2PadManager::GetPad(int pad)
{
	return &pads[pad];
}
