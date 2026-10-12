#ifndef _PS2TYPES_H_
#define _PS2TYPES_H_

// A structure for a simple sprite with animation
// and __very__ simple movement
typedef struct 
{
	int x, y;					// position on screen (top left)
	int z;						// z depth (big = near)
	int w, h;					// width and height
	int u, v;					// offset from texture top left
	unsigned char r, g, b, a;	// colour and alpha
	int dx;						// direction for x
	int dy;						// direction for y
	int FCountMax;				// Count to this value before changing frame
	int FCount;					// Frame Counter
	int NFrames;				// number of animation frames
} PS2Sprite_t;


#endif

