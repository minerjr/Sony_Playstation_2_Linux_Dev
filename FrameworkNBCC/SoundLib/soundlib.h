/*
FILE: soundlib.h
DESCRIPTION: 
WRITTEN BY: Derek Nedelman (nedelman@netzero.net)
DATE: September 6, 2002
*/

#ifndef SOUNDLIB_INCLUDED
#define SOUNDLIB_INCLUDED


//Defines----------------------------------------------------------------------
#define SOUND_ENABLE_MP3				1		//Set to 0 to disable mp3 file support. Set to 1 to enable.
#define SOUND_ENABLE_OGG				0		//Set to 0 to disable ogg file support. Set to 1 to enable.

//For the 'play_offset' parameter in soundlib_play_sound() 
#define SOUND_PLAY_BEGINNING			0
#define SOUND_PLAY_CONTINUE				-1

//Sound callback IDs
#define SOUND_CALLBACK_END				0
#define SOUND_CALLBACK_EFFECT			1

//Sound output callback IDs
#define SOUND_OUTPUT_CALLBACK_EFFECT	0

//Messages passed to callback sounds created with soundlib_create_sound_from_callback()
#define SOUND_CALLBACK_MSG_OPEN			1
#define SOUND_CALLBACK_MSG_CLOSE		2
#define SOUND_CALLBACK_MSG_READ			3
#define SOUND_CALLBACK_MSG_SEEK			4
#define SOUND_CALLBACK_MSG_IS_EOS		5


//Structures-------------------------------------------------------------------
typedef struct
{
	void* file;					//Don't touch
	int file_or_memory;			//Don't touch

	char* data;					//The sound data
	int num_bytes;				//The number of bytes in 'data'

	int num_channels;			//1 for mono, 2 for stereo
    int samples_per_sec;		//Samples per second
	int bits_per_sample;		//Bits per sample
}soundlib_sound_buffer_t;


//Prototypes-------------------------------------------------------------------

//Initialization/shutdown
int soundlib_initialize(int max_sounds,
						void* (*buffer_malloc)(int num_bytes, void* user_data),
						void (*buffer_free)(void* bytes, void* user_data),
						void* malloc_user_data);
void soundlib_shutdown(void);

void soundlib_update(void);

//Device functions
int soundlib_get_block_size(void);
int soundlib_get_num_outputs(void);
float soundlib_get_output_volume_step_size(void);
void soundlib_set_output_volume(int device, float volume);

void soundlib_set_output_callback(int device, int callback, void (*callback_function)(int callback_id, char* buffer, int num_bytes, void* user_data), void* user_data);

//Sound functions

//Creation/destruction functions
int soundlib_create_sound_from_file(const char* file, int is_streaming, int force_into_memory);
int soundlib_create_sound_from_memory(const void* memory, int num_bytes, int is_streaming);
int soundlib_create_sound_from_callback(int (*callback)(int message, void* ptr, int data, void* user_data), void* user_data, int num_channels);
int soundlib_create_sound_instance(int sound);
void soundlib_destroy_sound(int sound);
void soundlib_destroy_all_sounds(void);

//Play functions
void soundlib_play_sound(int sound, int device, int is_looping, int play_offset);
void soundlib_stop_sound(int sound);

//Volume functions
float soundlib_get_sound_volume_step_size(void);
void soundlib_set_sound_volume(int sound, float volume);
void soundlib_set_sound_volumes(int sound, float left_volume, float right_volume);
void soundlib_set_sound_left_volume(int sound, float volume);
void soundlib_set_sound_right_volume(int sound, float volume);

//Sample rate functions
void soundlib_set_sound_frequency(int sound, int samples_per_sec);
void soundlib_restore_sound_frequency(int sound);

//3D sound functions
void soundlib_enable_3d_sound(int sound, int enable);
void soundlib_set_3d_sound_position(int sound, const float* pos);
void soundlib_set_3d_sound_min_max_volume(int sound, float min_volume, float max_volume);
void soundlib_set_3d_sound_min_max_distance(int sound, float min_distance, float max_distance);

//Callbacks
void soundlib_set_sound_callback(int sound, int callback, int (*callback_function)(int sound, int callback_id, char* buffer, int num_bytes, void* user_data), void* user_data);


//3D listener functions
void soundlib_set_listener(const float* pos, const float* up, const float* forward);
void soundlib_set_listener_position(const float* pos);
void soundlib_set_listener_orientation(const float* up, const float* forward);


//Sound buffer functions
int soundlib_open_sound_buffer_from_file(soundlib_sound_buffer_t* buffer, const char* file);
int soundlib_open_sound_buffer_from_memory(soundlib_sound_buffer_t* buffer, const void* memory, int num_bytes);
void soundlib_close_sound_buffer_file(soundlib_sound_buffer_t* buffer);
int soundlib_load_sound_buffer(soundlib_sound_buffer_t* buffer);
int soundlib_get_sound_buffer_bytes(soundlib_sound_buffer_t* buffer, int offset, int num_bytes, void** bytes, void** bytes2);

//Mixing functions
void soundlib_mix_buffers(void* out, const void* a, const void* b, int num_bytes);

#endif