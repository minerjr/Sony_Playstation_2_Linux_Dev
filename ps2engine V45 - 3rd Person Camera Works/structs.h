#include <libvu0.h>

#ifndef STRUCTS_H
#define STRUCTS_H

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef int int128 __attribute__ ((mode(TI), aligned(16)));
typedef unsigned int uint128 __attribute__ ((mode(TI), aligned(16)));

typedef union
{
	uint128		ul128;
	uint32		ul64[2];
	uint32		ui32[4];
	float		f32[4];
	ps2_vu0_fvector	fvect;
} qword_t;

#endif
