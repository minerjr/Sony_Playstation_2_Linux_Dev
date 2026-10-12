#ifndef _PS2TYPES_H_
#define _PS2TYPES_H_

#include "vecmaths.h"

#define XAXIS (0)
#define YAXIS (1)
#define ZAXIS (2)


typedef struct
{
	Dn_CHCR_t chcr;				// channel control register 
	Dn_MADR_t madr;				// memory address register
} DmacControl_t;

// This needs better organisation - particularly the vertex information
typedef struct 
{
	FVector Pos;				// position on screen (center)
	FVector TR, BR, TL, BL;		// untransformed coordinates
	int TRC, TLC, BRC, BLC;		// Clipping flag for each vertex (ADC)
	FMatrix World;				// world positional matrix
	int w, h;					// width and height
	int u, v;					// offset from texture top left
	u_char r, g, b, a;			// colour and alpha
	int FCountMax;				// Count to this value for frame change
	int FCount;					// Frame Counter
	int NFrames;				// number of animation frames
	float RotX;					// Rotation angle in radians round x axis
	float RotY;					// Rotation angle in radians round y axis
	float RotZ;					// Rotation angle in radians round z axis
} PS2Sprite_t;


#endif

