#ifndef STRUCTS_H
#define STRUCTS_H

#include <libvu0.h>

typedef union
{
	u_long128	ul128;
	u_long		ul64[2];
	u_int		ui32[4];
	float		f32[4];
	ps2_VU0_fvector	fvect;
} qword_t;


#endif
