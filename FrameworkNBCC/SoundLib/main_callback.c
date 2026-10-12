#include "soundlib.h"
#include "joypadlib.h"
#include <malloc.h>
#include <stdio.h>

typedef struct
{
	int num_words;
	soundlib_sound_buffer_t* words;

	int current_word;
	int sound_offset;	
}phrase_t;

int phrase_callback(int message, void* ptr, int data, void* user_data)
{
	phrase_t* phrase = (phrase_t*)user_data;

	switch (message)
	{
		case SOUND_CALLBACK_MSG_READ: 
		{
			//'data' indicates the number of bytes to read

			void* bytes;
			int total_num_bytes = 0,
				num_bytes;
			
			//Get a pointer to the sound data and update the various offsets and quantites we have
			num_bytes = soundlib_get_sound_buffer_bytes(&phrase->words[phrase->current_word], phrase->sound_offset, data, &bytes, 0);
			total_num_bytes += num_bytes;
			phrase->sound_offset += num_bytes;
			data -= num_bytes;
			
			//Copy the sound data to the output
			memcpy(ptr, bytes, num_bytes);
			ptr = (char*)ptr + num_bytes;

			if (num_bytes < data)
			{
				//We reached the end of the current word. Go to the next one.
				phrase->current_word++;
				phrase->sound_offset = 0;

				//We may have finished, so make sure we don't try to access something that doesn't exist
				if (phrase->current_word < phrase->num_words)
				{
					//Get a pointer to the sound data and update the various offsets and quantites we have
					num_bytes = soundlib_get_sound_buffer_bytes(&phrase->words[phrase->current_word], phrase->sound_offset, data, &bytes, 0);
					total_num_bytes += num_bytes;
					phrase->sound_offset += num_bytes;

					//Copy the sound data to the output
					memcpy(ptr, bytes, num_bytes);
				}
			}
						
			return total_num_bytes;
		}
		case SOUND_CALLBACK_MSG_IS_EOS: 
		{
			return phrase->current_word == phrase->num_words;
		}
	}

	return 0;
}

int main(void)
{
	int done = 0;				//Flag to exit the main loop

	int active_joypad = 0;		//Which joypad to get input from
	int buttons;				//Button presses for the joypad

	const int num_sounds = 1;	//Maximum number of sounds for demo to use
	
	int sound;					//Looping variable and ID of the created sound
	const int num_sound_files = 5;	//Number of different sound files to load from
	const char* sound_files[] = {"1.wav", "2.wav", "3.wav", "4.wav", "5.wav"};

	phrase_t phrase;			//The phrase that will be pieced together from the sound files

	//Print out message about program
	printf("This demo shows how to implement a callback-type sound.\n");
	printf("Press START on joypad %d to exit.\n", num_sounds, active_joypad);

	//Initialize libraries
	soundlib_initialize(num_sounds, 0, 0, 0);
	joypadlib_initialize();

	//Initialize the data for the phrase
	phrase.current_word = 0;
	phrase.sound_offset = 0;
	phrase.num_words = num_sound_files;
	phrase.words = (soundlib_sound_buffer_t*)malloc(sizeof(soundlib_sound_buffer_t) * phrase.num_words);
	for (sound = 0; sound < num_sound_files; sound++)
	{
		soundlib_open_sound_buffer_from_file(&phrase.words[sound], sound_files[sound]);
		phrase.words[sound].data = (char*)malloc(phrase.words[sound].num_bytes);
		soundlib_load_sound_buffer(&phrase.words[sound]);
		soundlib_close_sound_buffer_file(&phrase.words[sound]);
	}	

	//Create the sound
	sound = soundlib_create_sound_from_callback(phrase_callback, &phrase, 2);
	soundlib_play_sound(sound, 0, 1, SOUND_PLAY_BEGINNING);

	while (!done)
	{
		joypadlib_update();
		soundlib_update();

		buttons = joypadlib_get_joypad_buttons(active_joypad, 0, PRESSURE_TYPE_NONE);

		if (buttons & PS2PAD_BUTTON_START) 
			done = 1;
	}	

	//We need to destroy the sound first so that we can safely free up the phrase data
	soundlib_destroy_sound(sound);

	//Now free up the sound data for the words
	for (sound = 0; sound < phrase.num_words; sound++)
	{
		free(phrase.words[sound].data);
	}	
	free(phrase.words);	

	//Shutdown libraries
	soundlib_shutdown();	//Library will destroy used sounds
	joypadlib_shutdown();
}
