/****************************************************************************\
|* PS2 framework definitions
|* =========================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Sat Jul 20 09:24:52 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/
#ifndef __structs__header__
#define __structs__header__

#include "constants.h"
#include "gifPacket.h"
#include "vifPacket.h"

typedef union
	{
	u_long128     ul128;
	u_long        ul64[2];
	u_int         ui32[4];
	float         f32[4];
	ps2_vu0_fvector fvect;
	} qword_t;

typedef struct
	{
	VifPacket vp;
	u_long128 *buffer;
	unsigned int buffer_size;
	} vifpkt_t;


typedef struct
	{
	qword_t rgb;
	unsigned int tex_index;
	unsigned int nfaces;
	int texture;
	char *vertex;
	char *normal;
	char *st;
	} mesh_t;


#define _SET_VECTOR(_p,_x,_y,_z,_w)  (_p[0] = _x, _p[1] = _y, _p[2] = _z, _p[3] = _w)
#define MULQWORD(a, b, c) ( \
		((qword_t*)&a)->f32[0]=((qword_t*)&b)->f32[0]*((qword_t*)&c)->f32[0], \
		((qword_t*)&a)->f32[1]=((qword_t*)&b)->f32[1]*((qword_t*)&c)->f32[1], \
		((qword_t*)&a)->f32[2]=((qword_t*)&b)->f32[2]*((qword_t*)&c)->f32[2], \
		((qword_t*)&a)->f32[3]=((qword_t*)&b)->f32[3]*((qword_t*)&c)->f32[3])
#define ADDQWORD(a, b, c) ( \
		((qword_t*)&a)->f32[0]=((qword_t*)&b)->f32[0]+((qword_t*)&c)->f32[0], \
		((qword_t*)&a)->f32[1]=((qword_t*)&b)->f32[1]+((qword_t*)&c)->f32[1], \
		((qword_t*)&a)->f32[2]=((qword_t*)&b)->f32[2]+((qword_t*)&c)->f32[2], \
		((qword_t*)&a)->f32[3]=((qword_t*)&b)->f32[3]+((qword_t*)&c)->f32[3])
#define CPYQWORD(a, b) ( \
		((qword_t*)&a)->f32[0]=((qword_t*)&b)->f32[0], \
		((qword_t*)&a)->f32[1]=((qword_t*)&b)->f32[1], \
		((qword_t*)&a)->f32[2]=((qword_t*)&b)->f32[2], \
		((qword_t*)&a)->f32[3]=((qword_t*)&b)->f32[3])
#define NORM_VECTOR(a) { \
		float l= \
			((qword_t*)&a)->f32[0]*((qword_t*)&a)->f32[0]+ \
			((qword_t*)&a)->f32[1]*((qword_t*)&a)->f32[1]+ \
			((qword_t*)&a)->f32[2]*((qword_t*)&a)->f32[2]; \
		if (l==0.0f) l=1.0f; \
		l=1.0f/l; \
		((qword_t*)&a)->f32[0]*=l; \
		((qword_t*)&a)->f32[0]*=l; \
		((qword_t*)&a)->f32[0]*=l; \
	}

class Model;
typedef struct
	{
	unsigned int nfaces;
	unsigned int nmodels;
	Model *models;
	} scene_t;

typedef struct
	{
	int name_count, index;
	int allocated;
	char **names;
	ps2_vu0_fvector *rots;
	} filelist_t;

#endif // ! __structs__header__
