/*
FILE: soundlib.c
DESCRIPTION: 
WRITTEN BY: Derek Nedelman (nedelman@netzero.net)
DATE: September 6, 2002
*/

//Includes---------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include "soundlib.h"

#if SOUND_ENABLE_MP3
	#include <mad.h>
#endif

#if SOUND_ENABLE_OGG
	#include <tremor/ivorbiscodec.h>
#endif


//Defines----------------------------------------------------------------------
#define SOUND_ENABLE_ASM			1	//Set to 0 to disable assembly language, 1 to enable. Enabled is better

typedef int (*SOUND_STREAM_CALLBACK_FUNCTION)(int message, void* ptr, int data, void* user_data);

//For sound_stream_t.type and the 'type' parameter of soundlib_create_sound_from_disk_or_memory_file()
#define SOUND_STREAM_DISK			1
#define SOUND_STREAM_MEMORY			2
#define SOUND_STREAM_CALLBACK		3

//For sound_stream_t.sound_type 
#define SOUND_TYPE_UNDEFINED		0
#define SOUND_TYPE_WAV				1
#define SOUND_TYPE_OGG				2
#define SOUND_TYPE_MP3				3
#define SOUND_TYPE_CALLBACK			4

//For sound_t.source
#define SOUND_SOURCE_BUFFER			1	//Sound comes from a loaded buffer
#define SOUND_SOURCE_STREAM			2	//Sound comes from a streaming source

//Defines the internal sound volume limits
#define SOUND_VOLUME_PRECISION_LOG_2	11
#define SOUND_VOLUME_OUTPUTS_PER_LEVEL	(1 << SOUND_VOLUME_PRECISION_LOG_2)

#define SOUND_MIN_VOLUME			15
#define SOUND_MAX_VOLUME			0
#define SOUND_NUM_VOLUME_LEVELS		(SOUND_MIN_VOLUME + 1)

//Sound device constants
#define SOUND_FRAGMENT_SIZE_LOG_2	12	//log2(size of fragments used by the sound device)
#define SOUND_FRAGMENT_SIZE			(1 << SOUND_FRAGMENT_SIZE_LOG_2)

#define SOUND_NUM_FRAGMENTS			3	//The amount of buffering the sound devices use. 2 = double buffering

#define SOUND_DEVICE_MAX_CALLBACKS	1	//The maximum number of sound device callbacks

#define SOUND_MAX_CALLBACKS			2	//The maximum number of sound callbacks

//Do not ever change these values
#define SOUND_BITS_PER_SAMPLE		16
#define SOUND_SAMPLES_PER_SEC		48000
#define SOUND_NUM_CHANNELS			2


//Macros-----------------------------------------------------------------------

//These do nothing
#define soundlib_lock_sound(sound)
#define soundlib_unlock_sound(sound)

#define soundlib_lock_device(device)
#define soundlib_unlock_device(device)

#define soundlib_lock_listener()
#define soundlib_unlock_listener()


//Structures-------------------------------------------------------------------
typedef struct
{
	void* source;				//Reference to file where sound is stored
	int is_owned_memory_file;	//If 'source' points to a memory file, indicates
								//whether or not this struct owns the memory. If so, close() should free it
	void* user_data;

	int type;					//SOUND_STREAM_DISK for disk file, in which case 'source' points to a FILE*
								//SOUND_STREAM_MEMORY for a memory file, in which case 'source' points to memory
								//SOUND_STREAM_CALLBACK for a callback, in which case 'source' points to a callback function
	int sound_type;				//The type of sound file. SOUND_TYPE_WAV, SOUND_TYPE_OGG or SOUND_TYPE_MP3
	int pos;					//Current absolute offset into file, whether it's a disk or memory file

	int sound_offset;			//Absolute offset into file where start of sound data can be found

	int sound_size;				//Size, in bytes, of the sound. If it's an MP3, OGG, or callback type, sound_size = 0
	int source_size;			//Size, in bytes, of the entire file. If it's a callback type, source_size = 0

	int num_channels;			//1 for mono, 2 for stereo
    int samples_per_sec;		//Samples per second. Should be SOUND_OUTPUT_SAMPLES_PER_SECOND
	int bits_per_sample;		//Bits per sample. Should be SOUND_OUTPUT_BITS_PER_SAMPLE

	#if SOUND_ENABLE_MP3
		struct mad_stream mp3_stream;
		struct mad_frame mp3_frame;
		struct mad_synth mp3_synth;
			
		char* mp3_buffer;
		int mp3_buffer_size;
		int mp3_pcm_pos;
	#endif

	#if SOUND_ENABLE_OGG
		ogg_sync_state   ogg_oy;//Sync and verify incoming physical bitstream
		ogg_stream_state ogg_os;//Take physical pages, weld into a logical stream of packets
		ogg_page         ogg_og;//One Ogg bitstream page.  Vorbis packets are inside
		ogg_packet       ogg_op;//One raw packet of data for decode
  
		vorbis_info      ogg_vi;//Struct that stores all the static vorbis bitstream settings
		vorbis_comment   ogg_vc;//Struct that stores all the bitstream user comments
		vorbis_dsp_state ogg_vd;//Central working state for the packet->PCM decoder
		vorbis_block     ogg_vb;//Local working space for packet->PCM decode
		
		char* ogg_buffer;		//Pointer to a buffer that was allocated by libvorbis
		int ogg_buffer_size;	//Size of 'ogg_buffer'
		ogg_int32_t** ogg_pcm;	//Pointer to PCM sound data
		int ogg_pcm_pos;		//Current offset into 'ogg_pcm'
		int ogg_pcm_length;		//Number of entries in 'ogg_pcm'
	#endif 
}sound_stream_t;

typedef struct
{
	int (*callback)(int sound, int callback_id, char* buffer, int num_bytes, void* user_data);
	void* user_data;
}sound_callback_t;

typedef struct
{
	int index;				//Index of this sound in the global 'g_sounds' array

	int source;				//Any of the SOUND_SOURCE_* constants. Indicates where sound comes from
	
	//Contains the full sound if sound_source = SOUND_SOURCE_BUFFER.
	//If sound_source = SOUND_SOURCE_STREAM holds the data for the current output fragment
	//If this sound is an instance of another sound, 'buffer' is merely a pointer and shouldn't be freed
	char* buffer;
	int buffer_size;		//The amount of data pointed to by 'buffer'
	
	//Used if sound_source = SOUND_SOURCE_STREAM	
	sound_stream_t stream;

	int size;				//Size, in bytes, of the sound if the sound is a fully-loaded WAV sound
							//Otherwise, this isn't used

	//Sound format
	int num_channels;		//1 for mono, 2 for stereo
    int samples_per_sec;	//Samples per second
	int bits_per_sample;	//Bits per sample. 8 or 16 (should be 16 though)

	//Other stuff
	int parent;				//-1 if sound is an original. Otherwise, the index of sound if it's an instance
	
	int is_used;			//0 if this sound isn't being used. 1 if it is
	int is_playing;			//0 if this sound is being played. 1 if it is
	int is_looping;			//0 if this sound, when playing, is to be played once. 1 if it is to be looped

	int is_3d;				//0 if this sound is 3d, 1 if not. This corresponds to 'num_channels'. A mono source indicates a 3d sound, stereo indicates non-3d
	int is_3d_enabled;		//0 if 3d sound processing is disabled on sound, 1 if it is enabled

	int play_samples_per_sec;	//Samples per second to use, if playing. Defaults to 'samples_per_sec'
	int samples_per_sec_acc;	//Accumulator used in resampling the number of samples per second
		
	int play_offset;		//Current play offset into this sound in bytes. May exceed 'buffer_size' if this is a streaming or callback sound, or an instance of those types
	int buffer_offset;		//Current play offset into 'buffer' in bytes. Will always be 0 for a streaming sound or an instance of a streaming sound
	
	int device;				//Index of device to play this sound into

	int left_volume;		//Volume set by user calls to soundlib_set_sound_volume()
	int right_volume;		//Volume set by user calls to soundlib_set_sound_volume()

	//3D variables
	float pos[3];	
	float min_volume, max_volume;
	float min_distance_squared, max_distance_squared;

	sound_callback_t callbacks[SOUND_MAX_CALLBACKS];

	int num_instances;		//Number of instances of this sound
	int* instances;			//Array of indexes of instances of this sound. This array is packed

	int update_stamp;		//Used during updating of sounds to make sure sound isn't updated more than once	
}sound_t;

typedef struct
{
	float pos[3];	
	float forward[3];
	float up[3];
	float right[3];			//Derived from cross_product(forward, up)	
}sound_listener_t;

typedef struct
{
	float pos[3];	
	float world_to_listener[4][4];
}sound_processed_listener_t;

typedef struct
{
	void (*callback)(int callback_id, char* buffer, int num_bytes, void* user_data);
	void* user_data;
}sound_device_callback_t;

typedef struct 
{
	int sound_fd;
	char* buffer;

	sound_device_callback_t callbacks[SOUND_DEVICE_MAX_CALLBACKS];	
}sound_device_t;

typedef struct
{
	unsigned long riff_chunk;			//'RIFF' chunk
	unsigned long riff_chunk_length;	//Size of RIFF chunk
	unsigned long chunk_type;			//'WAVE' chunk

	unsigned long fmt_chunk;			//'fmt ' chunk
	unsigned long fmt_chunk_length;		//Length of format chunk
	unsigned short format;				//1 for PCM-code
	unsigned short num_channels;		//1 for mono, 2 for stereo
	unsigned long samples_per_sec;		//Frequency of sample
	unsigned long bytes_per_sec;
	unsigned short bytes_per_sample;	//Bytes per sample - 1 or 2
	unsigned short bits_per_sample;

	unsigned long data_chunk;			//'data' chunk
	unsigned long data_length;			//samplecount
}wav_header_t;


//Globals----------------------------------------------------------------------

//Sound devices - both devices can be used simultaneously
#define NUM_SOUND_DEVICES 		2	//Do not ever change this value
static sound_device_t g_devices[NUM_SOUND_DEVICES];

//Temporary sound buffers used in the process of generating an output sound
#define NUM_TEMP_SOUND_BUFFERS	2	//Do not ever change this value
static char* g_temp_buffers[NUM_TEMP_SOUND_BUFFERS];

//Volume table used to speed up volume level calculations
static short* g_volume_table;

//Sounds
static int g_max_sounds;
static sound_t* g_sounds;

//Allocation/free function pointers used on the sound buffers in 'g_sounds[].buffer'
static void* (*g_buffer_malloc)(int num_bytes, void* user_data);
static void (*g_buffer_free)(void* bytes, void* user_data);
static void* g_malloc_user_data;

//3D sound listener
static sound_listener_t g_listener;

//Update variable
static int g_update_stamp = 1;


//Prototypes-------------------------------------------------------------------
static void soundlib_update_sound2(int sound, int update_stamp, sound_processed_listener_t* listener);
static void soundlib_update_sound(int sound, int update_stamp, sound_processed_listener_t* listener);
static void soundlib_update_device(int device, int update_stamp, sound_processed_listener_t* listener);
static int soundlib_num_available_fragments(int device);

static int soundlib_open_callback_stream(SOUND_STREAM_CALLBACK_FUNCTION callback, void* user_data, sound_stream_t* stream);
static int soundlib_open_stream(const void* file, int num_bytes, int sound_type, int type, int is_owned_memory_file, int force_into_memory, sound_stream_t* stream);
static void soundlib_close_stream(sound_stream_t* stream);
static int soundlib_read_stream_raw(sound_stream_t* stream, void* data, int num_bytes);
static int soundlib_read_stream(sound_stream_t* stream, void* data, int num_bytes);
static void soundlib_seek_stream(sound_stream_t* stream, int offset_from_sound_start);
static int soundlib_is_eos(sound_stream_t* stream);
static void soundlib_refresh_stream(sound_stream_t* stream);

static int soundlib_determine_file_type(const void* memory);
static int soundlib_create_sound_from_disk_or_memory_file(const void* file, int num_bytes, int sound_type, int type, int is_streaming, int force_into_memory);

static void* soundlib_default_buffer_malloc(int num_bytes, void* user_data);
static void soundlib_default_buffer_free(void* bytes, void* user_data);

static int soundlib_find_first_free_sound(int first_search);
static void soundlib_remove_sound_instance(int sound, int instance);

void soundlib_mix_buffers(void* out, const void* a, const void* b, int num_bytes);
static void soundlib_resample_frequency(char* out, int out_samples_per_sec, int out_max_bytes, int* num_output_bytes,										
										const char* in, int in_samples_per_sec, int in_num_bytes, int* num_input_bytes,
										int* accumulator, int num_channels);
static void soundlib_build_volume_table(void);
static void soundlib_adjust_volume(char* out, const char* in, int num_bytes, int left_volume, int right_volume);
static void soundlib_adjust_volume_mono_to_stereo(char* out, const char* in, int num_bytes, int left_volume, int right_volume);
static void soundlib_3d_processing(char* out, const char* in, int sound, sound_processed_listener_t* listener);
static void soundlib_mono_to_stereo(char* out, const char* in);

static float soundlib_dot_product(const float* a, const float* b);
static void soundlib_cross_product(float* out, const float* a, const float* b);
static void soundlib_inverse_transform(float out[4][4], const float* x, const float* y, const float* z, const float* translation);
static void soundlib_transform(float* out, float mat[4][4], const float* vec);
static float soundlib_vector_squared_length(float* vec);
static float soundlib_vector_length(float* vec);


//Functions--------------------------------------------------------------------

//Inline functions-------------------------------------------------------------
static inline unsigned long FourCC(char ch0, char ch1, char ch2, char ch3)
{
	return (unsigned long)ch0 | ((unsigned long)ch1 << 8) | ((unsigned long)ch2 << 16) | ((unsigned long)ch3 << 24);
}

static inline int soundlib_float_to_int(float f)
{	
#if SOUND_ENABLE_ASM
	int i;
	
	asm 
	(
		"lwc1 $f0, 0x0(%0);	#Load 'f' into f0 register
		 cvt.w.s $f0, $f0;	#Convert f0 register to an int
		 swc1 $f0, 0x0(%1);	#Store f0 register to 'i'"
		 :
		 :"r"(&f), "r"(&i)
		 :"$f0"
	);

	return i;
#else
	return (int)f;	
#endif 
}

static inline float soundlib_int_to_float(int i)
{
#if SOUND_ENABLE_ASM
	float f;
	
	asm 
	(
		"lwc1 $f0, 0x0(%1);	#Load 'i' into f0 register
		 cvt.s.w $f0, $f0;	#Convert f0 register to a float
		 swc1 $f0, 0x0(%0);	#Store f0 register to 'f'"
		 :
		 :"r"(&f), "r"(&i)
		 :"$f0"
	);
		  
	return f;
#else
	return (float)i;
#endif
}

static inline int soundlib_float_to_volume(float vol)
{
	if (vol < 0.0f)
		vol = 0.0f;
	else if (vol > 1.0f)
		vol = 1.0f;

	return soundlib_float_to_int((1.0f - vol) * SOUND_MIN_VOLUME);
}

static inline float soundlib_sqrt(float f)
{
#if SOUND_ENABLE_ASM
	asm 
	(
		"sqrt.s %0, %0;"
		:"=&f"(f)
		:"f"(f)
	);

	return f;
#else
	return (float)sqrt((double)f);
#endif
}


//Initialization/shutdown------------------------------------------------------
int soundlib_initialize(int max_sounds,
						void* (*buffer_malloc)(int num_bytes, void* user_data),
						void (*buffer_free)(void* bytes, void* user_data),
						void* malloc_user_data)
{
	int i;
	int setting;
	const char* device_names[NUM_SOUND_DEVICES] = 
	{
		"/dev/dsp",
		"/dev/dsp1"
	};	

	int output_bytes_per_second;		//Number of bytes to output to a device per second
	unsigned long sound_fragment_time;	//Time length, in microseconds, of a fragment 

	int devices_can_be_updated,
        devices_can_be_updated_next;

	struct itimerval timer;
	
	output_bytes_per_second = ((SOUND_BITS_PER_SAMPLE / 8) * SOUND_SAMPLES_PER_SEC * SOUND_NUM_CHANNELS);
	sound_fragment_time = (unsigned long)(1000000.0 * SOUND_FRAGMENT_SIZE / output_bytes_per_second);
	//At 2 bytes per sample, 48000 samples per sec, stereo sound, and a fragment size of 4096, 
	//the mixer needs to be called at least 47 times per second

	memset(g_devices, 0, sizeof(g_devices));
	
	//Set up devices
	for (i = 0; i < NUM_SOUND_DEVICES; i++)
	{		
		g_devices[i].sound_fd = open(device_names[i], O_WRONLY);

		if (g_devices[i].sound_fd == -1)
			return i;

		//Set fragment size
		setting = (SOUND_NUM_FRAGMENTS << 16) | SOUND_FRAGMENT_SIZE_LOG_2;
		ioctl(g_devices[i].sound_fd, SNDCTL_DSP_SETFRAGMENT, &setting);

		//Set sample size
		setting = SOUND_BITS_PER_SAMPLE;
		ioctl(g_devices[i].sound_fd, SNDCTL_DSP_SAMPLESIZE, &setting);

		//Set number of channels
		setting = SOUND_NUM_CHANNELS - 1;	//Subtract 1 because SNDCTL_DSP_STEREO uses 0 for mono, 1 for stereo
		ioctl(g_devices[i].sound_fd, SNDCTL_DSP_STEREO, &setting);

		//Set frequency
		setting = SOUND_SAMPLES_PER_SEC;
		ioctl(g_devices[i].sound_fd, SNDCTL_DSP_SPEED, &setting);

		//Set master volume
		setting = 32767;
		ioctl(g_devices[i].sound_fd, MIXER_WRITE(SOUND_MIXER_VOLUME), &setting);

		//Mixing volume
		soundlib_set_output_volume(i, 1.0f);

		g_devices[i].buffer = (char*)memalign(16, SOUND_FRAGMENT_SIZE);
	}

	//Set up sounds
	g_max_sounds = max_sounds;
	g_sounds = (sound_t*)malloc(sizeof(sound_t) * g_max_sounds);
	memset(g_sounds, 0, sizeof(sound_t) * max_sounds);
	for (i = 0; i < g_max_sounds; i++)
	{
		g_sounds[i].index = i;		
		
		g_sounds[i].instances = (int*)malloc(sizeof(g_sounds[i].instances[0]) * g_max_sounds);
	}

	//Allocate temporary sound buffers
	for (i = 0; i < NUM_TEMP_SOUND_BUFFERS; i++)
	{
		g_temp_buffers[i] = (char*)memalign(16, SOUND_FRAGMENT_SIZE);
	}
	
	//Build the volume table
	soundlib_build_volume_table();

	//Set up sound memory allocator/deallocator
	if (buffer_malloc)
	{
		g_buffer_malloc = buffer_malloc;
		g_buffer_free = buffer_free;
		g_malloc_user_data = malloc_user_data;
	}
	else
	{
		g_buffer_malloc = soundlib_default_buffer_malloc;
		g_buffer_free = soundlib_default_buffer_free;
		g_malloc_user_data = 0;
	}
	
	//Initialize listener
	memset(&g_listener, 0, sizeof(g_listener));
	
	return 0;
}

void soundlib_shutdown(void)
{
	int i;
	struct itimerval timer;
	
	//Free up the audio devices	
	for (i = 0; i < NUM_SOUND_DEVICES; i++)
	{
		if (g_devices[i].sound_fd != -1)
		{
			close(g_devices[i].sound_fd);	
			g_devices[i].sound_fd = -1;
		
			free(g_devices[i].buffer);
			g_devices[i].buffer = 0;
		}
	}

	//Free the sounds
	if (g_sounds)
	{
		for (i = 0; i < g_max_sounds; i++)
		{
			soundlib_destroy_sound(i);
			
			free(g_sounds[i].instances);
		}
	
		free(g_sounds);
		g_sounds = 0;
		
		g_max_sounds = 0;
	}	

	//Free up the temporary sound buffers
	for (i = 0; i < NUM_TEMP_SOUND_BUFFERS; i++)
	{
		free(g_temp_buffers[i]);
	}

	//Free the volume table
	free(g_volume_table);
}


void soundlib_update(void)
{
	int i;
	sound_processed_listener_t listener;	

	//Compute listener parameters
	soundlib_lock_listener();
	{
		memcpy(listener.pos, g_listener.pos, sizeof(g_listener.pos));
			
		soundlib_inverse_transform(listener.world_to_listener, g_listener.right, g_listener.up, g_listener.forward, g_listener.pos);
	}
	soundlib_unlock_listener();

	//Update devices
	for (i = 0; i < NUM_SOUND_DEVICES; i++)
	{
		while (soundlib_num_available_fragments(i))
			soundlib_update_device(i, g_update_stamp, &listener);
	}
	g_update_stamp++;	
}


//Device functions-------------------------------------------------------------
int soundlib_get_block_size(void)
{
	return SOUND_FRAGMENT_SIZE;
}

int soundlib_get_num_outputs(void)
{
	return NUM_SOUND_DEVICES;
}

float soundlib_get_output_volume_step_size(void)
{
	return 1.0f / 32767.0f;
}

void soundlib_set_output_volume(int device, float volume)
{
	int i_volume = soundlib_float_to_int(volume * 32767.0f);

	int mixer_ioctl = (device == 0) ? SOUND_MIXER_PCM : SOUND_MIXER_ALTPCM;
	ioctl(g_devices[device].sound_fd, MIXER_WRITE(mixer_ioctl), &i_volume);
}

void soundlib_set_output_callback(int device, int callback, void (*callback_function)(int callback_id, char* buffer, int num_bytes, void* user_data), void* user_data)
{
	soundlib_lock_device(device);
	{
		g_devices[device].callbacks[callback].callback = callback_function;
		g_devices[device].callbacks[callback].user_data = user_data;
	}	
	soundlib_unlock_device(device);
}

//Sound sample functions-------------------------------------------------------

//Callbacks--------------------------------------------------------------------
void soundlib_set_sound_callback(int sound, int callback, int (*callback_function)(int sound, int callback_id, char* buffer, int num_bytes, void* user_data), void* user_data)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].callbacks[callback].callback = callback_function;
		g_sounds[sound].callbacks[callback].user_data = user_data;
	}
	soundlib_unlock_sound(sound);
}

//Creation/destruction---------------------------------------------------------
int soundlib_create_sound_from_file(const char* file, int is_streaming, int force_into_memory)
{
	//Open the file to make sure it exists, and make sure it's a supported type
	FILE* the_file;
	wav_header_t wav_header;
	int sound_type;

	the_file = fopen(file, "rb");
	if (!the_file)
		return -1;

	fread(&wav_header, sizeof(wav_header), 1, the_file);
	fclose(the_file);
	sound_type = soundlib_determine_file_type(&wav_header);
	
	if (sound_type == SOUND_TYPE_UNDEFINED)
		return -1;
	
	return soundlib_create_sound_from_disk_or_memory_file(file, 0, sound_type, SOUND_STREAM_DISK, is_streaming, force_into_memory);
}

int soundlib_create_sound_from_memory(const void* memory, int num_bytes, int is_streaming)
{
	int sound = -1;
	int sound_type;

	sound_type = soundlib_determine_file_type(memory);
	switch (sound_type)
	{
		case SOUND_TYPE_WAV:
		{
			sound = soundlib_create_sound_from_disk_or_memory_file(memory, num_bytes, sound_type, SOUND_STREAM_MEMORY, is_streaming, 1);
			break;
		}
		case SOUND_TYPE_MP3:
		case SOUND_TYPE_OGG:
		{
			sound = soundlib_create_sound_from_disk_or_memory_file(memory, num_bytes, sound_type, SOUND_STREAM_MEMORY, 1, 1);
			break;
		}
	}	

	return sound;
}

int soundlib_create_sound_from_callback(int (*callback)(int message, void* ptr, int data, void* user_data), void* user_data, int num_channels)
{
	int new_sound;
	
	new_sound = soundlib_find_first_free_sound(0);
	if (new_sound < 0)
		return -1;
  	
	if (!soundlib_open_callback_stream(callback, user_data, &g_sounds[new_sound].stream))
		return 0;	

	g_sounds[new_sound].source = SOUND_SOURCE_STREAM;

	g_sounds[new_sound].buffer = (char*)g_buffer_malloc(SOUND_FRAGMENT_SIZE, g_malloc_user_data);
	g_sounds[new_sound].buffer_size = SOUND_FRAGMENT_SIZE;		
	
	g_sounds[new_sound].size = 0;
	
	g_sounds[new_sound].bits_per_sample = SOUND_BITS_PER_SAMPLE;
	g_sounds[new_sound].play_samples_per_sec = SOUND_SAMPLES_PER_SEC;
	g_sounds[new_sound].samples_per_sec_acc = 0;
	g_sounds[new_sound].num_channels = num_channels;

	g_sounds[new_sound].parent = -1;
	g_sounds[new_sound].is_playing = 0;
	g_sounds[new_sound].is_looping = 0;
	g_sounds[new_sound].buffer_offset = 0;
	g_sounds[new_sound].play_offset = 0;
	g_sounds[new_sound].left_volume = g_sounds[new_sound].right_volume = 0;
	g_sounds[new_sound].num_instances = 0;	

	g_sounds[new_sound].is_3d = (g_sounds[new_sound].num_channels == 2) ? 0 : 1;
	g_sounds[new_sound].is_3d_enabled = g_sounds[new_sound].is_3d;

	memset(g_sounds[new_sound].callbacks, 0, sizeof(g_sounds[new_sound].callbacks));
	memset(g_sounds[new_sound].pos, 0, sizeof(g_sounds[new_sound].pos));
	g_sounds[new_sound].min_volume = 1.0f;
	g_sounds[new_sound].max_volume = 1.0f;
	g_sounds[new_sound].min_distance_squared = 1.0f;
	g_sounds[new_sound].max_distance_squared = 1.0f;

	soundlib_lock_sound(new_sound);
	{
		g_sounds[new_sound].is_used = 1;
	}
	soundlib_unlock_sound(new_sound);

	return new_sound;
}

int soundlib_create_sound_instance(int sound)
{
	int new_sound = -1;
	int root_sound;
	
	soundlib_lock_sound(sound);
	{
		//Only find an empty sound if the source sound is used
		if (g_sounds[sound].is_used)
			new_sound = soundlib_find_first_free_sound(0);
		
		//This test will fail if either the source sound is unusued or there are no free sounds to use
		if (new_sound < 0)
			goto done;
	
		//Start creating the instantiated sound...

		//Regardless of the original type, the instantiated sound is of a buffer type because its data source is its member buffer
		g_sounds[new_sound].source = SOUND_SOURCE_BUFFER;
		
		//Copy the buffer parameters from the original sound
		g_sounds[new_sound].buffer = g_sounds[sound].buffer;
		g_sounds[new_sound].buffer_size = g_sounds[sound].buffer_size;
		g_sounds[new_sound].size = g_sounds[sound].size;
		g_sounds[new_sound].bits_per_sample = g_sounds[sound].bits_per_sample;
		g_sounds[new_sound].play_samples_per_sec = g_sounds[sound].play_samples_per_sec;
		g_sounds[new_sound].samples_per_sec_acc = g_sounds[sound].samples_per_sec_acc;
		g_sounds[new_sound].samples_per_sec = g_sounds[sound].samples_per_sec;
		g_sounds[new_sound].num_channels = g_sounds[sound].num_channels;

		//Find out the true parent. We don't want to create an instance of an instance
		if (g_sounds[sound].parent >= 0)
			root_sound = g_sounds[sound].parent;
		else
			root_sound = sound;
				
		//Give the new sound and the parent sound references to each other
		g_sounds[new_sound].parent = root_sound;
		g_sounds[root_sound].instances[g_sounds[root_sound].num_instances++] = new_sound;
		
		//Set up some other data
		g_sounds[new_sound].is_playing = 0;
		g_sounds[new_sound].is_looping = g_sounds[sound].is_looping;
		g_sounds[new_sound].buffer_offset = g_sounds[sound].buffer_offset;
		g_sounds[new_sound].play_offset = g_sounds[sound].play_offset;
		g_sounds[new_sound].left_volume = g_sounds[sound].left_volume;
		g_sounds[new_sound].right_volume = g_sounds[sound].right_volume;
		g_sounds[new_sound].num_instances = 0;	

		g_sounds[new_sound].is_3d = (g_sounds[new_sound].num_channels == 2) ? 0 : 1;
		g_sounds[new_sound].is_3d_enabled = g_sounds[new_sound].is_3d;

		memcpy(g_sounds[new_sound].callbacks, g_sounds[sound].callbacks, sizeof(g_sounds[new_sound].callbacks));
		memcpy(g_sounds[new_sound].pos, g_sounds[sound].pos, sizeof(g_sounds[new_sound].pos));

		g_sounds[new_sound].min_volume = g_sounds[sound].min_volume;
		g_sounds[new_sound].max_volume = g_sounds[sound].max_volume;

		g_sounds[new_sound].min_distance_squared = g_sounds[sound].min_distance_squared;
		g_sounds[new_sound].max_distance_squared = g_sounds[sound].max_distance_squared;
	}
	done:;
	soundlib_unlock_sound(sound);
	
	if (new_sound >= 0)
	{
		soundlib_lock_sound(new_sound);
		{
			g_sounds[new_sound].is_used = 1;
		}
		soundlib_unlock_sound(new_sound);
	}


	return new_sound;
}

void soundlib_destroy_sound(int sound)
{
	int i;

	soundlib_lock_sound(sound);
	{
		if (g_sounds[sound].is_used)
		{
			//Destroy all instances of the sound
			for (i = 0; i < g_sounds[sound].num_instances; i++)
			{
				soundlib_destroy_sound(g_sounds[sound].instances[i]);
			}

			if (g_sounds[sound].parent >= 0)
			{
				//Remove the instance from the parent
				soundlib_remove_sound_instance(g_sounds[sound].parent, sound);
			}
			else
			{	
				//Destroy the sound buffer
				if (g_sounds[sound].buffer && g_buffer_free)
				{
					g_buffer_free(g_sounds[sound].buffer, g_malloc_user_data);
				}
			}
			g_sounds[sound].buffer = 0;
			
			//Close the stream if it's being used
			if (g_sounds[sound].source == SOUND_SOURCE_STREAM)
			{
				soundlib_close_stream(&g_sounds[sound].stream);
			}			
			
			g_sounds[sound].is_used = 0;		
		}
	}
	soundlib_unlock_sound(sound);
}

void soundlib_destroy_all_sounds(void)
{
	int i;

	for (i = 0; i < g_max_sounds; i++)
		soundlib_destroy_sound(i);
}

//Playing----------------------------------------------------------------------
void soundlib_play_sound(int sound, int device, int is_looping, int play_offset)
{
	soundlib_lock_sound(sound);
	{
		if (g_sounds[sound].is_used)
		{
			if (g_sounds[sound].parent >= 0)
			{
				//We have a sound instance.
				int parent = g_sounds[sound].parent;
				
				//Set the play offset
				if (g_sounds[parent].source == SOUND_SOURCE_BUFFER)
				{
					//We can only adjust the play offset for a sound instance if the source sound is a buffer sound

					if (play_offset >= 0)
					{
						g_sounds[sound].play_offset = g_sounds[sound].buffer_offset = play_offset;
						g_sounds[sound].samples_per_sec_acc = 0;
					}
				}
				else
				{
					//The source sound is a file or callback type. The play offset of this instance must match it
					g_sounds[sound].play_offset = g_sounds[parent].play_offset;
				}
			}
			else
			{
				//We have a non-instance sound

				switch (g_sounds[sound].source)
				{
					case SOUND_SOURCE_BUFFER:
					{
						if (play_offset >= 0)
						{
							g_sounds[sound].play_offset = g_sounds[sound].buffer_offset = play_offset;
							g_sounds[sound].samples_per_sec_acc = 0;
						}

						break;
					}
					case SOUND_SOURCE_STREAM:
					{	
						if (g_sounds[sound].stream.sound_type == SOUND_TYPE_WAV)
						{
							//It's a WAV stream. We can seek to where the user wants
							if (play_offset >= 0)
							{
								if (play_offset > g_sounds[sound].stream.sound_size)
									play_offset = g_sounds[sound].stream.sound_size;

								soundlib_seek_stream(&g_sounds[sound].stream, play_offset);
								g_sounds[sound].play_offset = play_offset;
							}
						}
						else
						{
							//It's some other kind of stream. Set the stream to the beginning.
							if (play_offset >= 0)
							{
								play_offset = 0;

								if (play_offset != g_sounds[sound].play_offset)
								{
									soundlib_seek_stream(&g_sounds[sound].stream, play_offset);
									g_sounds[sound].play_offset = play_offset;
									soundlib_refresh_stream(&g_sounds[sound].stream);
								}
							}
						}

						break;
					}
				}
			}	

			g_sounds[sound].is_playing = 1;
			g_sounds[sound].is_looping = is_looping;
			g_sounds[sound].device = device;
		}		
	}
	soundlib_unlock_sound(sound);
}

void soundlib_stop_sound(int sound)
{
	soundlib_lock_sound(sound);
	{
		if (g_sounds[sound].is_used)
		{
			g_sounds[sound].is_playing = 0;
			if (g_sounds[sound].callbacks[SOUND_CALLBACK_END].callback)
				g_sounds[sound].callbacks[SOUND_CALLBACK_END].callback(sound, 0, 0, SOUND_CALLBACK_END, g_sounds[sound].callbacks[SOUND_CALLBACK_END].user_data);
		}		
	}
	soundlib_unlock_sound(sound);
}


//Volume functions-------------------------------------------------------------
float soundlib_get_sound_volume_step_size(void)
{
	return 1.0f / SOUND_NUM_VOLUME_LEVELS;
}

void soundlib_set_sound_volume(int sound, float volume)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].left_volume = g_sounds[sound].right_volume = soundlib_float_to_volume(volume);
	}
	soundlib_unlock_sound(sound);
}

void soundlib_set_sound_volumes(int sound, float left_volume, float right_volume)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].left_volume = soundlib_float_to_volume(left_volume);
		g_sounds[sound].right_volume = soundlib_float_to_volume(right_volume);
	}
	soundlib_unlock_sound(sound);
}

void soundlib_set_sound_left_volume(int sound, float volume)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].left_volume = soundlib_float_to_volume(volume);
	}
	soundlib_unlock_sound(sound);
}

void soundlib_set_sound_right_volume(int sound, float volume)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].right_volume = soundlib_float_to_volume(volume);
	}
	soundlib_unlock_sound(sound);
}


//Sample rate functions--------------------------------------------------------
void soundlib_set_sound_frequency(int sound, int samples_per_sec)
{
	//Sets the frequency at which the sound is played. Only sounds that
	//are entirely loaded in memory (or instances of such sounds) will allow
	//the frequency to be modified

	int can_change_rate = 0;

	soundlib_lock_sound(sound);
	{
		if (g_sounds[sound].parent >= 0)
		{
			if (g_sounds[g_sounds[sound].parent].source == SOUND_SOURCE_BUFFER);
				can_change_rate = 1;
		}
		else if (g_sounds[sound].source == SOUND_SOURCE_BUFFER)
		{
			can_change_rate = 1;
		}
		
		if (can_change_rate)
		{
			g_sounds[sound].play_samples_per_sec = samples_per_sec;
			g_sounds[sound].samples_per_sec_acc = 0;
		}
	}
	soundlib_unlock_sound(sound);
}

void soundlib_restore_sound_frequency(int sound)
{
	//Sets the play frequency to be the same as the source format frequency

	soundlib_lock_sound(sound);
	{
		g_sounds[sound].samples_per_sec = g_sounds[sound].play_samples_per_sec;
	}
	soundlib_unlock_sound(sound);
}


//3D sound functions-----------------------------------------------------------
void soundlib_enable_3d_sound(int sound, int enable)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].is_3d_enabled = enable;
	}
	soundlib_unlock_sound(sound);
}

void soundlib_set_3d_sound_position(int sound, const float* pos)
{
	soundlib_lock_sound(sound);
	{
		memcpy(g_sounds[sound].pos, pos, sizeof(g_sounds[sound].pos));
	}
	soundlib_unlock_sound(sound);
}

void soundlib_set_3d_sound_min_max_volume(int sound, float min_volume, float max_volume)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].min_volume = min_volume;
		g_sounds[sound].max_volume = max_volume;
	}
	soundlib_unlock_sound(sound);
}

void soundlib_set_3d_sound_min_max_distance(int sound, float min_distance, float max_distance)
{
	soundlib_lock_sound(sound);
	{
		g_sounds[sound].min_distance_squared = min_distance * min_distance;
		g_sounds[sound].max_distance_squared = max_distance * max_distance;
	}
	soundlib_unlock_sound(sound);
}


//3D listener functions--------------------------------------------------------
void soundlib_set_listener(const float* pos, const float* up, const float* forward)
{
	soundlib_lock_listener();
	{
		memcpy(g_listener.pos, pos, sizeof(g_listener.pos));
		memcpy(g_listener.up, up, sizeof(g_listener.up));
		memcpy(g_listener.forward, forward, sizeof(g_listener.forward));

		soundlib_cross_product(g_listener.right, g_listener.up, g_listener.forward);
	}
	soundlib_unlock_listener();
}

void soundlib_set_listener_position(const float* pos)
{
	soundlib_lock_listener();
	{
		memcpy(g_listener.pos, pos, sizeof(g_listener.pos));
	}
	soundlib_unlock_listener();
}

void soundlib_set_listener_orientation(const float* up, const float* forward)
{
	soundlib_lock_listener();
	{
		memcpy(g_listener.up, up, sizeof(g_listener.up));
		memcpy(g_listener.forward, forward, sizeof(g_listener.forward));

		soundlib_cross_product(g_listener.right, g_listener.forward, g_listener.up);
	}
	soundlib_unlock_listener();
}


//Sound buffer functions-------------------------------------------------------
int soundlib_open_sound_buffer_from_file(soundlib_sound_buffer_t* buffer, const char* file)
{	
	wav_header_t wav_header;

	//Reset the structure
	memset(buffer, 0, sizeof(soundlib_sound_buffer_t));

	//Open the file	
	buffer->file = fopen(file, "rb");
	if (!buffer->file)
		return 0;

	//Read some data from the file and make sure it's a WAV
	fread(&wav_header, sizeof(wav_header), 1, (FILE*)buffer->file);
	if (soundlib_determine_file_type(&wav_header) != SOUND_TYPE_WAV)
	{
		fclose((FILE*)buffer->file);
		buffer->file = 0;
		return 0;
	}

	//It's a WAV file. Get the relevant info
	buffer->num_bytes = wav_header.data_length;

	buffer->num_channels = wav_header.num_channels;
	buffer->samples_per_sec = wav_header.samples_per_sec;
	buffer->bits_per_sample = wav_header.bits_per_sample;

	buffer->file_or_memory = 0;
	
	return 1;
}

int soundlib_open_sound_buffer_from_memory(soundlib_sound_buffer_t* buffer, const void* memory, int num_bytes)
{
	wav_header_t* wav_header;

	//Reset the structure
	memset(buffer, 0, sizeof(soundlib_sound_buffer_t));

	//Make sure it's a WAV
	if (soundlib_determine_file_type(memory) != SOUND_TYPE_WAV)
		return 0;
	
	//It's a WAV file. Get the relevant info
	buffer->file = (void*)memory;
	wav_header = (wav_header_t*)memory;

	buffer->num_bytes = wav_header->data_length;

	buffer->num_channels = wav_header->num_channels;
	buffer->samples_per_sec = wav_header->samples_per_sec;
	buffer->bits_per_sample = wav_header->bits_per_sample;	

	buffer->file_or_memory = 1;

	return 0;
}

void soundlib_close_sound_buffer_file(soundlib_sound_buffer_t* buffer)
{	
	if (buffer->file_or_memory == 0 && buffer->file)
	{
		fclose((FILE*)buffer->file);		
	}

	buffer->file = 0;
}

int soundlib_load_sound_buffer(soundlib_sound_buffer_t* buffer)
{
	if (!buffer->data)
		return 0;
	
	if (buffer->file_or_memory == 0)
	{
		//Disk file
		fread(buffer->data, 1, buffer->num_bytes, (FILE*)buffer->file);
	}
	else
	{
		//Memory
		memcpy(buffer->data, (char*)buffer->file + sizeof(wav_header_t), buffer->num_bytes);
	}

	return 1;
}

int soundlib_get_sound_buffer_bytes(soundlib_sound_buffer_t* buffer, int offset, int num_bytes, void** bytes, void** bytes2)
{
	//Return a pointer to the requested data

	if (offset > buffer->num_bytes)
	{
		//Whoops. Requested data at an offset that doesn't exist
		*bytes = 0;
		if (bytes2)
			*bytes2 = 0;
		return 0;
	}

	if (offset + num_bytes > buffer->num_bytes)
	{		
		//The user is requesting data that goes past the end of the sound buffer

		//Adjust the return value
		num_bytes = buffer->num_bytes - offset;

		//Set the return pointers and we're done
		*bytes = (char*)buffer->data + offset;
		if (bytes2)
			*bytes2 = (char*)buffer;
	}
	else
	{
		//Easy. Just return what they want
		*bytes = (char*)buffer->data + offset;
		if (bytes2)
			*bytes2 = 0;
	}

	return num_bytes;
}

static inline void soundlib_add_clip_8_shorts(short* out, const short* a, const short* b)
{
	//Adds the 8 shorts pointed to by 'a' and 'b' and puts the result in 'out'	

#if SOUND_ENABLE_ASM
	//Does a parallel signed add with saturate on the 8 halfwords (shorts) pointed to by 'a' and 'b'
	asm 
	(
		"lq $5, 0x0(%1);		#Load a into reg 5
		 lq $6, 0x0(%2);		#Load b into reg 6
		 paddsh $7, $5, $6;		#Parallel signed add with saturate
		 sq $7, 0x0(%0)			#Store result in out"
		 :
		 :"r"(out), "r"(a), "r"(b)
		 :"$5", "$6", "$7"
	);
#else	
	int i;
	int temp;

	for (i = 0; i < 8; i++)
	{
		temp = out[i] + in[i];
		if (temp < -32768)
			temp = -32768;
		else if (temp > 32767)
			temp = 32767;

		out[i] = temp;
	}
#endif
}

void soundlib_mix_buffers(void* out, const void* a, const void* b, int num_bytes)
{
	short* out_samples = (short*)out;
	short* a_samples = (short*)a;
	short* b_samples = (short*)b;
	int i;
	int num_quadwords = num_bytes >> 4;

	for (i = 0; i < num_quadwords; i++)
	{
		soundlib_add_clip_8_shorts(out_samples, a_samples, b_samples);
		out_samples += 8;
		a_samples += 8;
		b_samples += 8;
	}
}

//Local functions--------------------------------------------------------------
static void soundlib_update_sound2(int sound, int update_stamp, sound_processed_listener_t* listener)
{
	//Should only be called by soundlib_update_sound()
	if (g_sounds[sound].update_stamp == update_stamp)
		return;

	if (g_sounds[sound].parent >= 0)
	{
		//This sound is an instance. Update the parent
		soundlib_update_sound2(g_sounds[sound].parent, update_stamp, listener);		
	}
	else
	{	
		//We have a non-instance sound in need of updating
		switch (g_sounds[sound].source)
		{
			case SOUND_SOURCE_STREAM:
			{	
				int num_bytes = 0;
				int num_bytes_to_grab;

				if (g_sounds[sound].num_channels == 2)
					num_bytes_to_grab = SOUND_FRAGMENT_SIZE;
				else
					num_bytes_to_grab = SOUND_FRAGMENT_SIZE / 2;

				do
				{
					num_bytes += soundlib_read_stream(&g_sounds[sound].stream, &g_sounds[sound].buffer[num_bytes], num_bytes_to_grab - num_bytes);
					
					g_sounds[sound].play_offset += num_bytes;

					if (soundlib_is_eos(&g_sounds[sound].stream))
					{
						//We reached the end of the sound

						if (g_sounds[sound].is_looping)
						{
							//Start over
							soundlib_seek_stream(&g_sounds[sound].stream, 0);
						}
						else
						{	
							//Insert silence					
							memset(&g_sounds[sound].buffer[num_bytes], 0, num_bytes_to_grab - num_bytes);
							num_bytes = num_bytes_to_grab;
						}
					}
				}while (num_bytes < num_bytes_to_grab);

				break;
			}			
		}
	}

	//Record the update stamp so that the sound won't be erroneously updated
	g_sounds[sound].update_stamp = update_stamp;
}

static void soundlib_update_sound(int sound, int update_stamp, sound_processed_listener_t* listener)
{
	//This should only be called by the soundlib_update_device()

	int num_bytes;
	
	int root_sound;
	int root_sound_source;		

	int current_temp_buffer = 0;

	if (g_sounds[sound].is_playing)
	{
		//If the sound is an instance, we can only use the sound if the parent sound is used
		//We must also lock the parent to prevent it from becoming destroyed while using it
		if (g_sounds[sound].parent >= 0)
		{
			soundlib_lock_sound(g_sounds[sound].parent);
			
			if (!g_sounds[g_sounds[sound].parent].is_used)
			{
				//If we're here, sound is ok, but the parent isn't. This means we can't 
				//play 'sound', so we just bomb out early
				soundlib_unlock_sound(g_sounds[sound].parent);
				goto done;
			}
		}
	}
					
	//We need to know the "root" source of the sound to know how to deal with things later
	if (g_sounds[sound].parent >= 0)
		root_sound = g_sounds[sound].parent;
	else
		root_sound = sound;
	root_sound_source = g_sounds[root_sound].source;

	//Process end-of-sound condition
	if (root_sound_source == SOUND_SOURCE_BUFFER)
	{
		//See if we ran to the end of the buffer
		if (g_sounds[sound].play_offset == g_sounds[sound].size)
		{
			if (g_sounds[sound].is_looping)
			{						
				g_sounds[sound].buffer_offset = 0;
				g_sounds[sound].play_offset = 0;
			}
			else
			{
				g_sounds[sound].is_playing = 0;				
			}
		}
	}
	else
	{
		//See if we got to the end of the stream. No need to see if the sound is looping
		//because soundlib_update_sound2() checks for that condition and resets the stream if needed
		if (soundlib_is_eos(&g_sounds[root_sound].stream))
			g_sounds[sound].is_playing = 0;
	}

	if (g_sounds[sound].is_playing == 0)
	{
		//This will only happen if the sound was just stopped
		if (g_sounds[sound].callbacks[SOUND_CALLBACK_END].callback)
			g_sounds[sound].callbacks[SOUND_CALLBACK_END].callback(sound, 0, 0, SOUND_CALLBACK_END, g_sounds[sound].callbacks[SOUND_CALLBACK_END].user_data);
	}		
	else
	{
		//If the sound is playing, do all the great stuff

		//The sound playing is in progress
		soundlib_update_sound2(sound, update_stamp, listener);

		if (root_sound_source == SOUND_SOURCE_BUFFER)
		{
			//Generate an intermediate, resampled sound before mixing into the output buffer
			int num_output;
			int total_num_output = 0;
			int num_scanned = 0;
			int num_bytes_to_grab;

			if (g_sounds[sound].num_channels == 2)
				num_bytes_to_grab = SOUND_FRAGMENT_SIZE;
			else
				num_bytes_to_grab = SOUND_FRAGMENT_SIZE / 2;
		
			do
			{
				//Figure out how many bytes to sample from the sound buffer
				if (g_sounds[sound].buffer_offset + num_bytes_to_grab > g_sounds[sound].buffer_size)
					num_bytes = g_sounds[sound].buffer_size - g_sounds[sound].buffer_offset;
				else
					num_bytes = num_bytes_to_grab;

				soundlib_resample_frequency(&g_temp_buffers[current_temp_buffer][total_num_output], g_sounds[sound].play_samples_per_sec, num_bytes_to_grab - total_num_output, &num_output,
											&g_sounds[sound].buffer[g_sounds[sound].buffer_offset], g_sounds[sound].samples_per_sec, num_bytes, &num_scanned,
											&g_sounds[sound].samples_per_sec_acc, g_sounds[sound].num_channels);

				total_num_output += num_output;
																
				//Modify the offsets accordingly
				g_sounds[sound].buffer_offset += num_scanned;
				g_sounds[sound].play_offset += num_scanned;
						
				//Handle end of buffer condition
				if (g_sounds[sound].buffer_offset == g_sounds[sound].buffer_size)
				{
					if (g_sounds[sound].is_looping)
					{							
						g_sounds[sound].buffer_offset = 0;
						g_sounds[sound].play_offset = 0;
					}
					else
					{
						//Insert silence in the remaing portion of the temp buffer and break out of loop
						memset(&g_temp_buffers[current_temp_buffer][total_num_output], 0, num_bytes_to_grab - total_num_output);
						total_num_output = num_bytes_to_grab;
					}
				}
			}while (total_num_output < num_bytes_to_grab);

			if (g_sounds[sound].is_3d)
			{
				if (g_sounds[sound].is_3d_enabled)
					soundlib_3d_processing(g_temp_buffers[current_temp_buffer ^ 1], g_temp_buffers[current_temp_buffer], sound, listener);
				else
					soundlib_mono_to_stereo(g_temp_buffers[current_temp_buffer ^ 1], g_temp_buffers[current_temp_buffer]);
				current_temp_buffer ^= 1;
			}

			if (g_sounds[sound].callbacks[SOUND_CALLBACK_EFFECT].callback)
				g_sounds[sound].callbacks[SOUND_CALLBACK_EFFECT].callback(sound, SOUND_CALLBACK_EFFECT, g_temp_buffers[current_temp_buffer], SOUND_FRAGMENT_SIZE, g_sounds[sound].callbacks[SOUND_CALLBACK_EFFECT].user_data);

			if (g_sounds[sound].left_volume | g_sounds[sound].right_volume)
			{
				soundlib_adjust_volume(g_temp_buffers[current_temp_buffer], g_temp_buffers[current_temp_buffer], SOUND_FRAGMENT_SIZE, g_sounds[sound].left_volume, g_sounds[sound].right_volume);
			}	

			soundlib_mix_buffers(g_devices[g_sounds[sound].device].buffer, g_devices[g_sounds[sound].device].buffer, g_temp_buffers[current_temp_buffer], SOUND_FRAGMENT_SIZE);			
		}
		else
		{
			//It's a streaming sound type. 

			if (g_sounds[sound].num_channels == 2)
				memcpy(g_temp_buffers[current_temp_buffer], g_sounds[sound].buffer, SOUND_FRAGMENT_SIZE);
			else
				memcpy(g_temp_buffers[current_temp_buffer], g_sounds[sound].buffer, SOUND_FRAGMENT_SIZE / 2);
			
			if (g_sounds[sound].is_3d)
			{
				if (g_sounds[sound].is_3d_enabled)
					soundlib_3d_processing(g_temp_buffers[current_temp_buffer ^ 1], g_temp_buffers[current_temp_buffer], sound, listener);
				else
					soundlib_mono_to_stereo(g_temp_buffers[current_temp_buffer ^ 1], g_temp_buffers[current_temp_buffer]);
				current_temp_buffer ^= 1;				
			}
			
			if (g_sounds[sound].callbacks[SOUND_CALLBACK_EFFECT].callback)
				g_sounds[sound].callbacks[SOUND_CALLBACK_EFFECT].callback(sound, SOUND_CALLBACK_EFFECT, g_temp_buffers[current_temp_buffer], SOUND_FRAGMENT_SIZE, g_sounds[sound].callbacks[SOUND_CALLBACK_EFFECT].user_data);

			if (g_sounds[sound].left_volume | g_sounds[sound].right_volume)
			{
				soundlib_adjust_volume(g_temp_buffers[current_temp_buffer], g_temp_buffers[current_temp_buffer], SOUND_FRAGMENT_SIZE, g_sounds[sound].left_volume, g_sounds[sound].right_volume);				
			}

			soundlib_mix_buffers(g_devices[g_sounds[sound].device].buffer, g_devices[g_sounds[sound].device].buffer, g_temp_buffers[current_temp_buffer], SOUND_FRAGMENT_SIZE);
		}
	}
		
	if (g_sounds[sound].parent >= 0)
	{
		soundlib_unlock_sound(g_sounds[sound].parent);
	}
	
	done:;	
}

static void soundlib_update_device(int device, int update_stamp, sound_processed_listener_t* listener)
{
	int i;

	memset(g_devices[device].buffer, 0, SOUND_FRAGMENT_SIZE);

	for (i = 0; i < g_max_sounds; i++)
	{
		soundlib_lock_sound(i);
		{
			if (g_sounds[i].is_used && g_sounds[i].is_playing && g_sounds[i].device == device)
			{
				soundlib_update_sound(i, g_update_stamp, listener);
			}
		}
		soundlib_unlock_sound(i);
	}

	soundlib_lock_device(device);	
	{
		if (g_devices[device].callbacks[SOUND_OUTPUT_CALLBACK_EFFECT].callback)
			g_devices[device].callbacks[SOUND_OUTPUT_CALLBACK_EFFECT].callback(SOUND_OUTPUT_CALLBACK_EFFECT, g_devices[device].buffer, SOUND_FRAGMENT_SIZE, g_devices[device].callbacks[SOUND_OUTPUT_CALLBACK_EFFECT].user_data);
	}
	soundlib_unlock_device(device);

	write(g_devices[device].sound_fd, g_devices[device].buffer, SOUND_FRAGMENT_SIZE);	
}

static int soundlib_num_available_fragments(int device)
{
	audio_buf_info fragment_info;

	ioctl(g_devices[device].sound_fd, SNDCTL_DSP_GETOSPACE, &fragment_info);

	return fragment_info.fragments;
}

static int soundlib_open_callback_stream(SOUND_STREAM_CALLBACK_FUNCTION callback, void* user_data, sound_stream_t* stream)
{
	stream->type = SOUND_STREAM_CALLBACK;
	stream->sound_type = SOUND_TYPE_CALLBACK;

	stream->source = callback;
	stream->user_data = user_data;

	callback(SOUND_CALLBACK_MSG_OPEN, 0, 0, stream->user_data);

	return 1;
}

static int soundlib_load_file_into_memory(const char* filename, char** memory, int* num_bytes)
{
	FILE* file;
	
	file = fopen(filename, "rb");
	if (!file)
		return 0;

	fseek(file, 0, SEEK_END);
	*num_bytes = ftell(file);
	fseek(file, 0, SEEK_SET);

	*memory = (char*)g_buffer_malloc(*num_bytes, g_malloc_user_data);
	fread(*memory, 1, *num_bytes, file);

	fclose(file);

	return 1;	
}

static int soundlib_open_stream(const void* file, int num_bytes, int sound_type, int type, int is_owned_memory_file, int force_into_memory, sound_stream_t* stream)
{	
	//Opens the passed in file, whether its a disk or memory based file.
	//Currently, only WAV files are supported
	//If the file doesn't exist or isn't a WAV file, 0 is returned
	//Otherwise, 1 is returned

	wav_header_t wav_header;//We use this structure as a sort of temporary buffer to 
							//read the beginning of the file to figure out its type

	stream->type = type;
	switch (stream->type)
	{
		case SOUND_STREAM_DISK: 
		{
			if (force_into_memory)
			{
				//Load the file into memory and then stream from that
				char* memory_file;

				if (!soundlib_load_file_into_memory((const char*)file, &memory_file, &num_bytes))
					return 0;
				stream->is_owned_memory_file = 1;

				stream->type = SOUND_STREAM_MEMORY;

				if (sound_type == SOUND_TYPE_UNDEFINED)
					sound_type = soundlib_determine_file_type(memory_file);

				switch (sound_type)
				{
					case SOUND_TYPE_WAV:
					{
						memcpy(&wav_header, memory_file, sizeof(wav_header_t));
						stream->source = (void*)memory_file;
						stream->sound_type = SOUND_TYPE_WAV;
						break;
					}		
				#if SOUND_ENABLE_OGG		
					case SOUND_TYPE_OGG:
					{	
						stream->source = (void*)memory_file;
						stream->sound_type = SOUND_TYPE_OGG;
						break;
					}
				#endif
				#if SOUND_ENABLE_MP3
					case SOUND_TYPE_MP3:
					{
						stream->source = (void*)memory_file;
						stream->sound_type = SOUND_TYPE_MP3;
						break;
					}
				#endif
					default:
					{
						g_buffer_free(memory_file, g_malloc_user_data);
						return 0;
					}
				}
			}
			else
			{
				//A stream-from-disk sound
				stream->source = fopen((const char*)file, "rb");
				if (!stream->source)			
					return 0;	
				
				fread(&wav_header, sizeof(wav_header), 1, (FILE*)stream->source);
				fseek((FILE*)stream->source, 0, SEEK_SET);

				if (sound_type == SOUND_TYPE_UNDEFINED)
					sound_type = soundlib_determine_file_type(&wav_header);

				switch (sound_type)
				{
					case SOUND_TYPE_WAV:
					{
						stream->sound_type = SOUND_TYPE_WAV;
						break;
					}		
				#if SOUND_ENABLE_OGG		
					case SOUND_TYPE_OGG:
					{
						stream->sound_type = SOUND_TYPE_OGG;
						break;
					}
				#endif
				#if SOUND_ENABLE_MP3
					case SOUND_TYPE_MP3:
					{
						stream->sound_type = SOUND_TYPE_MP3;
						break;
					}
				#endif
					default:
					{
						fclose((FILE*)stream->source);
						return 0;
					}
				}
			}
			
			break;
		}

		case SOUND_STREAM_MEMORY:
		{
			if (sound_type == SOUND_TYPE_UNDEFINED)
				sound_type = soundlib_determine_file_type(file);

			switch (sound_type)
			{
				case SOUND_TYPE_WAV:
				{
					memcpy(&wav_header, file, sizeof(wav_header_t));
					stream->source = (void*)file;
					stream->sound_type = SOUND_TYPE_WAV;
					break;
				}		
			#if SOUND_ENABLE_OGG		
				case SOUND_TYPE_OGG:
				{	
					stream->source = (void*)file;			
					stream->sound_type = SOUND_TYPE_OGG;
					break;
				}
			#endif
			#if SOUND_ENABLE_MP3
				case SOUND_TYPE_MP3:
				{
					stream->source = (void*)file;
					stream->sound_type = SOUND_TYPE_MP3;
					break;
				}
			#endif
				default:
				{
					return 0;
				}
			}

			stream->is_owned_memory_file = is_owned_memory_file;
			
			break;
		}
	}	

	switch (stream->sound_type)
	{
		case SOUND_TYPE_WAV:
		{
			stream->sound_offset = stream->pos = sizeof(wav_header_t);
			soundlib_seek_stream(stream, 0);

			stream->num_channels = wav_header.num_channels;
			stream->samples_per_sec = wav_header.samples_per_sec;
			stream->bits_per_sample = wav_header.bits_per_sample;

			stream->sound_size = wav_header.data_length;
			stream->source_size = stream->sound_offset + stream->sound_size;

			break;
		}
	#if SOUND_ENABLE_MP3
		case SOUND_TYPE_MP3:
		{			
			mad_stream_init(&stream->mp3_stream);
			mad_frame_init(&stream->mp3_frame);
			mad_synth_init(&stream->mp3_synth);
			stream->mp3_buffer_size = 4096;
			stream->mp3_buffer = (char*)g_buffer_malloc(stream->mp3_buffer_size, g_malloc_user_data);
			stream->mp3_pcm_pos = 0;

			stream->sound_offset = 0;

			switch (stream->type)
			{
				case SOUND_STREAM_DISK: 
				{
					fseek((FILE*)stream->source, 0, SEEK_END);
					stream->source_size = ftell((FILE*)stream->source);	
					break;
				}
				case SOUND_STREAM_MEMORY:
				{
					stream->source_size = num_bytes;					
					break;
				}
			}
			soundlib_seek_stream(stream, 0);

			stream->sound_size = 0;

			soundlib_read_stream(stream, 0, 0);

			stream->num_channels = MAD_NCHANNELS(&stream->mp3_frame.header);
			stream->samples_per_sec = stream->mp3_frame.header.samplerate;
			stream->bits_per_sample = SOUND_BITS_PER_SAMPLE;

			break;
		}
	#endif
	#if SOUND_ENABLE_OGG
		case SOUND_TYPE_OGG:
		{
			int i;
			
			stream->ogg_buffer_size = 4096;
			stream->ogg_pcm_pos = 0;
			stream->ogg_pcm_length = 0;		

			stream->sound_offset = 0;

			switch (stream->type)
			{
				case SOUND_STREAM_DISK: 
				{
					fseek((FILE*)stream->source, 0, SEEK_END);
					stream->source_size = ftell((FILE*)stream->source);	
					break;
				}
				case SOUND_STREAM_MEMORY:
				{
					stream->source_size = num_bytes;
					break;
				}
			}
			soundlib_seek_stream(stream, 0);

			stream->sound_size = 0;

			ogg_sync_init(&stream->ogg_oy);

			//Comments are from the oggvorbis decoding example

			//Grab some data at the head of the stream.  We want the first page
			//(which is guaranteed to be small and only contain the Vorbis
			//stream initial header) We need the first page to get the stream serialno

			//submit a block to libvorbis' Ogg layer
			stream->ogg_buffer = ogg_sync_buffer(&stream->ogg_oy, stream->ogg_buffer_size);
			num_bytes = soundlib_read_stream_raw(stream, stream->ogg_buffer, stream->ogg_buffer_size);
			ogg_sync_wrote(&stream->ogg_oy, num_bytes);
						    
			//Get the first page
			if (ogg_sync_pageout(&stream->ogg_oy, &stream->ogg_og) != 1)
			{
				//Ran out of data or it's an invalid bitstream
				//Ran out of data?
				goto error;
			}
  
			//Get the serial number and set up the rest of decode. serialno first; 
			//use it to set up a logical stream
			ogg_stream_init(&stream->ogg_os, ogg_page_serialno(&stream->ogg_og));
    
			//Extract the initial header from the first page and verify that the
			//Ogg bitstream is in fact Vorbis data
    
			//I handle the initial header first instead of just having the code
			//read all three Vorbis headers at once because reading the initial
			//header is an easy way to identify a Vorbis bitstream and it's
			//useful to see that functionality seperated out.
			vorbis_info_init(&stream->ogg_vi);
			vorbis_comment_init(&stream->ogg_vc);
			if (ogg_stream_pagein(&stream->ogg_os, &stream->ogg_og) < 0)
			{ 
				//error; stream version mismatch perhaps
				//Error reading first page of Ogg bitstream data
				goto error;
			}
    
			if (ogg_stream_packetout(&stream->ogg_os, &stream->ogg_op) != 1)
			{ 
				//no page? must not be vorbis
				//Error reading initial header packet
				goto error;
			}
    
			if (vorbis_synthesis_headerin(&stream->ogg_vi, &stream->ogg_vc, &stream->ogg_op) < 0)
			{ 
				//error case; not a vorbis header
				//This Ogg bitstream does not contain Vorbis audio data.
				goto error;
			}
    
			//At this point, we're sure we're Vorbis.  We've set up the logical
			//(Ogg) bitstream decoder.  Get the comment and codebook headers and
			//set up the Vorbis decoder
    
			//The next two packets in order are the comment and codebook headers.
			//They're likely large and may span multiple pages.  Thus we read
			//and submit data until we get our two pacakets, watching that no
			//pages are missing.  If a page is missing, error out; losing a
			//header page is the only place where missing data is fatal.    
			i = 0;
			while (i < 2)
			{
				while (i < 2)
				{
					int result = ogg_sync_pageout(&stream->ogg_oy, &stream->ogg_og);
					if (result == 0)
						break; //Need more data
				
					//Don't complain about missing or corrupt data yet.  We'll catch it at the packet output phase
					if (result == 1)
					{
						ogg_stream_pagein(&stream->ogg_os, &stream->ogg_og); 

						//We can ignore any errors here as they'll also become apparent at packetout
						while (i < 2)
						{
							result = ogg_stream_packetout(&stream->ogg_os, &stream->ogg_op);
							if (result == 0)
								break;
							if (result < 0)
							{
								//Uh oh; data at some point was corrupted or missing!
								//We can't tolerate that in a header.  Die.
								//Corrupt secondary header.  Exiting
								goto error;
							}
							vorbis_synthesis_headerin(&stream->ogg_vi, &stream->ogg_vc, &stream->ogg_op);
							i++;
						}
					}
				}

				//no harm in not checking before adding more
				stream->ogg_buffer = ogg_sync_buffer(&stream->ogg_oy, stream->ogg_buffer_size);
				num_bytes = soundlib_read_stream_raw(stream, stream->ogg_buffer, stream->ogg_buffer_size);
				if (num_bytes == 0 && i < 2)
				{
					//End of file before finding all Vorbis headers!
					goto error;
				}
				ogg_sync_wrote(&stream->ogg_oy, num_bytes);

				//??? This can't be right but it seems to work ok. 
				//How do we know where the first sound starts?
				if (stream->ogg_oy.fill > stream->sound_offset)
					stream->sound_offset = stream->ogg_oy.fill;
			}
    
			//We finally know something about the bitstream
			stream->num_channels = stream->ogg_vi.channels;
			stream->samples_per_sec = stream->ogg_vi.rate;
			stream->bits_per_sample = SOUND_BITS_PER_SAMPLE;
			
			//OK, got and parsed all three headers. Initialize the Vorbis packet->PCM decoder.
			vorbis_synthesis_init(&stream->ogg_vd, &stream->ogg_vi); //central decode state
			vorbis_block_init(&stream->ogg_vd, &stream->ogg_vb);     //local state for most of the decode so multiple block decodes can proceed in parallel.  We could init multiple vorbis_block structures for vd here

			//These couple of goto's are ugly, but it cleans up the logic above
			goto success;

			error:
			goto end;
			
			success:;
			soundlib_read_stream(stream, 0, 0);

			stream->num_channels = stream->ogg_vi.channels;
			stream->samples_per_sec = stream->ogg_vi.rate;
			stream->bits_per_sample = SOUND_BITS_PER_SAMPLE;

			end:;
			break;
		}
	#endif
	}

	return 1;
}

static void soundlib_close_stream(sound_stream_t* stream)
{
	switch (stream->type)
	{
		case SOUND_STREAM_DISK: 
		{
			if (stream->source)
			{
				fclose((FILE*)stream->source);
				stream->source = 0;
			}
			
			break;
		}
		case SOUND_STREAM_MEMORY:
		{			
			if (stream->is_owned_memory_file && stream->source)
			{
				g_buffer_free(stream->source, g_malloc_user_data);
			}
			stream->source = 0;
			break;
		}
		case SOUND_STREAM_CALLBACK:
		{
			if (stream->source)
			{	
				SOUND_STREAM_CALLBACK_FUNCTION callback = (SOUND_STREAM_CALLBACK_FUNCTION)stream->source;
				callback(SOUND_CALLBACK_MSG_CLOSE, 0, 0, stream->user_data);
				stream->source = 0;
			}
			break;
		}
	}

	switch (stream->sound_type)
	{
		case SOUND_TYPE_WAV:
		{			
			break;
		}
	#if SOUND_ENABLE_MP3
		case SOUND_TYPE_MP3:
		{			
			mad_synth_finish(&stream->mp3_synth);
			mad_frame_finish(&stream->mp3_frame);
			mad_stream_finish(&stream->mp3_stream);
			if (stream->mp3_buffer && g_buffer_free)
			{
				g_buffer_free(stream->mp3_buffer, g_malloc_user_data);
				stream->mp3_buffer = 0;
			}
			break;
		}
	#endif
	#if SOUND_ENABLE_OGG
		case SOUND_TYPE_OGG:
		{
			ogg_stream_clear(&stream->ogg_os);
  
			vorbis_block_clear(&stream->ogg_vb);
			vorbis_dsp_clear(&stream->ogg_vd);
			vorbis_comment_clear(&stream->ogg_vc);
			vorbis_info_clear(&stream->ogg_vi);

			ogg_sync_clear(&stream->ogg_oy);

			break;
		}
	#endif
	}
}

static int soundlib_read_stream_raw(sound_stream_t* stream, void* data, int num_bytes)
{
	switch (stream->type)
	{
		case SOUND_STREAM_DISK: 
		{
			if (stream->pos + num_bytes > stream->source_size)
				num_bytes = stream->source_size - stream->pos;

			fread(data, 1, num_bytes, (FILE*)stream->source);

			break;
		}
		case SOUND_STREAM_MEMORY:
		{
			if (stream->pos + num_bytes > stream->source_size)
				num_bytes = stream->source_size - stream->pos;

			memcpy(data, (char*)stream->source + stream->pos, num_bytes);			
			
			break;
		}
		case SOUND_STREAM_CALLBACK:
		{
			SOUND_STREAM_CALLBACK_FUNCTION callback = (SOUND_STREAM_CALLBACK_FUNCTION)stream->source;
			num_bytes = callback(SOUND_CALLBACK_MSG_READ, data, num_bytes, stream->user_data);
			break;
		}
	}

	stream->pos += num_bytes;

	return num_bytes;
}

static inline int soundlib_clip_sample(int x) 
{
	if (x < -32768)
		x = -32768;
	else if (x > 32767)
		x = 32767;
	return x;
}

#if SOUND_ENABLE_MP3
static inline short soundlib_mad_fixed_to_short(mad_fixed_t fixed)
{
	return soundlib_clip_sample(fixed >> (MAD_F_FRACBITS - 15));
}
#endif

static int soundlib_update_mp3_stream(sound_stream_t* stream)
{
#if SOUND_ENABLE_MP3
	int num_read;					
	int left_over;
					
	stream->mp3_pcm_pos = 0;
			
	if (stream->mp3_stream.next_frame != 0)
	{
		//We have a truncated frame. We need to keep it, so move it to the beginning of the buffer
		left_over = stream->mp3_stream.bufend - stream->mp3_stream.next_frame;
		memcpy(stream->mp3_buffer, stream->mp3_stream.next_frame, left_over);
	}
	else
	{
		left_over = 0;
	}
						
	//Read in some data
	num_read = soundlib_read_stream_raw(stream, &stream->mp3_buffer[left_over], stream->mp3_buffer_size - left_over);
	if (num_read == 0)
		return 0;
					
	//Pipe the new buffer content to libmad's stream decoder facility.
	mad_stream_buffer(&stream->mp3_stream, stream->mp3_buffer, left_over + num_read);
									
	//Decode the frame
	retry_decode:
	if (mad_frame_decode(&stream->mp3_frame, &stream->mp3_stream))
	{	
		if (stream->mp3_stream.error == MAD_ERROR_LOSTSYNC)
		{					
			mad_stream_sync(&stream->mp3_stream);
			goto retry_decode;
		}
		else
		{							
			return 0;
		}
	}

	mad_synth_frame(&stream->mp3_synth, &stream->mp3_frame);

	return num_read;
#else
	return 0;
#endif
}

static int soundlib_update_ogg_stream(sound_stream_t* stream)
{
#if SOUND_ENABLE_OGG
	//We need to get more data from the file
	int num_read;

	stream->ogg_pcm_pos = 0;

	//Read in some bytes
	stream->ogg_buffer = ogg_sync_buffer(&stream->ogg_oy, stream->ogg_buffer_size);
	num_read = soundlib_read_stream_raw(stream, stream->ogg_buffer, stream->ogg_buffer_size);
	ogg_sync_wrote(&stream->ogg_oy, num_read);
	if (num_read == 0)
		return 0;

	if (ogg_sync_pageout(&stream->ogg_oy, &stream->ogg_og) > 0)
	{
		ogg_stream_pagein(&stream->ogg_os, &stream->ogg_og);
		if (ogg_stream_packetout(&stream->ogg_os, &stream->ogg_op) > 0)
		{
			if (vorbis_synthesis(&stream->ogg_vb, &stream->ogg_op) == 0)
				vorbis_synthesis_blockin(&stream->ogg_vd, &stream->ogg_vb);
				
			stream->ogg_pcm_length = vorbis_synthesis_pcmout(&stream->ogg_vd, &stream->ogg_pcm);
		}
	}
	
	return num_read;
#else
	return 0;
#endif
}

static int soundlib_read_stream(sound_stream_t* stream, void* data, int num_bytes)
{	
	switch (stream->sound_type)
	{
		case SOUND_TYPE_WAV:
		{
			num_bytes = soundlib_read_stream_raw(stream, data, num_bytes);
			break;
		}
	#if SOUND_ENABLE_MP3
		case SOUND_TYPE_MP3:
		{
			short* out_samples = (short*)data;
			int current_output = 0;
			int total_num_bytes = 0;
				
			do
			{
				if (num_bytes)
				{
					//Convert madlib samples to output samples
					for (; stream->mp3_pcm_pos < stream->mp3_synth.pcm.length; stream->mp3_pcm_pos++)
					{
						short sample;

						//Left channel
						sample = soundlib_mad_fixed_to_short(stream->mp3_synth.pcm.samples[0][stream->mp3_pcm_pos]);
						out_samples[current_output++] = sample;
						total_num_bytes += 2;
						if (total_num_bytes == num_bytes)
							break;

						//Right channel. If the decoded stream is monophonic then the right output channel is the same as the left one
						if (MAD_NCHANNELS(&stream->mp3_frame.header) > 1)
						{
							sample = soundlib_mad_fixed_to_short(stream->mp3_synth.pcm.samples[1][stream->mp3_pcm_pos]);
							out_samples[current_output++] = sample;
							total_num_bytes += 2;
							if (total_num_bytes == num_bytes)
								break;
						}					
					}
				}

				if (stream->mp3_pcm_pos == stream->mp3_synth.pcm.length)
					stream->mp3_pcm_pos = 0;
					
				//If the input buffer is empty, refill it
				if (stream->mp3_pcm_pos == 0)
				{
					if (soundlib_update_mp3_stream(stream) == 0)
						break;					
				}	
			}while (total_num_bytes < num_bytes);

			//Assign num_bytes here because we may have broken out because of an error or end of file
			num_bytes = total_num_bytes;
			
			break;
		}
	#endif
	#if SOUND_ENABLE_OGG
		case SOUND_TYPE_OGG:
		{
			short* out_samples = (short*)data;
			int current_output = 0;
			int total_num_bytes = 0;
				
			do
			{
				if (num_bytes && stream->ogg_pcm_length > 0)
				{
					//Convert ogg samples to output samples
					do
					{
						for (; stream->ogg_pcm_pos < stream->ogg_pcm_length; stream->ogg_pcm_pos++)
						{
							short sample;

							//Left channel
							sample = (short)soundlib_clip_sample(stream->ogg_pcm[0][stream->ogg_pcm_pos]>>9);
							out_samples[current_output++] = sample;
							total_num_bytes += 2;
							if (total_num_bytes == num_bytes)
								break;

							//Right channel
							if (stream->ogg_vi.channels > 1)
							{
								sample = (short)soundlib_clip_sample(stream->ogg_pcm[1][stream->ogg_pcm_pos]>>9);
								out_samples[current_output++] = sample;
								total_num_bytes += 2;
								if (total_num_bytes == num_bytes)
									break;
							}
						}
						
						if (stream->ogg_pcm_pos == stream->ogg_pcm_length)
						{
							//Reached the end of the current PCM stream. See if we can get more

							stream->ogg_pcm_pos = 0;
							vorbis_synthesis_read(&stream->ogg_vd, stream->ogg_pcm_length);

							if (ogg_stream_packetout(&stream->ogg_os, &stream->ogg_op) > 0)
							{
								//There was another packet in the current page
								if (vorbis_synthesis(&stream->ogg_vb, &stream->ogg_op) == 0)
									vorbis_synthesis_blockin(&stream->ogg_vd, &stream->ogg_vb);

								stream->ogg_pcm_length = vorbis_synthesis_pcmout(&stream->ogg_vd, &stream->ogg_pcm);
							}
							else
								stream->ogg_pcm_length = 0;							
						}
						
						if (total_num_bytes == num_bytes)
							break;

					}while (stream->ogg_pcm_length > 0);
				}

				if (stream->ogg_pcm_length == 0)
				{
					//We need to get more data from the file
					if (soundlib_update_ogg_stream(stream) == 0)
						break;
				}								
			}while (total_num_bytes < num_bytes);

			//Assign num_bytes here because we may have broken out because of an error or end of file
			num_bytes = total_num_bytes;
			
			break;
		}
	#endif
		case SOUND_TYPE_CALLBACK:
		{
			int num_read;
			SOUND_STREAM_CALLBACK_FUNCTION callback = (SOUND_STREAM_CALLBACK_FUNCTION)stream->source;

			num_read = callback(SOUND_CALLBACK_MSG_READ, data, num_bytes, stream->user_data);
			if (num_read < num_bytes)
			{
				memset((char*)data + num_read, 0, num_bytes - num_read);
			}

			break;
		}

		break;
	}

	return num_bytes;
}	

static void soundlib_seek_stream(sound_stream_t* stream, int offset_from_sound_start)
{
	switch (stream->type)
	{
		case SOUND_STREAM_DISK: 
		{
			fseek((FILE*)stream->source, stream->sound_offset + offset_from_sound_start, SEEK_SET);
			break;
		}
		case SOUND_STREAM_MEMORY:
		{			
			break;
		}
		case SOUND_STREAM_CALLBACK:
		{
			SOUND_STREAM_CALLBACK_FUNCTION callback = (SOUND_STREAM_CALLBACK_FUNCTION)stream->source;
			callback(SOUND_CALLBACK_MSG_SEEK, 0, offset_from_sound_start, stream->user_data);
			break;
		}
	}

	stream->pos = stream->sound_offset + offset_from_sound_start;
}

static int soundlib_is_eos(sound_stream_t* stream)
{
	switch (stream->type)
	{
		case SOUND_STREAM_DISK:
		case SOUND_STREAM_MEMORY:
		{
			return stream->pos >= stream->source_size;	
		}
		case SOUND_STREAM_CALLBACK:
		{
			SOUND_STREAM_CALLBACK_FUNCTION callback = (SOUND_STREAM_CALLBACK_FUNCTION)stream->source;
			return callback(SOUND_CALLBACK_MSG_IS_EOS, 0, 0, stream->user_data);
		}
	}

	return 1;
}

static void soundlib_refresh_stream(sound_stream_t* stream)
{
	//Kills whatever sound data is cached and gets new sound data

	//Support function for soundlib_play_sound()

	switch (stream->sound_type)
	{
	#if SOUND_ENABLE_MP3
		case SOUND_TYPE_MP3:
		{
			mad_frame_mute(&stream->mp3_frame);
			mad_synth_mute(&stream->mp3_synth);
			stream->mp3_stream.next_frame = 0;

			soundlib_update_mp3_stream(stream);
			break;
		}
	#endif
	#if SOUND_ENABLE_OGG
		case SOUND_TYPE_OGG:
		{
			vorbis_synthesis_read(&stream->ogg_vd, stream->ogg_pcm_length);
			stream->ogg_pcm_length = 0;
			ogg_sync_reset(&stream->ogg_oy);
			ogg_stream_reset(&stream->ogg_os);			
						
			soundlib_update_ogg_stream(stream);
			break;
		}
	#endif
	}
}
	
static int soundlib_determine_file_type(const void* memory)
{
	//The MP3 case is weird. There's no preliminary header information to indicate
	//an MP3 file so if the library has MP3 support enabled, MP3 is the type
	//we return if all the others fail

	const wav_header_t* wav_header = (const wav_header_t*)memory;
	const char* bytes = (const char*)memory;

	if (wav_header->chunk_type == FourCC('W', 'A', 'V', 'E'))
	{
		return SOUND_TYPE_WAV;
	}
#if SOUND_ENABLE_OGG
	else if (bytes[0] == 'O' && bytes[1] == 'g' && bytes[2] == 'g')
	{
		return SOUND_TYPE_OGG;
	}
#endif
#if SOUND_ENABLE_MP3
	else
	{
		return SOUND_TYPE_MP3;
	}
#else
	else
	{
		return SOUND_TYPE_UNDEFINED;
	}
#endif			
}

static int soundlib_create_sound_from_disk_or_memory_file(const void* file, int num_bytes, int sound_type, int type, int is_streaming, int force_into_memory)
{
	int new_sound;
	sound_stream_t stream;
	sound_stream_t* p_sound_stream;
	void* source_file = (void*)file;
	int is_owned_memory_file = 0;

	new_sound = soundlib_find_first_free_sound(0);
	if (new_sound < 0)
		return -1;

	if (is_streaming || sound_type == SOUND_TYPE_MP3 || sound_type == SOUND_TYPE_OGG)
	{
		is_streaming = 1;

		if (type == SOUND_STREAM_MEMORY)
		{
			source_file = g_buffer_malloc(num_bytes, g_malloc_user_data);
			memcpy(source_file, file, num_bytes);
			is_owned_memory_file = 1;
		}		
		g_sounds[new_sound].source = SOUND_SOURCE_STREAM;
		p_sound_stream = &g_sounds[new_sound].stream;
	}
	else
	{
		g_sounds[new_sound].source = SOUND_SOURCE_BUFFER;
		p_sound_stream = &stream;
	}

	if (!soundlib_open_stream(source_file, num_bytes, sound_type, type, is_owned_memory_file, force_into_memory, p_sound_stream))
		return 0;	

	if (is_streaming || p_sound_stream->sound_type == SOUND_TYPE_MP3 || p_sound_stream->sound_type == SOUND_TYPE_OGG)
	{
		//The sound file should be streamed out of the g_sounds[new_sound].file member
		g_sounds[new_sound].buffer = (char*)g_buffer_malloc(SOUND_FRAGMENT_SIZE, g_malloc_user_data);
		g_sounds[new_sound].buffer_size = SOUND_FRAGMENT_SIZE;		
	}
	else
	{
		//It's a non-streaming file. Load the sound into the g_sounds[new_sound].buffer member
		g_sounds[new_sound].buffer = (char*)g_buffer_malloc(p_sound_stream->sound_size, g_malloc_user_data);
		g_sounds[new_sound].buffer_size = p_sound_stream->sound_size;

		soundlib_read_stream(p_sound_stream, g_sounds[new_sound].buffer, p_sound_stream->sound_size);
		soundlib_close_stream(p_sound_stream);
	}	
	g_sounds[new_sound].size = p_sound_stream->sound_size;
	
	g_sounds[new_sound].bits_per_sample = p_sound_stream->bits_per_sample;
	g_sounds[new_sound].play_samples_per_sec = g_sounds[new_sound].samples_per_sec = p_sound_stream->samples_per_sec;
	g_sounds[new_sound].samples_per_sec_acc = 0;
	g_sounds[new_sound].num_channels = p_sound_stream->num_channels;

	g_sounds[new_sound].parent = -1;
	g_sounds[new_sound].is_playing = 0;
	g_sounds[new_sound].is_looping = 0;
	g_sounds[new_sound].buffer_offset = 0;
	g_sounds[new_sound].play_offset = 0;
	g_sounds[new_sound].left_volume = g_sounds[new_sound].right_volume = 0;
	g_sounds[new_sound].num_instances = 0;	

	g_sounds[new_sound].is_3d = (g_sounds[new_sound].num_channels == 2) ? 0 : 1;
	g_sounds[new_sound].is_3d_enabled = g_sounds[new_sound].is_3d;

	memset(g_sounds[new_sound].callbacks, 0, sizeof(g_sounds[new_sound].callbacks));
	memset(g_sounds[new_sound].pos, 0, sizeof(g_sounds[new_sound].pos));
	g_sounds[new_sound].min_volume = 1.0f;
	g_sounds[new_sound].max_volume = 1.0f;
	g_sounds[new_sound].min_distance_squared = 1.0f;
	g_sounds[new_sound].max_distance_squared = 1.0f;

	soundlib_lock_sound(new_sound);
	{
		g_sounds[new_sound].is_used = 1;
	}
	soundlib_unlock_sound(new_sound);

	return new_sound;
}

static void* soundlib_default_buffer_malloc(int num_bytes, void* user_data)
{
	return (void*)memalign(16, num_bytes);
}

static void soundlib_default_buffer_free(void* bytes, void* user_data)
{
	free(bytes);
}

static int soundlib_find_first_free_sound(int first_search)
{
	int i;

	for (i = first_search; i < g_max_sounds; i++)
	{
		if (g_sounds[i].is_used == 0)
			return i;
	}

	if (first_search > 0)
	{
		for (i = 0; i < first_search; i++)
		{
			if (g_sounds[i].is_used == 0)
				return i;
		}
	}

	return -1;
}

static void soundlib_remove_sound_instance(int sound, int instance)
{
	//Removes 'instance' from the sound's list of instances. Does nothing else
	//Should ONLY be called by soundlib_destroy_sound()
	
	int i;

	for (i = 0; i < g_sounds[sound].num_instances; i++)
	{
		if (g_sounds[sound].instances[i] == instance)
		{
			for (; i < g_sounds[sound].num_instances - 1; i++)
				g_sounds[sound].instances[i] = g_sounds[sound].instances[i + 1];
			g_sounds[sound].num_instances--;
			break;
		}
	}
}

static void soundlib_resample_frequency(char* out, int out_samples_per_sec, int out_max_bytes, int* num_output_bytes,
										const char* in, int in_samples_per_sec, int in_num_bytes, int* num_input_bytes,
										int* accumulator, int num_channels)
{
	if (out_samples_per_sec == in_samples_per_sec)
	{
		//The sample frequency is the same. Just do a copy
		int num_bytes = (out_max_bytes < in_num_bytes) ? out_max_bytes : in_num_bytes;
		memcpy(out, in, num_bytes);
		*num_output_bytes = *num_input_bytes = num_bytes;
	}
	else if (num_channels == 2)
	{
		//The sample frequencies are different. We have to step through and resample

		unsigned long* out_samples = (unsigned long*)out;
		const unsigned long* in_samples = (const unsigned long*)in;
		int num_samples_in_input = in_num_bytes / 4;

		int num_samples_output = 0;		//How many samples actually put out by this function
		int num_samples_input = 0;		//How many samples actually read from 'in'

		while (num_samples_input < num_samples_in_input)
		{
			out_samples[num_samples_output] = in_samples[num_samples_input];
			num_samples_output++;

			out_max_bytes -= 4;
			if (out_max_bytes <= 0)
			{
				num_samples_input++;
				break;
			}
			
			*accumulator += out_samples_per_sec;
			while (*accumulator >= in_samples_per_sec && num_samples_input < num_samples_in_input)
			{
				num_samples_input++;		//Move to next input sample
				*accumulator -= in_samples_per_sec;	//Decrement accumulator
			}
		}
		
		*num_input_bytes = num_samples_input * 4;
		*num_output_bytes = num_samples_output * 4;
	}
	else //if (num_channels == 1)
	{
		//The sample frequencies are different. We have to step through and resample

		unsigned short* out_samples = (unsigned short*)out;
		const unsigned short* in_samples = (const unsigned short*)in;
		int num_samples_in_input = in_num_bytes / 2;

		int num_samples_output = 0;		//How many samples actually put out by this function
		int num_samples_input = 0;		//How many samples actually read from 'in'

		while (num_samples_input < num_samples_in_input)
		{
			out_samples[num_samples_output] = in_samples[num_samples_input];
			num_samples_output++;

			out_max_bytes -= 2;
			if (out_max_bytes <= 0)
			{
				num_samples_input++;
				break;
			}
			
			*accumulator += out_samples_per_sec;
			while (*accumulator >= in_samples_per_sec && num_samples_input < num_samples_in_input)
			{
				num_samples_input++;		//Move to next input sample
				*accumulator -= in_samples_per_sec;	//Decrement accumulator
			}
		}
		
		*num_input_bytes = num_samples_input * 2;
		*num_output_bytes = num_samples_output * 2;
	}
}

static inline int soundlib_adjust_sample_volume(int sample, int vol)
{
	//int row_offset = vol * SOUND_VOLUME_OUTPUTS_PER_LEVEL
	//int column_offset = (sample + 32768) / SOUND_VOLUME_OUTPUTS_PER_LEVEL
	//return g_volume_table[row_offset + column_offset];
	return g_volume_table[(vol << SOUND_VOLUME_PRECISION_LOG_2) + (((sample + (1 << (SOUND_BITS_PER_SAMPLE - 1))) >> (16 - SOUND_VOLUME_PRECISION_LOG_2)) & (SOUND_VOLUME_OUTPUTS_PER_LEVEL - 1))];
}

static void soundlib_adjust_volume(char* out, const char* in, int num_bytes, int left_volume, int right_volume)
{
	short* out_samples = (short*)out;
	const short* in_samples = (const short*)in;
	int num_samples_to_output = num_bytes / 2;
	int i;

	for (i = 0; i < num_samples_to_output; i += 2)
	{
		out_samples[i] = soundlib_adjust_sample_volume(in_samples[i], left_volume);
		out_samples[i + 1] = soundlib_adjust_sample_volume(in_samples[i + 1], right_volume);
	}
}

static void soundlib_adjust_volume_mono_to_stereo(char* out, const char* in, int num_bytes, int left_volume, int right_volume)
{
	short* out_samples = (short*)out;
	const short* in_samples = (const short*)in;
	int num_samples_in_input = num_bytes / 2;
	int in_i, out_i = 0;

	for (in_i = 0; in_i < num_samples_in_input; in_i++)
	{
		out_samples[out_i++] = soundlib_adjust_sample_volume(in_samples[in_i], left_volume);
		out_samples[out_i++] = soundlib_adjust_sample_volume(in_samples[in_i], right_volume);
	}
}

static void soundlib_build_volume_table(void)
{
	int row_offset = 0;

	int volume;
	int output;
	int shift;

	int sample;
	int sample_increment = 65536 / SOUND_VOLUME_OUTPUTS_PER_LEVEL;

	g_volume_table = (short*)malloc(SOUND_NUM_VOLUME_LEVELS * SOUND_VOLUME_OUTPUTS_PER_LEVEL * sizeof(short));

	for (volume = SOUND_MAX_VOLUME; volume <= SOUND_MIN_VOLUME; volume++)
	{
		sample = -32768;

		for (output = 0; output < SOUND_VOLUME_OUTPUTS_PER_LEVEL; output++)
		{
			//The shift is needed because we'll end up with sound data that's off by 255 on the positive side
			if (sample > 0)
				shift = 255;
			else
				shift = 0;

			if (volume == SOUND_MAX_VOLUME)
				g_volume_table[row_offset + output] = sample + shift;
			else				
				g_volume_table[row_offset + output] = (SOUND_MIN_VOLUME - volume) * (sample + shift) / SOUND_NUM_VOLUME_LEVELS;
			
			sample += sample_increment;
		}

		row_offset += SOUND_VOLUME_OUTPUTS_PER_LEVEL;
	}
}

static void soundlib_3d_processing(char* out, const char* in, int sound, sound_processed_listener_t* listener)
{
	static const float x_axis[3] = {1.0f, 0.0f, 0.0f};
	
	float local_pos[3];
	float temp_vector[3];
	
	float distance_squared;
	float volume_level;
	float cos_angle;
	float panning;
	int left_volume, right_volume;

	soundlib_transform(local_pos, listener->world_to_listener, g_sounds[sound].pos);
		
	distance_squared = soundlib_vector_squared_length(local_pos);

	//Determine volume level based on distance
	if (distance_squared > g_sounds[sound].max_distance_squared)
		volume_level = 0.0f;
	else if (distance_squared < g_sounds[sound].min_distance_squared)
		volume_level = 1.0f;
	else
		volume_level = 1.0f - (distance_squared - g_sounds[sound].min_distance_squared)/(g_sounds[sound].max_distance_squared - g_sounds[sound].min_distance_squared);

	volume_level = (g_sounds[sound].max_volume * volume_level) + (g_sounds[sound].min_volume * (1.0f - volume_level));

	//Determine panning
	temp_vector[0] = local_pos[0];
	temp_vector[1] = 0.0f;
	temp_vector[2] = local_pos[2];
	cos_angle = soundlib_dot_product(temp_vector, x_axis) / soundlib_vector_length(temp_vector);
	panning = (1.0f + cos_angle) * 0.5f;

	//Finally, determine volume levels
	left_volume = soundlib_float_to_volume((1.0f - panning) * volume_level);
	right_volume = soundlib_float_to_volume(panning * volume_level);

	soundlib_adjust_volume_mono_to_stereo(out, in, SOUND_FRAGMENT_SIZE / 2, left_volume, right_volume);	
}

static void soundlib_mono_to_stereo(char* out, const char* in)
{
	short* out_samples = (short*)out;
	const short* in_samples = (const short*)in;
	int num_samples_in_input = SOUND_FRAGMENT_SIZE / 2;
	int in_i, out_i = 0;

	for (in_i = 0; in_i < num_samples_in_input; in_i++)
	{
		out_samples[out_i++] = in_samples[in_i];
		out_samples[out_i++] = in_samples[in_i];
	}
}

static void soundlib_cross_product(float* out, const float* a, const float* b)
{
	out[0] = a[1] * b[2] - a[2] * b[1];
	out[1] = a[2] * b[0] - a[0] * b[2];
	out[2] = a[0] * b[1] - a[1] * b[0];
}

static float soundlib_dot_product(const float* a, const float* b)
{
	return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

static void soundlib_inverse_transform(float out[4][4], const float* x, const float* y, const float* z, const float* translation)
{	
	out[0][0] = x[0]; out[0][1] = x[1]; out[0][2] = x[2]; out[0][3] = -soundlib_dot_product(x, translation);
	out[1][0] = y[0]; out[1][1] = y[1]; out[1][2] = y[2]; out[1][3] = -soundlib_dot_product(y, translation);
	out[2][0] = z[2]; out[2][1] = z[1]; out[2][2] = z[2]; out[2][3] = -soundlib_dot_product(z, translation);
	out[3][0] = 0.0f; out[3][1] = 0.0f; out[3][2] = 0.0f; out[3][3] = 1.0f;
}

static void soundlib_transform(float* out, float mat[4][4], const float* vec)
{
	out[0] = (vec[0] * mat[0][0]) + (vec[1] * mat[0][1]) + (vec[2] * mat[0][2]) + (mat[0][3]);
	out[1] = (vec[0] * mat[1][0]) + (vec[1] * mat[1][1]) + (vec[2] * mat[1][2]) + (mat[1][3]);
	out[2] = (vec[0] * mat[2][0]) + (vec[1] * mat[2][1]) + (vec[2] * mat[2][2]) + (mat[2][3]);
}

static float soundlib_vector_squared_length(float* vec)
{
	return (vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]);
}

static float soundlib_vector_length(float* vec)
{
	return soundlib_sqrt((vec[0] * vec[0]) + (vec[1] * vec[1]) + (vec[2] * vec[2]));
}
