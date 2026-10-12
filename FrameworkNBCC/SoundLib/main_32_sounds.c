#include "soundlib.h"
#include "joypadlib.h"

int main(void)
{
	int done = 0;				//Flag to exit the main loop

	int active_joypad = 0;		//Which joypad to get input from
	int buttons;				//Button presses for the joypad

	const int num_sounds = 32;	//Maximum number of sounds for demo to use
	int sounds[num_sounds];		//IDs of created sounds

	int sound;					//Looping variable
	const int num_sound_files = 6;	//Number of different sound files to load from
	const char* sound_files[] = {"1.wav", "2.wav", "3.wav", "4.wav", "5.wav", "outofhere-mono.wav"};

	//const char* mp3file = "outofhere-mono.wav";

	int i;

	//Print out message about program
	printf("It's hard to tell, but %d sounds are played in this demo.\n", num_sounds);
	printf("Press START on joypad %d to exit.\n", active_joypad);	

	//Initialize libraries
	soundlib_initialize(num_sounds, 0, 0, 0);
	joypadlib_initialize();

	//sounds[0] = soundlib_create_sound_from_file(mp3file, 0, 0);
	//soundlib_play_sound(sounds[0], 0, 1, SOUND_PLAY_BEGINNING);	

	//Create and start playing all the sounds
	for (sound = 0; sound < num_sounds; sound++)
	{
		sounds[sound] = soundlib_create_sound_from_file(sound_files[sound % num_sound_files], 0, 0);
//soundlib_play_sound(sounds[sound], 0, 1, SOUND_PLAY_BEGINNING);		
	}

	while (!done)
	{
		joypadlib_update();
		soundlib_update();

		buttons = joypadlib_get_joypad_buttons(active_joypad, 0, PRESSURE_TYPE_NONE);
		if (buttons & PS2PAD_BUTTON_START) 
			done = 1;
		else if(buttons & PS2PAD_BUTTON_CROSS)
	               soundlib_play_sound(sounds[0], 0, 0, SOUND_PLAY_BEGINNING);	
		else if(buttons & PS2PAD_BUTTON_SQUARE)
	               soundlib_play_sound(sounds[2], 0, 0, SOUND_PLAY_BEGINNING);
		else if(buttons & PS2PAD_BUTTON_TRIANGLE)
	               soundlib_play_sound(sounds[5], 0, 0, SOUND_PLAY_BEGINNING);
		else if(buttons & PS2PAD_BUTTON_CIRCLE)
	               soundlib_play_sound(sounds[4], 0, 0, SOUND_PLAY_BEGINNING);

//		soundlib_play_sound(sounds[sound], 0, 1, SOUND_PLAY_BEGINNING);

	}	

	//Shutdown libraries
	soundlib_shutdown();	//Library will destroy used sounds
	joypadlib_shutdown();
}

