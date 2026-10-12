/****************************************************************************\
|* VU0 operations
|* ==============
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SCEE
|* First created			: 
|* Modification history		: Updated by David Froy for ps2engine
|*
\****************************************************************************/

#ifndef __vu0__header__
#define __vu0__header__

#include <stdio.h>
#include <stdlib.h>

#include <ps2gs.h>
#include <libvu0.h>

// prototypes

void ps2_vu0_rot_trans_pers_n_clip_col(__u128 *v0, ps2_vu0_fmatrix m0, ps2_vu0_fvector *v1, ps2_vu0_fvector *nv, 
		ps2_vu0_fvector *st, ps2_vu0_fvector *c, ps2_vu0_fmatrix ll, ps2_vu0_fmatrix lc, int n);
void ps2_vu0_rot_camera_matrix(ps2_vu0_fmatrix m, ps2_vu0_fvector p, ps2_vu0_fvector zd, 
		ps2_vu0_fvector yd, ps2_vu0_fvector rot);

void ps2_vu0_scale_matrix(ps2_vu0_fmatrix outputMatrix, ps2_vu0_fmatrix inputMatrix, ps2_vu0_fvector scaleVector);

#endif // ! __vu0__header__
