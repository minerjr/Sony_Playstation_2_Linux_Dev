#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <ctype.h>
#include <malloc.h>

typedef char BYTE;

//Head of the Ms3d file.
typedef struct MS3D_HEADER_TYP
{
	char 	id[10];
	int	version;


}MS3D_HEADER, *MS3D_HEADER_PTR;

//Contains all the vertex info for the ms3d file.
typedef struct MS3D_VERTEX_TYP
{
	unsigned char flags;
	unsigned char refCount;
	char boneID;
	float vertex[3];
} MS3D_VERTEX, *MS3D_VERTEX_PTR;

//Contains all the triangle info for the ms3d file.
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

//Contains all the group info for the ms3d file.
typedef struct MS3D_GROUP_TYP
{
	unsigned char flags;
	char name[32];
	unsigned short numTriangles;
	unsigned short *triangleIndices;
	char materialIndex;
} MS3D_GROUP, *MS3D_GROUP_PTR;

//Contains all the material info for the ms3d file.
typedef struct MS3D_MATERIAL_TYP
{
	char name[32];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[4];
	float shininess;
	float transparency;
	BYTE mode;
	char texture[128];
	char alphamap[128];
} MS3D_MATERIAL, *MS3D_MATERIAL_PRT;

class Ms3dFile
{

	public:
		unsigned short 		numVertices;//Number of vertices
		MS3D_VERTEX*	vertices;//The actual vertices
		unsigned short		numTriangles;//Number of triangles
		MS3D_TRIANGLE*	triangles;//The actual triangles
		unsigned short 		numGroups;//Number of groups
		MS3D_GROUP*	groups;//The actual groups
		unsigned short		numMaterials;//Number of materials
		MS3D_MATERIAL*	materials;//The actual materials 

	//Loads a ms3d file into memory
	bool load(char *fileName );
	
	//Simple constructor needed to create the class.
	Ms3dFile(char *fileName)
	{
		load(fileName);
	}
	//Simple deconstructor needed to delete the class.
	~Ms3dFile()
	{
		if(vertices)
			delete vertices;
		if(triangles)
			delete triangles;
		if(groups)
			delete groups;
		if(materials)
			delete materials;
	}



};

