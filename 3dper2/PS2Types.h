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


// Struct for a vertex
typedef struct
{
	FVector Vertex;				// Untransformed position
	FVector TVertex;			// Transformed position
	float s, t;					// Untransformed Texture Coordinates
	u_char r, g, b, a;			// colour and alpha
	int ADC;					// Clipping bit
	float S, T, Q;				// Transformed Texture Coordinates
}Vertex_t;

// Struct for a Sprite
typedef struct 
{
	FMatrix World;				// world positional matrix
	FVector Pos;				// position on screen (center)
	Vertex_t Vertex[4];			// The vertices
	float RotX;					// Rotation angle in radians round x axis
	float RotY;					// Rotation angle in radians round y axis
	float RotZ;					// Rotation angle in radians round z axis
	float Scale;				// Scaling factor for image
} PS2Sprite_t;

#endif

