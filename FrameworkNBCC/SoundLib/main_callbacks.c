#include "soundlib.h"
#include "joypadlib.h"

int end_and_effect_handler(int sound, int callback_id, char* buffer, int num_bytes, void* user_data)
{
	switch (callback_id)
	{
		case SOUND_CALLBACK_END:
		{
			printf("The end of %s was reached.\n", user_data);
			break;
		}
		case SOUND_CALLBACK_EFFECT:
		{	
			//As a dumb "effect" we'll mute the left channel
			int i;
			int num_samples = num_bytes / 2;
			short* samples = (short*)buffer;
			for (i = 0; i < num_samples; i += 2)
			{
				samples[i] = 0;
			}
		
			break;
		}
	}

	return 0;
}

int main(void)
{
	int done = 0;				//Flag to exit the main loop

	int active_joypad = 0;		//Which joypad to get input from
	int buttons;				//Button presses for the joypad
		
	int sound;					//ID of created sound

	const char* sound_file = "leftright-stereo.wav";

	//Print out message about program
	printf("This demo shows how to use the callback mechanism on sounds.\n");
	printf("Press START on joypad %d to exit.\n", active_joypad);	

	//Initialize libraries
	soundlib_initialize(1, 0, 0, 0);
	joypadlib_initialize();	

	//Create and start playing the sound
	sound = soundlib_create_sound_from_file(sound_file, 0, 0);
	soundlib_set_sound_callback(sound, SOUND_CALLBACK_END, end_and_effect_handler, (void*)sound_file);
	soundlib_set_sound_callback(sound, SOUND_CALLBACK_EFFECT, end_and_effect_handler, 0);
	soundlib_play_sound(sound, 0, 0, SOUND_PLAY_BEGINNING);
		
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