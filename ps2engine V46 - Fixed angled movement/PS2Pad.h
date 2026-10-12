/*
FILE: PS2Pad.h
DESCRIPTION: Header file for the PS2PadVibrationEffect, PS2Pad and PS2PadManager classes
WRITTEN BY: Derek Nedelman (nedelman@netzero.net)
DATE: August 12, 2002
HOME PAGE: http://www.playstation2-linux.com/projects/joypadlib 
*/


#ifndef PS2PAD_INCLUDED
#define PS2PAD_INCLUDED


//Includes---------------------------------------------------------------------
extern "C"
{
#include "joypadlib.h"
};


//Classes----------------------------------------------------------------------
class PS2PadVibrationEffect
{
public:
	virtual ~PS2PadVibrationEffect()	{}

	virtual void Update(float elapsedTime) = 0;
	virtual bool IsDone() const = 0;	
	virtual void GetIntensities(unsigned char& smallIntensity, unsigned char& bigIntensity) = 0;
};


class PS2Pad
{
	friend class PS2PadManager;

public:
	enum
	{
		BUTTON_RIGHT = PS2PAD_PRESSURE_DIGITAL_RIGHT,
		BUTTON_LEFT = PS2PAD_PRESSURE_DIGITAL_LEFT,
		BUTTON_UP = PS2PAD_PRESSURE_DIGITAL_UP,
		BUTTON_DOWN = PS2PAD_PRESSURE_DIGITAL_DOWN,
		BUTTON_TRIANGLE = PS2PAD_PRESSURE_TRIANGLE,
		BUTTON_CIRCLE = PS2PAD_PRESSURE_CIRCLE,
		BUTTON_CROSS = PS2PAD_PRESSURE_CROSS,
		BUTTON_SQUARE = PS2PAD_PRESSURE_SQUARE,
		BUTTON_L1 = PS2PAD_PRESSURE_L1,
		BUTTON_R1 = PS2PAD_PRESSURE_R1,
		BUTTON_L2 = PS2PAD_PRESSURE_L2,
		BUTTON_R2 = PS2PAD_PRESSURE_R2,
		BUTTON_L3,
		BUTTON_R3,
		BUTTON_SELECT,
		BUTTON_START,
		NUM_BUTTONS
	};
	
	enum 
	{
		BUTTON_RIGHT_BIT	= 0x00000001,
		BUTTON_LEFT_BIT		= 0x00000002,
		BUTTON_UP_BIT		= 0x00000004,
		BUTTON_DOWN_BIT		= 0x00000008,
		BUTTON_TRIANGLE_BIT 	= 0x00000010,
		BUTTON_CIRCLE_BIT	= 0x00000020,
		BUTTON_CROSS_BIT	= 0x00000040,
		BUTTON_SQUARE_BIT	= 0x00000080,
		BUTTON_L1_BIT		= 0x00000100,
		BUTTON_R1_BIT		= 0x00000200,
		BUTTON_L2_BIT		= 0x00000400,
		BUTTON_R2_BIT		= 0x00000800,
		BUTTON_L3_BIT		= 0x00001000,
		BUTTON_R3_BIT		= 0x00002000,
		BUTTON_SELECT_BIT	= 0x00004000,
		BUTTON_START_BIT	= 0x00008000
	};

protected:
	int index;

	struct PadState
	{
		unsigned long buttons;
		float buttonPressures[PS2PAD_NUM_PRESSURE_BUTTONS];
		float leftStick[2];
		float rightStick[2];

		bool isPluggedIn;
		int type;
	};	
	PadState previousState;
	PadState state;
	PS2PadVibrationEffect* vibrationEffect;

	float buttonVelocities[NUM_BUTTONS];
	
public:	
				
	//Multiple button pressing
	bool WasPreviouslyPressed(unsigned long buttons) const;
	bool IsPressed(unsigned long buttons) const;
	bool WasPressed(unsigned long buttons) const;
	bool IsHeld(unsigned long buttons) const;	
	bool WasReleased(unsigned long buttons) const;

	//Single button pressing
	bool WasButtonPreviouslyPressed(int button) const;
	bool IsButtonPressed(int button) const;
	bool WasButtonPressed(int button) const;
	bool IsButtonHeld(int button) const;
	bool WasButtonReleased(int button) const;
	
	//Status
	bool IsPluggedIn() const	{return state.isPluggedIn;}
	bool WasTypeChanged() const	{return previousState.type != state.type;} 
	int GetType() const		{return state.type;}
	void GetSticks(float leftStick[2], float rightStick[2]);		
	float GetLeftX() const		{return state.leftStick[0];}
	float GetLeftY() const		{return state.leftStick[1];}
	float GetRightX() const		{return state.rightStick[0];}
	float GetRightY() const		{return state.rightStick[1];}
	float GetPressure(int button) const   {return state.buttonPressures[button];}
	float GetVelocity(int button) const   {return buttonVelocities[button];}

	//Vibration
	void SetVibrationEffect(PS2PadVibrationEffect* effect);	
	
protected:
	void Update(float elapsedTime);	
};


class PS2PadManager
{
	enum {MAX_JOYPADS = 2};
	
	static int numPads;
	static PS2Pad pads[MAX_JOYPADS];
	
	static bool isInitialized;

public:
	static void Initialize();
	static void Shutdown();
	
	static void Update(float elapsedTime = 1.0f);

	static int GetNumPads()		{return numPads;}
	static PS2Pad* GetPad(int pad);
};


#endif
