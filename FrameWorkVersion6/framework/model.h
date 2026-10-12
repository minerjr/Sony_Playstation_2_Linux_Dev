/****************************************************************************\
|* PS2 framework definitions
|* =========================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: 	Added ms3d file support and future file 
|*					formats. Added comments.(NEEDED)
|* Comments:	Ok finally figured out the ps2 format used in this file.
|*		The format is not too bad once you figured it out.
|*		Unfortuntly who ever made this orginal version used a
|*		afful file format. The file format(.ms) contained zipped data
|*		and is a real pain to decode and makes a newbie want to cry.
|*		I first started to figure out the format by simply dumping
|*		the data out of memory after the file was loaded and look at
|*		what came out. I started to understand how the data was stored
|*		in the mesh struct.(See structs.h)
|*		First of all i'll explain the mesh struct.
|*
|*		(unsigned int nfaces)
|*		This contains the number of faces in the mesh.
|*
|*		(char *vertex)
|*		This is just a pointer to an array of vertexs,
|*		which are actually qword_t's. Depending of what type of 
|*		model you are loading the amount of vertex's will very.
|*		If you use a triangle stripe the number of vertexs is equal to
|*		the number of faces plus 2. This will take into 
|*		consideration the fact that tristrips start with 3 coordinates
|*		then contiue for each triangle you only need 1 more coordinate.
|*		If you use a triangle then the number of vertexs is equal to
|*		the number of faces times 3. This will take into consideration
|*		the fact that each triangle has 3 distinct coordinates.
|*		Now when puting data into a vertex you need to do a few things
|*		with the vertex. Ill show an example.(Its long, more --->>) 
|*
|*		((qword_t)(mesh[index of mesh].vertex + (index of the vertex you want to update) * VERT_DATA_LEN(then either TRI or STRIPPED))->fvect[index of 0 to 2, x = 0, y = 1, z = 2]
|*
|*		Now i know that is about the ugliest code you have ever seen,
|*		however it is not too bad once you see what is going on.
|*		This took about the longest to figure out and also corrected
|*		alot of data problems i was having. IE crapping up the ps2 :(
|*
|*		Now the first part is the cast of qword_t. What this does is
|*		convert the next 128 bits of data in the vertex array into 
|*		a qword_t so we can get our data out or into the vertex.
|*		However we only read the first 95 bits. The last 32 bits
|*		are actually part of the next vertex. The vertexs contain
|*		3 32bit floats that are alligned << 16. IE a
|*		ps2_vu0_fvector(see libvu0.h and structs.h). 
|*		This is used because the 
|*		data is going to be sent to UV1 to be transformed then
|*		sent to the gs in a gif packet.
|*
|*		Next is the mesh[index of mesh].vertex.
|*		the mesh is actually an array of meshes that make up the 
|*		model. The index is just that, and index of the current mesh
|*		you are working on. And vertex is the pointer to the array of
|*		vertex's. Easy eh.(Yes I am Canadian)
|*
|*		Next is the (Index of the vertex you want) * VERT_DATA_LEN*
|*		If you don't understand pointer math then I suggest look it
|*		up. What this does is step through the vertex memory a certain
|*		amount of bytes at a time. VERT_DATA_LEN_TRI equals 3*4
|*		which is for 3 floats.IE your X, Y, and Z. Now you multiply
|*		this by the number of the vertex you want and you will get the 
|*		address of where that vertex is stored in the array.
|*
|*		Now the last part is the )->fvect[0,1,2]
|*		These are the actual X Y and Z values. 	
|*		X = 0, Y = 1, Z = Z. These will allow you to read and write to
|*		the vertex.
|*
|*		(char *normal)
|*		This workes exactly like char *vertex above. Just repeat the 
|*		processes and it'll work. There is a normal for every vertex.
|*		So they match up 1:1.
|*
|*		(char *st)
|*		Again st works the same for vertex except for one thing.
|*		There are only 2 parts of the qword_t that you use.
|*		Instead of the three like the vertex and normal.
|*		Also ST_DATA_LEN_TRI is equal to 2 * 4 which is for
|*		two floats. IE S and T. 
|*		
|*		(qword_t rgb);
|*		This is the mesh color. The mesh has one color and is applied 
|*		to the all of the vertex's in the mesh. To access the R B G A
|*		values of this qword_t use the rgb.fvect[0 to 3] to get the 
|*		value you want.
|*
|*		As you can see this is not too hard. Unfortuntly, the internal
|*		data format and that of Milkshape files are not compatable.
|*		IE it takes alot of hacking to get it to fit. HAHA :).
|*		I hope that I can add more loaders alot easier now that I know
|*		where you need to put the data. 
|*		MD2 are now also supported.
|*		Also support keyframe animation.
|*		the actuall mesh used by the engine is that of the last frame
|*		plus 1. The data is inturpulated between two keyframes and 
|*		dumped into there. The key frames are created using
|*		buildMD2Animations()
|*		to run the animation simply use 
|*			(modelname).runAnimation(name of the animation)
\****************************************************************************/
#ifndef __model__header__
#define __model__header__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <sys/timeb.h>

#include "ps2gs.h"
#include "gifPacket.h"
#include "vifPacket.h"
#include "vu0.h"

#include "constants.h"
#include "structs.h"
#include "log.h"
#include "Md2File.h"
#include "Ms3dFile.h"

#include "minilzo.h"


#define TRIMODEL_VERSION (1)
#define STRIPPED_VERSION (6)
#define STRIPPED_MAGIC ("STRIPPED")

//Different flags for types of 3d models.
#define MODEL_BAD_FORMAT (0)
#define MODEL_TRIMODEL   (1)
#define MODEL_STRIPPED   (2)

#define VERT_DATA_LEN_STRIP (3*4)
#define NORM_DATA_LEN_STRIP (3*4)
#define ST_DATA_LEN_STRIP   (2*4)
//No idea what that was for. 
//#define VERT_DATA_LEN_TRI   (16)
//Amount of bytes a vertex takes.(think 3 floats)
#define VERT_DATA_LEN_TRI (3*4)
//
#define NORM_DATA_LEN_TRI (3*4)
#define ST_DATA_LEN_TRI   (2*4)

#define VU_VERTS_STRIP (100)
#define VU_VERTS_TRI (100)

#include "framework.h"

struct animation_t
{
	char name[16];
	int currentIndex;
	int startIndex, endIndex;
};

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

		unsigned int		nframes;		// Number of frames. Used for animation;
		unsigned int		frameIndex;		// Index of the current frame
		animation_t		*animations;		// Animation information
		int			currentTime, endTime;	// Timers used for animation
		int			animationIndex;		// Index of the animation to use
		int 				type;			// Type of model

							Model(void);
							Model(char *filename);
							~Model(void);

		void 				initialise(void);
		void 				erase(void);
		void				runAnimation(char *name);
		void				buildMD2Animations(Md2File *TmpMd2File);
		void				getFileList(const char*filename, filelist_t *list);
		void 				loadFile(char *filename);
		void 				loadStripped(char *filename);
		void 				loadMS3D(char *filename);
		void				loadMD2(char *filename);
		void				buildPacket(void);
		void				draw(Framework *f);
		void				dump(void);
	};

#endif

