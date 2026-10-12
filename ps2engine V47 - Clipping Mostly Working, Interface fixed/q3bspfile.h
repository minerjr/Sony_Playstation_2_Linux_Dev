#include "matrix.h"
#include <memory.h>
#include <stdio.h>

#ifndef Q3BSPFILE_H
#define Q3BSPFILE_H

typedef unsigned char byte;

#define FACE_POLYGON 1

struct vector3i
{
	int x, y , z;
};

struct bspHeader
{
	char strID[4];
	int version;
};

struct bspLump
{
	int offset;
	int length;
};

struct bspVertex
{
	float position[3];
	float textureCoord[2];
	float lightmapCoord[2];
	float normal[3];
	byte color[4];
};

struct bspFace
{
    int textureID;				// The index into the texture array 
    int effect;					// The index for the effects (or -1 = n/a) 
    int type;					// 1=polygon, 2=patch, 3=mesh, 4=billboard 
    int startVertIndex;			// The starting index into this face's first vertex 
    int numOfVerts;				// The number of vertices for this face 
    int meshVertIndex;			// The index into the first meshvertex 
    int numMeshVerts;			// The number of mesh vertices 
    int lightmapID;				// The texture index for the lightmap 
    int lMapCorner[2];			// The face's lightmap corner in the image 
    int lMapSize[2];			// The size of the lightmap section 
    float lMapPos[3];			// The 3D origin of lightmap. 
    float lMapVecs[3][2];		// The 3D space for s and t unit vectors. 
    float vNormal[3];			// The face normal. 
    int size[2];
};

struct bspTexture
{
	char strName[64];
	int flags;
	int contents;
};

struct bspMeshVerts
{
	int offset;
};
enum lumps
{

	entities = 0,				// Stores player/object positions, etc...
    textures,					// Stores texture information
    planes,				    // Stores the splitting planes
    nodes,						// Stores the BSP nodes
    leafs,						// Stores the leafs of the nodes
    leafFaces,					// Stores the leaf's indices into the faces
    leafBrushes,				// Stores the leaf's indices into the brushes
    models,					// Stores the info of world models
    brushes,					// Stores the brushes info (for collision)
    brushSides,				// Stores the brush surfaces info
    vertices,					// Stores the level vertices
    meshVerts,					// Stores the model vertices offsets
    shaders,					// Stores the shader files (blending, anims..)
    faces,						// Stores the faces for the level
    lightmaps,					// Stores the lightmaps for the level
    lightVolumes,				// Stores extra world lighting information
    visData,					// Stores PVS and cluster info (visibility)
    maxLumps					// A constant to store the number of lumps
};


// This is our bitset class for storing which face has already been drawn.
// The bitset functionality isn't really taken advantage of in this version
// since we aren't rendering by leafs and nodes.
class CBitset 
{

public:

	// Initialize all the data members
    CBitset() : m_bits(0), m_size(0) {}

	// This is our deconstructor
	~CBitset() 
	{
		// If we have valid memory, get rid of it
		if(m_bits) 
		{
			delete m_bits;
			m_bits = NULL;
		}
	}

	// This resizes our bitset to a size so each face has a bit associated with it
	void Resize(int count) 
	{ 
		// Get the size of integers we need
		m_size = count/32 + 1;

		// Make sure we haven't already allocated memory for the bits
        if(m_bits) 
		{
			delete m_bits;
			m_bits = 0;
		}

		// Allocate the bits and initialize them
		m_bits = new unsigned int[m_size];
		ClearAll();
	}

	// This does the binary math to set the desired bit
	void Set(int i) 
	{
		m_bits[i >> 5] |= (1 << (i & 31));
	}

	// This returns if the desired bit slot is a 1 or a 0
	int On(int i) 
	{
		return m_bits[i >> 5] & (1 << (i & 31 ));
	}

	// This clears a bit to 0
	void Clear(int i) 
	{
		m_bits[i >> 5] &= ~(1 << (i & 31));
	}

	// This initializes the bits to 0
	void ClearAll() 
	{
		memset(m_bits, 0, sizeof(unsigned int) * m_size);
	}

private:

	// Our private bit data that holds the bits and size
	unsigned int *m_bits;
	int m_size;
};


class Q3BSP_Loader
{

public:
	Q3BSP_Loader();
	Q3BSP_Loader(char *fileName);
	~Q3BSP_Loader();

	bool load(char *fileName);

	int numberOfVerts;
	int numberOfFaces;
	int numberOfMeshVerts;

	bspVertex *m_pVerts;
	bspFace *m_pFaces;
	bspMeshVerts *m_pMeshVerts;

	CBitset facesDrawn;
};

#endif