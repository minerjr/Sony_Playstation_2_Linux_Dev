#include "soundlib.h"
#include "joypadlib.h"
#include <stdio.h>

int main(void)
{
	int done = 0;				//Flag to exit the main loop

	int active_joypad = 0;		//Which joypad to get input from
	int buttons;				//Button presses for the joypad
	float left_stick[2], right_stick[2];
	float left_volume, right_volume;

	int sound;					//ID of created sound

	const char* sound_file = "leftright-stereo.wav";

	//Print out message about program
	printf("Press up and down on the left and right sticks on joypad %d to modify the left and right volumes of the sound.\n", active_joypad);
	printf("Press START on joypad %d to exit.\n", active_joypad);	

	//Initialize libraries
	soundlib_initialize(1, 0, 0, 0);
	joypadlib_initialize();	

	//Create and start playing the sound
	sound = soundlib_create_sound_from_file(sound_file, 0, 0);
	soundlib_play_sound(sound, 0, 1, SOUND_PLAY_BEGINNING);
	
	while (!done)
	{
		joypadlib_update();
		soundlib_update();

		buttons = joypadlib_get_joypad_buttons(active_joypad, 0, PRESSURE_TYPE_NONE);
		joypadlib_get_joypad_sticks(active_joypad, left_stick, right_stick);

		if (buttons & PS2PAD_BUTTON_START) 
			done = 1;

		//If a stick is all the way up, that's full volume 
		//If a stick is all the way down, that's no volume
		//If a stick is centered, that's half volume
		left_volume = 1.0f - (left_stick[1] + 1.0f) / 2.0f;
		right_volume = 1.0f - (right_stick[1] + 1.0f) / 2.0f;
		printf("Left volume: %f  Right volume: %f\n", left_volume, right_volume);
		soundlib_set_sound_volumes(sound, left_volume, right_volume);		
	}

	//Shutdown libraries
	soundlib_shutdown();	//Library will destroy used sounds
	joypadlib_shutdown();
}

