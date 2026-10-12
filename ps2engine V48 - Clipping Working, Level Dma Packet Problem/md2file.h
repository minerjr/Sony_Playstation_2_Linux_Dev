#ifndef _MD2FILE_H
#define _MD2FILE_H

#include <math.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char byte;

#define MD2_MAX_TRIANGLES	4096
#define MD2_MAX_VERTICES	2046
#define MD2_MAX_TEXCOORDS	2048
#define MD2_MAX_FRAMES		512
#define MD2_MAX_SKINS		32
#define MD2_MAX_FRAMESIZE	(MD2_MAX_VERTICES * 4 + 128)

struct tMd2Header
{
	int magic;
	int version;
	int skinWidth;
	int skinHeight;
	int frameSize;
	int numSkins;
	int numVertices;
	int numTexCoords;
	int numTriangles;
	int numGLCommands;
	int numFrames;
	int offsetSkins;
	int offsetTexCoords;
	int offsetTriangles;
	int offsetFrames;
	int offsetGLCommands;
	int offsetEnd;

};

struct tMd2AliasTriangle
{
	byte vertex[3];
	byte lightNormalIndex;
};

struct tMd2Triangle
{
	float vertex[3];
	float normal;
};

struct tMd2Face
{
	short vertexIndices[3];
	short textureIndices[3];
};

struct tMd2TexCoord
{
	short u, v;
};

struct tMd2AliasFrame
{
	float scale[3];
	float translate[3];
	char name[16];
	tMd2AliasTriangle aliasVertices[1];
};

struct tMd2Frame
{
	char strName[16];
	tMd2Triangle *pVertices;
};

typedef char tMd2Skin[64];

class Md2File
{
public:
	Md2File(char *fileName);
	~Md2File();
	bool load(char *fileName);

	void readMD2Data();
	void cleanUp();

	FILE *m_FilePointer;

	tMd2Header		m_Header;
	tMd2Skin		*m_pSkins;
	tMd2TexCoord	*m_pTexCoords;
	tMd2Face		*m_pTriangles;
	tMd2Frame		*m_pFrames;

};

#endif
