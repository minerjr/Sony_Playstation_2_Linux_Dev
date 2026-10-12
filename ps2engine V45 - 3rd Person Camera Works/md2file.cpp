#include "md2file.h"

Md2File::Md2File(char *fileName)
{
	memset(&m_Header, 0, sizeof(tMd2Header));

	m_pSkins = NULL;
	m_pTexCoords = NULL;
	m_pTriangles = NULL;
	m_pFrames = NULL;
	load(fileName);
}

Md2File::~Md2File()
{
	cleanUp();
}

bool Md2File::load(char *fileName)
{
	m_FilePointer = fopen(fileName, "rb");

	if (!m_FilePointer)
	{
	
		return false;
	}

	fread(&m_Header, 1, sizeof(tMd2Header), m_FilePointer);

	if (m_Header.version != 8)
	{
	
		return false;
	}

	readMD2Data();

	//if (textureIndex < -1)
	//{
		
	//}
	
	return true;
}


void Md2File::readMD2Data()
{
	unsigned char buffer[MD2_MAX_FRAMESIZE];
	int j = 0;

	m_pSkins	= new tMd2Skin[m_Header.numSkins];
	m_pTexCoords	= new tMd2TexCoord[m_Header.numTexCoords];
	m_pTriangles	= new tMd2Face[m_Header.numTriangles];
	m_pFrames	= new tMd2Frame[m_Header.numFrames];

	fseek(m_FilePointer, m_Header.offsetSkins, SEEK_SET);
	
	fread(m_pSkins, sizeof(tMd2Skin), m_Header.numSkins, m_FilePointer);

	fseek(m_FilePointer, m_Header.offsetTexCoords, SEEK_SET);
	
	fread(m_pTexCoords, sizeof(tMd2TexCoord), m_Header.numTexCoords, m_FilePointer);

	fseek(m_FilePointer, m_Header.offsetTriangles, SEEK_SET);
	
	fread(m_pTriangles, sizeof(tMd2Face), m_Header.numTriangles, m_FilePointer);

	fseek(m_FilePointer, m_Header.offsetFrames, SEEK_SET);

	for (int i = 0; i < m_Header.numFrames; i++)
	{	
		tMd2AliasFrame *pFrame = (tMd2AliasFrame *) buffer;

		m_pFrames[i].pVertices = new tMd2Triangle[m_Header.numVertices];

		fread(pFrame, 1, m_Header.frameSize, m_FilePointer);

		strcpy(m_pFrames[i].strName, pFrame->name);

		tMd2Triangle *pVertices = m_pFrames[i].pVertices;

		for (j = 0;j < m_Header.numVertices;j++)
		{
			pVertices[j].vertex[0] = pFrame->aliasVertices[j].vertex[0] * pFrame->scale[0] + pFrame->translate[0];
			pVertices[j].vertex[2] = -1 * (pFrame->aliasVertices[j].vertex[1] * pFrame->scale[1] + pFrame->translate[1]);
			pVertices[j].vertex[1] = pFrame->aliasVertices[j].vertex[2] * pFrame->scale[2] + pFrame->translate[2];
		}
	}

	fclose(m_FilePointer);
	
}

void Md2File::cleanUp()
{
	

	if (m_pSkins)delete [] m_pSkins;
	if (m_pTexCoords)delete m_pTexCoords;
	if (m_pTriangles)delete m_pTriangles;
	if (m_pFrames)delete m_pFrames;
}
