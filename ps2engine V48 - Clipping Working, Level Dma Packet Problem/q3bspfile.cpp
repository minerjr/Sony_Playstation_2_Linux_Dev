#include "q3bspfile.h"


Q3BSP_Loader::Q3BSP_Loader()
{
	numberOfVerts = 0;
	numberOfFaces = 0;
	
	m_pVerts = NULL;
	m_pFaces = NULL;
}

Q3BSP_Loader::Q3BSP_Loader(char *fileName)
{
	numberOfVerts = 0;
	numberOfFaces = 0;
	
	m_pVerts = NULL;
	m_pFaces = NULL;
	load(fileName);
}

bool Q3BSP_Loader::load(char *fileName)
{
	FILE *inputFile;
	int x = 0;

	inputFile = fopen(fileName, "rb");
	if (inputFile == NULL)return false;

	bspHeader header;
	bspLump lumps[maxLumps];

	fread(&header, 1, sizeof(bspHeader), inputFile);
	fread(&lumps, maxLumps, sizeof(bspLump), inputFile);

	numberOfVerts = lumps[vertices].length / sizeof(bspVertex);
	m_pVerts = new bspVertex[numberOfVerts];

	numberOfFaces = lumps[faces].length / sizeof(bspFace);
	m_pFaces = new bspFace[numberOfFaces];

	numberOfMeshVerts = lumps[meshVerts].length / sizeof(bspMeshVerts);
	m_pMeshVerts = new bspMeshVerts[numberOfMeshVerts];

	fseek(inputFile, lumps[vertices].offset, SEEK_SET);

	for (x = 0; x < numberOfVerts; x++)
	{
		fread(&m_pVerts[x], 1, sizeof(bspVertex), inputFile);
		float tmpFloat = m_pVerts[x].position[1];
		m_pVerts[x].position[1] = m_pVerts[x].position[2];
		m_pVerts[x].position[2] = -tmpFloat;

		//m_pVerts[x].textureCoord[1] *= -1;
	}

	fseek(inputFile, lumps[faces].offset, SEEK_SET);

	fread(m_pFaces, numberOfFaces, sizeof(bspFace), inputFile);

	fseek(inputFile, lumps[meshVerts].offset, SEEK_SET);

	fread(m_pMeshVerts, numberOfMeshVerts, sizeof(bspMeshVerts), inputFile);

	fclose(inputFile);

	facesDrawn.Resize(numberOfFaces);
	return true;
}

Q3BSP_Loader::~Q3BSP_Loader()
{
	if (m_pVerts)
		delete m_pVerts;
	if (m_pFaces)
		delete m_pFaces;

}
