#include <libvu0.h>
#include "PS2Defines.h"

#ifndef STRUCTS_H
#define STRUCTS_H



typedef union
{
	uint128	ul128;
	uint		ul64[2];
	uint		ui32[4];
	float		f32[4];
	ps2_vu0_fvector	fvect;
} qword_t;

#endif
