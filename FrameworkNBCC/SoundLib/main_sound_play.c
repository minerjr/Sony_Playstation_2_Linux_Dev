#include "soundlib.h"
#include "joypadlib.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	int done = 0;				//Flag to exit the main loop

	int active_joypad = 0;		//Which joypad to get input from
	int buttons;				//Button presses for the joypad
		
	int sound;					//ID of created sound

	char* sound_file;

	//Make sure a filename was specified
	if (argc < 2)
	{
		fprintf(stderr, "The format of this program is: %s (filename)\n", argv[0]);
		return 1;
	}
	sound_file = argv[1];

	//Print out message about program
	printf("This demo will now play %s.\n", sound_file);
	printf("Press START on joypad %d to exit.\n", active_joypad);	

	//Initialize libraries
	soundlib_initialize(1, 0, 0, 0);
	joypadlib_initialize();	

	//Create and start playing the sound
	sound = soundlib_create_sound_from_file(sound_file, 0, 0);//The 'is_streaming' parameter is ignored
	if (sound == -1)
	{
		fprintf(stderr, "Couldn't create a sound for %s\n", sound_file);
		soundlib_shutdown();
		joypadlib_shutdown();
		return 1;
	}
	soundlib_play_sound(sound, 0, 1, SOUND_PLAY_BEGINNING);
	
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
