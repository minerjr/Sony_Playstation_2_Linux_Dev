/*
FILE: joypadlib.c
DESCRIPTION: The joypad library
WRITTEN BY: Derek Nedelman (nedelman@netzero.net)
DATE: June 19, 2002
HOME PAGE: http://www.playstation2-linux.com/projects/joypadlib 
*/


//Includes---------------------------------------------------------------------
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/ps2/pad.h>
#include <memory.h>

#include "joypadlib.h"


//Structures-------------------------------------------------------------------
typedef struct
{
	int fd;				//File descriptor
	int old_mode_offset;		//Offset of old mode (mode before app was started)
	struct ps2pad_stat status;	//Joypad status

	unsigned long buttons;		//Digital pad and buttons
	int left_stick[2];		//Left analog stick x/y
	int right_stick[2];		//Right analog stick x/y

	unsigned char button_pressures[PS2PAD_NUM_PRESSURE_BUTTONS];
}joypad_t;


//Globals----------------------------------------------------------------------
static int g_joy_status_fd = -1;

#define MAX_JOYPADS 2			//Maximum number of joypads
static joypad_t g_joypads[MAX_JOYPADS];	//Joypads


//Prototypes for local functions-----------------------------------------------
void joypadlib_enter_joypad_pressure_mode(int joypad);
void joypadlib_exit_joypad_pressure_mode(int joypad);

int joypadlib_get_joypad_num_actuator_lists(int joypad);
int joypadlib_get_joypad_num_actuators_in_list(int joypad, int list);
int joypadlib_get_joypad_actuator_in_list(int joypad, int list, int index_in_list);

int joypadlib_get_joypad_mode_offset(int joypad);
void joypadlib_set_joypad_mode_offset(int joypad, int offs, int lock_unlock);

int joypadlib_wait_for_finish(int joypad);
void joypadlib_wait_for_not_busy(int joypad);


//Functions--------------------------------------------------------------------
void joypadlib_initialize(void)
{
	int i;
	const char* device_names[MAX_JOYPADS] = 
	{
		"/dev/ps2pad00",
		"/dev/ps2pad10"
	};	

	//Open up the port status file
	g_joy_status_fd = open("/dev/ps2padstat", O_RDONLY | O_NONBLOCK);
	
	//Open the joypads
	memset(g_joypads, 0, sizeof(g_joypads));
	for (i = 0; i < MAX_JOYPADS; i++)
	{
		g_joypads[i].fd = open(device_names[i], O_RDONLY | O_NONBLOCK);				
		joypadlib_wait_for_not_busy(i);

		g_joypads[i].old_mode_offset = joypadlib_get_joypad_mode_offset(i);
		joypadlib_wait_for_not_busy(i);

		joypadlib_set_joypad_mode(i, MODE_ANALOG, MODE_LOCK);
		joypadlib_wait_for_finish(i);
		joypadlib_wait_for_not_busy(i);

		joypadlib_enable_joypad_actuators(i, 1, 1);
		joypadlib_wait_for_finish(i);
		joypadlib_wait_for_not_busy(i);

		joypadlib_enter_joypad_pressure_mode(i);
		joypadlib_wait_for_finish(i);		
		joypadlib_wait_for_not_busy(i);
	}

	//Do one update
	joypadlib_update();
}

void joypadlib_shutdown(void)
{
	int i;

	for (i = 0; i < MAX_JOYPADS; i++)
	{		
		joypadlib_exit_joypad_pressure_mode(i);
		joypadlib_wait_for_finish(i);
		joypadlib_wait_for_not_busy(i);

		joypadlib_set_joypad_actuators(i, 0, 0);
		joypadlib_wait_for_finish(i);
		joypadlib_wait_for_not_busy(i);		

		joypadlib_set_joypad_mode_offset(i, g_joypads[i].old_mode_offset, MODE_UNLOCK);
		joypadlib_wait_for_not_busy(i);
		
		if (g_joypads[i].fd >= 0)
		{
			close(g_joypads[i].fd);
			g_joypads[i].fd = -1;
		}
	}	

	if (g_joy_status_fd >= 0)
	{
		close(g_joy_status_fd);
		g_joy_status_fd = -1;
	}	
}

int joypadlib_num_supported_joypads(void)
{
	int total_joypads = -1;
	ioctl(g_joy_status_fd, PS2PAD_IOCGETNPADS, &total_joypads);
	return total_joypads;	
}

void joypadlib_update(void)
{	
	int i;
	struct ps2pad_stat pad_status[MAX_JOYPADS];
	unsigned char joypad_bytes[PS2PAD_DATASIZE];
	
	//Read all the joypad statuses
	read(g_joy_status_fd, pad_status, sizeof(pad_status));

	for (i = 0; i < MAX_JOYPADS; i++)
	{
		//Copy in the status previously read
		memcpy(&g_joypads[i].status, &pad_status[i], sizeof(pad_status[0]));
		
		//Read the joypad bytes that contain the joypad state
		memset(&joypad_bytes, 0, sizeof(joypad_bytes));
		read(g_joypads[i].fd, joypad_bytes, sizeof(joypad_bytes));
		joypadlib_wait_for_finish(i);
		
		//Extract the data from the bytes
		g_joypads[i].buttons = ~(((unsigned long)joypad_bytes[0] << 24) | ((unsigned long)joypad_bytes[1] << 16) | ((unsigned long)joypad_bytes[2] << 8) | (joypad_bytes[3] << 0));		
		if (joypadlib_get_joypad_type(i) != PS2PAD_TYPE_DIGITAL)
		{
			g_joypads[i].right_stick[0] = joypad_bytes[4];
			g_joypads[i].right_stick[1] = joypad_bytes[5];
			g_joypads[i].left_stick[0] = joypad_bytes[6];
			g_joypads[i].left_stick[1] = joypad_bytes[7];
		}
		else
		{
			g_joypads[i].right_stick[0] = 128;
			g_joypads[i].right_stick[1] = 128;
			g_joypads[i].left_stick[0] = 128;
			g_joypads[i].left_stick[1] = 128;
		}

		g_joypads[i].button_pressures[PS2PAD_PRESSURE_DIGITAL_RIGHT] = joypad_bytes[8];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_DIGITAL_LEFT] = joypad_bytes[9];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_DIGITAL_UP] = joypad_bytes[10];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_DIGITAL_DOWN] = joypad_bytes[11];

		g_joypads[i].button_pressures[PS2PAD_PRESSURE_TRIANGLE] = joypad_bytes[12];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_CIRCLE] = joypad_bytes[13];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_CROSS] = joypad_bytes[14];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_SQUARE] = joypad_bytes[15];

		g_joypads[i].button_pressures[PS2PAD_PRESSURE_L1] = joypad_bytes[16];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_R1] = joypad_bytes[17];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_L2] = joypad_bytes[18];
		g_joypads[i].button_pressures[PS2PAD_PRESSURE_R2] = joypad_bytes[19];		
	}	
}

unsigned long joypadlib_get_joypad_buttons(int joypad, void* button_pressures, int type)
{
	if (button_pressures != 0)
	{
		switch (type)
		{
			case PRESSURE_TYPE_BYTE:
			{
				memcpy(button_pressures, g_joypads[joypad].button_pressures, PS2PAD_NUM_PRESSURE_BUTTONS);
				break;
			}
			case PRESSURE_TYPE_FLOAT:
			{
				int i;
				float* float_button_pressures = (float*)button_pressures;
				for (i = 0; i < PS2PAD_NUM_PRESSURE_BUTTONS; i++)
					float_button_pressures[i] = ((float)g_joypads[joypad].button_pressures[i])/255.0f;
				
				break;
			}
		}		
	}

	return g_joypads[joypad].buttons;
}

void joypadlib_get_joypad_sticks(int joypad, float left_stick[2], float right_stick[2])
{
	const int dead_range = 30;
	const int dead_min = 127 - dead_range,
                  dead_max = 128 + dead_range;

	memset(left_stick, 0, sizeof(float) * 2);
	memset(right_stick, 0, sizeof(float) * 2);

	if (g_joypads[joypad].left_stick[0] < dead_min)
	{
		left_stick[0] = (g_joypads[joypad].left_stick[0] - 127) / 127.0f;
	}
	else if (g_joypads[joypad].left_stick[0] > dead_max)
	{
		left_stick[0] = (g_joypads[joypad].left_stick[0] - 127) / 128.0f;
	}

	if (g_joypads[joypad].left_stick[1] < dead_min)
	{
		left_stick[1] = (g_joypads[joypad].left_stick[1] - 127) / 127.0f;
	}
	else if (g_joypads[joypad].left_stick[1] > dead_max)
	{
		left_stick[1] = (g_joypads[joypad].left_stick[1] - 127) / 128.0f;
	}

	if (g_joypads[joypad].right_stick[0] < dead_min)
	{
		right_stick[0] = (g_joypads[joypad].right_stick[0] - 127) / 127.0f;
	}
	else if (g_joypads[joypad].right_stick[0] > dead_max)
	{
		right_stick[0] = (g_joypads[joypad].right_stick[0] - 127) / 128.0f;
	}

	if (g_joypads[joypad].right_stick[1] < dead_min)
	{
		right_stick[1] = (g_joypads[joypad].right_stick[1] - 127) / 127.0f;
	}
	else if (g_joypads[joypad].right_stick[1] > dead_max)
	{
		right_stick[1] = (g_joypads[joypad].right_stick[1] - 127) / 128.0f;
	}
}

int joypadlib_joypad_supports_pressure(int joypad)
{
	int supports_pressure = 0;
	
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCPRESSMODEINFO, &supports_pressure);
	if (supports_pressure != 1)
		supports_pressure = 0;
	
	return supports_pressure;
}

void joypadlib_enter_joypad_pressure_mode(int joypad)
{
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCENTERPRESSMODE, 0);	
}

void joypadlib_exit_joypad_pressure_mode(int joypad)
{
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCEXITPRESSMODE, 0);
}

int joypadlib_get_joypad_status(int joypad)
{
	int status;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCGETSTAT, &status);
	return status;
}

int joypadlib_get_joypad_type(int joypad)
{
	return PS2PAD_TYPE(g_joypads[joypad].status.type);
}

int joypadlib_get_joypad_num_modes(int joypad)
{
	struct ps2pad_modeinfo info;

	info.term = PS2PAD_MODETABLE;
	info.offs = -1;
	info.result = 0;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCMODEINFO, &info);

	return info.result;
}

int joypadlib_get_joypad_mode(int joypad, int mode)
{
	struct ps2pad_modeinfo info;

	info.term = PS2PAD_MODETABLE;
	info.offs = mode;
	info.result = 0;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCMODEINFO, &info);

	return info.result;
}

int joypadlib_get_joypad_mode_offset(int joypad)
{
	struct ps2pad_modeinfo info;

	info.term = PS2PAD_MODECUROFFS;
	info.result = 0;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCMODEINFO, &info);

	return info.result;
}

void joypadlib_set_joypad_mode_offset(int joypad, int offs, int lock_unlock)
{
	struct ps2pad_mode pad_mode;

	pad_mode.offs = offs;

	switch (lock_unlock)
	{
		case MODE_UNLOCK: pad_mode.lock = 2; break;
		case MODE_LOCK: pad_mode.lock = 3; break;
	}	

	ioctl(g_joypads[joypad].fd, PS2PAD_IOCSETMODE, &pad_mode);
}

int joypadlib_set_joypad_mode(int joypad, int mode, int lock_unlock)
{
	int i;

	int num_modes = joypadlib_get_joypad_num_modes(joypad);
	
	for (i = 0; i < num_modes; i++)
	{
		if (joypadlib_get_joypad_mode(joypad, i) == mode)
		{
			joypadlib_set_joypad_mode_offset(joypad, i, lock_unlock);
			
			return 1;
		}
	}

	return 0;
}

int joypadlib_get_joypad_num_actuators(int joypad)
{
	struct ps2pad_actinfo act_info;

	act_info.actno = -1;
	act_info.term = 0;
	act_info.result = 0;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCACTINFO, &act_info);

	return act_info.result;
}

int joypadlib_get_joypad_num_actuator_lists(int joypad)
{
	struct ps2pad_combinfo comb_info;

	comb_info.listno = -1;
	comb_info.offs = 0;
	comb_info.result = 0;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCACTINFO, &comb_info);

	return comb_info.result;
}

int joypadlib_get_joypad_num_actuators_in_list(int joypad, int list)
{
	struct ps2pad_combinfo comb_info;

	comb_info.listno = list;
	comb_info.offs = -1;
	comb_info.result = 0;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCACTINFO, &comb_info);

	return comb_info.result;
}

int joypadlib_get_joypad_actuator_in_list(int joypad, int list, int index_in_list)
{
	struct ps2pad_combinfo comb_info;

	comb_info.listno = list;
	comb_info.offs = index_in_list;
	comb_info.result = -1;
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCACTINFO, &comb_info);

	return comb_info.result;
}

void joypadlib_enable_joypad_actuators(int joypad, int enable_small, int enable_big)
{
	struct ps2pad_act act;
	
	act.len = 6;
	memset(act.data, -1, sizeof(act.data));

	if (enable_small && enable_big)
	{
		act.data[0] = 0;
		act.data[1] = 1;
	}
	else if (enable_small)
	{
		act.data[0] = 0;
	}
	else if (enable_big)
	{
		act.data[1] = 0;
	}

	ioctl(g_joypads[joypad].fd, PS2PAD_IOCSETACTALIGN, &act);
}

void joypadlib_set_joypad_actuators(int joypad, unsigned char small_intensity, unsigned char big_intensity)
{
	struct ps2pad_act act;
	
	act.len = 6;
	memset(act.data, -1, sizeof(act.data));

	if (small_intensity > 1)
		small_intensity = 1;

	act.data[0] = small_intensity;	//Small shaker
	act.data[1] = big_intensity;	//Big shaker
	
	ioctl(g_joypads[joypad].fd, PS2PAD_IOCSETACT, &act);
}

int joypadlib_wait_for_finish(int joypad)
{
	int status;

	do
	{
		status = PS2PAD_RSTAT_BUSY;
		ioctl(g_joypads[joypad].fd, PS2PAD_IOCGETREQSTAT, &status);			
	}while (status == PS2PAD_RSTAT_BUSY);

	return status;
}

void joypadlib_wait_for_not_busy(int joypad)
{
	while (joypadlib_get_joypad_status(joypad) == PS2PAD_STAT_BUSY)
	{
	}
}

