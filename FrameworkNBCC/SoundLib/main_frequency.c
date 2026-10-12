#include "soundlib.h"
#include "joypadlib.h"
#include <stdio.h>

int main(void)
{
	int done = 0;				//Flag to exit the main loop

	int active_joypad = 0;		//Which joypad to get input from
	int buttons;				//Button presses for the joypad
	float left_stick[2], right_stick[2];
	
	int sound;					//ID of created sound

	const char* sound_file = "engine-stereo.wav";

	int default_frequency = 48000;
	int frequency;

	//Print out message about program
	printf("Press up and down on the left stick on joypad %d to modify the playback frequency of the sound.\n", active_joypad);
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

		//If the stick is all the way up, that's max_frequency
		//If the stick is all the way down, that's min_frequency
		//If the stick is centered, that's normal frequency
		if (left_stick[1] < 0)
			frequency = default_frequency + -left_stick[1] * default_frequency * 4;
		else if (left_stick[1] > 0)
			frequency = default_frequency - left_stick[1] * default_frequency;
		else
			frequency = default_frequency;

		printf("Frequency: %d\n", frequency);
		
		soundlib_set_sound_frequency(sound, frequency);
	}

	//Shutdown libraries
	soundlib_shutdown();	//Library will destroy used sounds
	joypadlib_shutdown();
}
