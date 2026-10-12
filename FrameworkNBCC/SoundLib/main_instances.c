#include "soundlib.h"
#include "joypadlib.h"
#include <stdio.h>

int main(void)
{
	int done = 0;				//Flag to exit the main loop

	int active_joypad = 0;		//Which joypad to get input from
	int buttons;				//Button presses for the joypad

	const int num_sounds = 32;	//Maximum number of sounds for demo to use
	int sounds[num_sounds];		//IDs of created sounds

	int sound;					//Looping variable
	const char* sound_file = "1.wav";

	//Print out message about program
	printf("It's hard to tell, but %d sounds (1 original and %d instances)\nare played in this demo.\n", num_sounds, num_sounds - 1);	
	printf("Press START on joypad %d to exit.\n", active_joypad);	
	
	//Initialize libraries
	soundlib_initialize(num_sounds, 0, 0, 0);
	joypadlib_initialize();

	//Create the main sound first
	sounds[0] = soundlib_create_sound_from_file(sound_file, 0, 0);

	//Now create the instances	
	for (sound = 1; sound < num_sounds; sound++)
	{
		sounds[sound] = soundlib_create_sound_instance(sounds[0]);		
	}

	//Now play all the sounds
	for (sound = 0; sound < num_sounds; sound++)
	{
		soundlib_play_sound(sounds[sound], 0, 1, SOUND_PLAY_BEGINNING);		
	}

	while (!done)
	{
		joypadlib_update();
		soundlib_update();

		buttons = joypadlib_get_joypad_buttons(active_joypad, 0, PRESSURE_TYPE_NONE);

		if (buttons & PS2PAD_BUTTON_START) 
			done = 1;
	}	

	//Shutdown libraries
	soundlib_shutdown();	//Library will destroy used sounds
	joypadlib_shutdown();
}
