#ifndef __MS3DMODEL_H__
#define __MS3DMODEL_H__

#include <stdio.h>
#include "PS2Defines.h"
#include "cpipe3d.h"
#include "vecmaths.h"

struct Vector3
{
	float x, y, z;
	float s, t;
};

// Struct for a PS2 verte
struct MSPS2Vertex
{
	FVector Vertex;				// Untransformed position
	FVector TVertex;			// Transformed position
	float s, t;					// Untransformed Texture Coordinates
	unsigned char r, g, b, a;	// colour and alpha
	int ADC;					// Clipping bit
	float S, T, Q;				// Transformed Texture Coordinates
};


struct MSVertex
{
	// flags, x, y, z, u, v, bone index
	int flags;
	float x, y, z, u, v;
	int bone;
	int _pad; // 16 byte align
};

struct MSTriangle
{
	// triangle: flags, vertex index1, vertex index2, vertex index3, 
	// normal index1, normal index 2, normal index 3, smoothing group
	int flags, vert1, vert2, vert3,
			   norm1, norm2, norm3, smoothinggroup;
};

struct TriangleList
{
	Vector3 Vertex1;
	Vector3 Vertex2;
	Vector3 Vertex3;
};


struct MSMesh
{
	// mesh: name, flags, material index
	char strName[256];
	int flags, matindex;

	int nTriangles,	nNormals, nVerts;

	MSVertex * pVertices;
	Vector3 * pNormals;
	MSTriangle * pTriangles;
	TriangleList * pTriangleList;
};

struct MSMaterial
{
	char pName[256];
	float ar, ag, ab, aa;
	float dr, dg, db, da;
	float sr, sg, sb, sa;
	float er, eg, eb, ea;
	float shiny;
	float transparency;

	char pColorMap[256];
	char pAlphaMap[256];
};

struct MSPositionKey
{
	float time, posx, posy, posz;
};

struct MSRotationKey
{
	float time, rotx, roty, rotz;
};


class CMs3dModel
{
public:

	CMs3dModel(void);
	~CMs3dModel(void);

	bool Load(const char * strFilename);
	void SetPosition(float x, float y, float z);
	int BuildGSPacket(void *& pMem, CPipe3d * pPipe3d);
	FMatrix GetWorldMatix(void);
	
	void MoveLX(float Delta);
	void MoveLY(float Delta);
	void MoveLZ(float Delta);
	void MoveWX(float Delta);
	void MoveWY(float Delta);
	void MoveWZ(float Delta);
	void Reset(void);
	void SetScale(float Scale);

	float m_RotX;		// Rotation angle in radians round x axis
	float m_RotY;		// Rotation angle in radians round y axis
	float m_RotZ;		// Rotation angle in radians round z axis
	float m_Scale;		// Scaling factor for image
	FVector Pos;
	
protected:

	void GetNextLOC(char * pStr1024);
	
	FMatrix World;

	MSMesh * m_pMeshes;
	
	MSPS2Vertex * m_pVertexList;
	int m_nVertex;

	int m_nFrames;
	int m_nCurrentFrame;
	int m_nMeshes;

	FILE * m_fp;
};

#endif
