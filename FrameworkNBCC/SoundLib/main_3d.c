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

	const char* sound_file = "outofhere-mono.wav";

	float listener_pos[3] = {0, 0, -10};
	float listener_up[3] = {0, 1, 0};
	float listener_forward[3] = {0, 0, 1};

	float sound_pos[3] = {0, 0, 0};

	//Print out message about program
	printf("Use the left stick on joypad %d to move around to hear the 3D sound.\n", active_joypad);
	printf("Press START on joypad %d to exit.\n", active_joypad);	

	//Initialize libraries
	soundlib_initialize(1, 0, 0, 0);
	joypadlib_initialize();	

	soundlib_set_listener(listener_pos, listener_up, listener_forward);

	//Create and start playing the sound
	sound = soundlib_create_sound_from_file(sound_file, 0, 0);
	soundlib_set_3d_sound_position(sound, sound_pos);
	soundlib_set_3d_sound_min_max_volume(sound, .1f, 1.0f);
	soundlib_set_3d_sound_min_max_distance(sound, 10, 100.0f);
	soundlib_play_sound(sound, 0, 1, SOUND_PLAY_BEGINNING);
	
	while (!done)
	{
		joypadlib_update();
		soundlib_update();

		buttons = joypadlib_get_joypad_buttons(active_joypad, 0, PRESSURE_TYPE_NONE);
		joypadlib_get_joypad_sticks(active_joypad, left_stick, right_stick);

		if (buttons & PS2PAD_BUTTON_START) 
			done = 1;

		listener_pos[0] += left_stick[0] / 10;
		listener_pos[2] += -left_stick[1] / 10;
		if (listener_pos[0] < sound_pos[0])
		{
			//Sound is to the right of the listener

			if (listener_pos[2] < sound_pos[2])
			{
				//Sound is in front of listener				
				printf("Sound is to the right, in front of you.\n");
			}
			else if (listener_pos[2] > sound_pos[2])
			{
				//Sound is in back of the listener
				printf("Sound is to the right, in back of you.\n");
			}
			else
			{
				//Sound is directly to right
				printf("Sound is directly to the right of you.\n");
			}
		}
		else if (listener_pos[0] > sound_pos[0])
		{
			//Sound is to the left of the listener

			if (listener_pos[2] < sound_pos[2])
			{
				//Sound is in front of listener				
				printf("Sound is to the left, in front of you.\n");
			}
			else if (listener_pos[2] > sound_pos[2])
			{
				//Sound is in back of the listener
				printf("Sound is to the left, in back of you.\n");
			}
			else
			{
				//Sound is directly to left
				printf("Sound is directly to the left of you.\n");
			}
		}
		else
		{
			//Sound is either in front, back, or on the listener
			if (listener_pos[2] < sound_pos[2])
			{
				//Sound is in front of listener				
				printf("Sound is directly in front of you.\n");
			}
			else if (listener_pos[2] > sound_pos[2])
			{
				//Sound is in back of the listener
				printf("Sound is directly in back of you.\n");
			}
			else
			{
				//Sound is directly to left
				printf("Sound is on you.\n");
			}
		}
		soundlib_set_listener_position(listener_pos);
	}

	//Shutdown libraries
	soundlib_shutdown();	//Library will destroy used sounds
	joypadlib_shutdown();
}
