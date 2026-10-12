#include "perftime.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>


// global variables
__s32 g_perf = -1;
struct perf_dev_info g_pdi;				// records information about the hardware.
struct perf_dev_set_counters g_pdsc;	// used to set/get counters and their events.


// functions used for timing
void poll_counters()		// this updates the value of the 2 counters
{
	//assert(g_perf >= 0);
	ioctl(g_perf, PERF_IOCSETCOUNTERS, &g_pdsc);
}

__u64 read_counter0()		// this gives you the current setting of counter0
{
	//assert(g_perf >= 0);
	return g_pdsc.old_counters->ctr0_value;
}

__u64 read_counter1()		// this gives you the current setting of counter1
{
	//assert(g_perf >= 0);
	return g_pdsc.old_counters->ctr1_value;
}

// functions used for setup of timers
int set_timers(const __u8 event0, const __u8 event1, const __u8 target0, const __u8 target1);

int init_perftime()
{
	///////////////////////////
	// setup timer
	int res;
	assert(g_perf < 0);
	g_perf = open( "/dev/perf_counter", O_RDWR );
	if(g_perf < 0)
	{
		perror("couldn't open /dev/perf_counter\n");
		return 0;
	}
	ioctl(g_perf, PERF_IOCGETINFO, &g_pdi);
	ioctl(g_perf, PERF_IOCSTART, NULL);

	res = set_timers(CCR_CTR0_CPU_CYCLE, CCR_CTR1_DCACHE_MISS, CCR_TARGET_ALL, CCR_TARGET_ALL);
	if(res==0) close_perftime();

	return res;
}

int set_events(const __u8 event0, const __u8 event1)
{
	assert(g_perf >= 0);
	return set_timers(event0, event1, g_pdsc.old_counters->ctr0_target, g_pdsc.old_counters->ctr1_target);
}

int set_targets(const __u8 target0, const __u8 target1)
{
	assert(g_perf >= 0);
	return set_timers(g_pdsc.old_counters->ctr0_event, g_pdsc.old_counters->ctr1_event, target0, target1);
}

int set_timers(const __u8 event0, const __u8 event1, const __u8 target0, const __u8 target1)
{
	static struct perf_dev_counters pdc;			// working mem
	long r;

	assert(g_perf >= 0);
	g_pdsc.old_counters = 0;
	g_pdsc.new_counters = &pdc;

	//   /usr/src/linux-2.2.1_ps2/include/asm-mips/r5900_perf_counter.h
	g_pdsc.new_counters->ctr0_target = target0;
	g_pdsc.new_counters->ctr0_event = event0;
	g_pdsc.new_counters->ctr0_value = 0;
	g_pdsc.new_counters->ctr1_target = target1;
	g_pdsc.new_counters->ctr1_event = event1;
	g_pdsc.new_counters->ctr1_value = 0;
	r = ioctl(g_perf, PERF_IOCSETCOUNTERS, &g_pdsc);

	g_pdsc.old_counters = &pdc;
	g_pdsc.new_counters = 0;

	return (r>=0) ? 1 : 0;
}

// used to close up the device
void close_perftime()
{
	assert(g_perf >= 0);
	close(g_perf);
	g_perf = -1;
}

// information about your processor
int get_clock()			// estimated CPU cycle speed.
{
	assert(g_perf >= 0);
	return g_pdi.arch_info.cpu_clock;
}

int get_prid()				// Processor Revision Id.
{
	assert(g_perf >= 0);
	return g_pdi.arch_info.cpu_prid;
}

