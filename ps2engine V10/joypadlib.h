/*
FILE: joypadlib.h
DESCRIPTION: The joypad library header (and documentation) file
WRITTEN BY: Derek Nedelman (nedelman@netzero.net)
DATE: June 19, 2002
HOME PAGE: http://www.playstation2-linux.com/projects/joypadlib 
*/

#ifndef JOYPADLIB_INCLUDED
#define JOYPADLIB_INCLUDED


//Includes---------------------------------------------------------------------
#include <linux/ps2/pad.h>


//Defines----------------------------------------------------------------------
//Used to access the array of pressures returned by joypadlib_get_joypad_buttons()
#define PS2PAD_PRESSURE_DIGITAL_RIGHT	0
#define PS2PAD_PRESSURE_DIGITAL_LEFT	1
#define PS2PAD_PRESSURE_DIGITAL_UP	2
#define PS2PAD_PRESSURE_DIGITAL_DOWN	3
#define PS2PAD_PRESSURE_TRIANGLE	4
#define PS2PAD_PRESSURE_CIRCLE		5
#define PS2PAD_PRESSURE_CROSS		6
#define PS2PAD_PRESSURE_SQUARE		7
#define PS2PAD_PRESSURE_L1		8
#define PS2PAD_PRESSURE_R1		9
#define PS2PAD_PRESSURE_L2		10
#define PS2PAD_PRESSURE_R2		11
#define PS2PAD_NUM_PRESSURE_BUTTONS	12

//For the 'type' parameter of joypadlib_get_joypad_buttons()
#define PRESSURE_TYPE_NONE	0	//No array passed in
#define PRESSURE_TYPE_BYTE	1	//'button_pressures' points to an array of unsigned chars (range: 0 through 255)
#define PRESSURE_TYPE_FLOAT	2	//'button_pressures' points to an array of floats (range: -1 through 1)

//For the 'mode' parameter of joypadlib_set_joypad_mode()
//Also, returned by joypadlib_get_joypad_mode()
#define MODE_DIGITAL	4
#define MODE_ANALOG	7

//The 'lock_unlock' parameter of joypadlib_set_joypad_mode()
#define MODE_UNLOCK	1
#define MODE_LOCK	2


//Prototypes-------------------------------------------------------------------
void joypadlib_initialize(void);
void joypadlib_shutdown(void);

int joypadlib_num_supported_joypads(void);

void joypadlib_update(void);


unsigned long joypadlib_get_joypad_buttons(int joypad, void* button_pressures, int type);
void joypadlib_get_joypad_sticks(int joypad, float left_stick[2], float right_stick[2]);

int joypadlib_joypad_supports_pressure(int joypad);

int joypadlib_get_joypad_status(int joypad);
int joypadlib_get_joypad_type(int joypad);

int joypadlib_get_joypad_num_modes(int joypad);
int joypadlib_get_joypad_mode(int joypad, int mode_index);
int joypadlib_set_joypad_mode(int joypad, int mode, int lock_unlock);

int joypadlib_get_joypad_num_actuators(int joypad);
void joypadlib_enable_joypad_actuators(int joypad, int enable_small, int enable_big);
void joypadlib_set_joypad_actuators(int joypad, unsigned char small_intensity, unsigned char big_intensity);


#endif
