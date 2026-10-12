#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include "constants.h"
#include "structs.h"
#include "log.h"

typedef char BYTE;

typedef struct MS3D_HEADER_TYP
{
	char 	id[10];
	int	version;


}MS3D_HEADER, *MS3D_HEADER_PTR;

typedef struct MS3D_VERTEX_TYP
{
	unsigned char flags;
	unsigned char refCount;
	char boneID;
	float vertex[3];
} MS3D_VERTEX, *MS3D_VERTEX_PTR;

typedef struct MS3D_TRIANGLE_TYP
{
	unsigned short flags;
	unsigned short vertexIndices[3];
	float vertexNormals [3][3];
	float u[3];
	float v[3];
	unsigned char smoothingGroup;
	unsigned char groupIndex;
} MS3D_TRIANGLE, *MS3D_TRIANGLE_PTR;

typedef struct MS3D_GROUP_TYP
{
	unsigned char flags;
	char name[32];
	unsigned short numTriangles;
	unsigned short *triangleIndices;
	char materialIndex;
} MS3D_GROUP, *MS3D_GROUP_PTR;

class Ms3dFile
{

	public:
		unsigned short 		numVertices;
		MS3D_VERTEX*	vertices;
		unsigned short		numTriangles;
		MS3D_TRIANGLE*	triangles;
		unsigned short 		numGroups;
		MS3D_GROUP*	groups;
		GLuint *		Maintexture;

	bool Load(char *filename );
	//void Render(void);
	bool Load(char *filename, mesh_t *TmpMesh);
	

	Ms3dFile( GLuint *Tmptexture)
	{
		Maintexture = Tmptexture;
	}

	~Ms3dFile()
	{
		if(vertices)
			delete vertices;
		if(triangles)
			delete triangles;
		if(groups)
			delete groups;

	}



};

