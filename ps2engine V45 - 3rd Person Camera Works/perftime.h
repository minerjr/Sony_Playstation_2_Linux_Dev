#ifndef __PERFTIME_H__
#define __PERFTIME_H__

#ifdef __cplusplus
extern "C" {
#endif
	
// A list of valid events may be found in the following header
// /usr/src/linux-2.2.1_ps2/include/asm-mips/r5900_perf_counter.h
// You will not have to include it though, only perftime.h
#include <asm/types.h>
#include <sys/ioctl.h>
#include <linux/perf_dev.h>

// default events are CCR_CTR0_CPU_CYCLE and CCR_CTR1_DCACHE_MISS
int init_perftime();
int set_events(const __u8 event0, const __u8 event1);
int set_targets(const __u8 target0, const __u8 target1);
void close_perftime();

void poll_counters();	// this updates the value of the 2 counters
__u64 read_counter0();	// this gives you the current setting of counter0
__u64 read_counter1();	// this gives you the current setting of counter1

// information about your processor
int get_clock();			// estimated CPU cycle speed.
int get_prid();			// Processor Revision Id.


#ifdef __cplusplus
}
#endif

#endif
