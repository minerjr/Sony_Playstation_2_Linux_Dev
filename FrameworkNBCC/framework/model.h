/****************************************************************************\
|* PS2 framework definitions
|* =========================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/
#ifndef __model__header__
#define __model__header__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "ps2gs.h"
#include "gifPacket.h"
#include "vifPacket.h"
#include "vu0.h"

#include "constants.h"
#include "structs.h"
#include "log.h"

#include "Ms3dFile.h"

#include "minilzo.h"

#define TRIMODEL_VERSION (1)
#define STRIPPED_VERSION (6)
#define STRIPPED_MAGIC ("STRIPPED")

#define MODEL_BAD_FORMAT (0)
#define MODEL_TRIMODEL   (1)
#define MODEL_STRIPPED   (2)

#define VERT_DATA_LEN_STRIP (3*4)
#define NORM_DATA_LEN_STRIP (3*4)
#define ST_DATA_LEN_STRIP   (2*4)
//#define VERT_DATA_LEN_TRI   (16)
#define VERT_DATA_LEN_TRI (3*4)
#define NORM_DATA_LEN_TRI (3*4)
#define ST_DATA_LEN_TRI   (2*4)

#define VU_VERTS_STRIP (100)
#define VU_VERTS_TRI (100)

#include "framework.h"

class Model
	{
	private:
	
	protected:
		mesh_t *			mesh;
	
	public:
		ps2_vu0_fvector 	objTrans;		// Object translation
		ps2_vu0_fvector 	objRot;			// Object rotation
		ps2_vu0_fvector 	objRot0;		// Object base rotation

		vifpkt_t 			vpkt;

		u_int *				localScreen;	// 
		u_int *				localLight;		// 
		u_int *				lightColour;	// Colour of the illumination
		u_int *				localClip;		//
		u_int *				outlineFactor;	//
		unsigned int 		nfaces;			// Number of faces in the model
		unsigned int 		nmeshes;		// Number of meshes in the model
		int 				type;			// Type of model

							Model(void);
							Model(char *filename);
							~Model(void);

		void 				initialise(void);
		void 				erase(void);
		
		void				getFileList(const char*filename, filelist_t *list);
		void 				loadFile(char *filename);
		void 				loadStripped(char *filename);
		void				buildPacket(void);
		void				draw(Framework *f);
	};

#endif

