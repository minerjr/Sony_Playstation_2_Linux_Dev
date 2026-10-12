#include <memory.h>
#include "sps2.h"
#include "PS2Defines.h"
#include "AddRegisters.h"
#include "ms3dmodel.h"



#define MSVERBOSE
#define DVOUT

CMs3dModel::CMs3dModel(void)
{
	m_pMeshes = NULL;
	m_nMeshes = 0;
	m_RotX = 0.0f;
	m_RotY = 0.0f;
	m_RotZ = 0.0f;
	m_Scale = 1.0f;
	SetPosition(0.0f, -30.0f, 0.0f);
}

CMs3dModel::~CMs3dModel(void)
{
	for(int nMesh = 0; nMesh < m_nMeshes; nMesh++)
	{
		if(m_pMeshes[nMesh].pVertices != NULL)
		{
			#ifdef MSVERBOSE
			printf("Delete m_pMeshes[%d].pVertices\n", nMesh);
			#endif
			delete [] m_pMeshes[nMesh].pVertices;
			m_pMeshes[nMesh].pVertices = NULL;
		}
		
		if(m_pMeshes[nMesh].pNormals != NULL)
		{
			#ifdef MSVERBOSE
			printf("Delete m_pMeshes[%d].pNormals\n", nMesh);
			#endif
			delete [] m_pMeshes[nMesh].pNormals;
			m_pMeshes[nMesh].pNormals = NULL;
		}
		
		if(m_pMeshes[nMesh].pTriangles != NULL)
		{
			#ifdef MSVERBOSE
			printf("Delete m_pMeshes[%d].pTriangles\n", nMesh);
			#endif
			delete [] m_pMeshes[nMesh].pTriangles;
			m_pMeshes[nMesh].pTriangles = NULL;
		}
		
		if(m_pMeshes[nMesh].pTriangleList != NULL)
		{
			#ifdef MSVERBOSE
			printf("Delete m_pMeshes[%d].pTriangleList\n", nMesh);
			#endif
			delete [] m_pMeshes[nMesh].pTriangleList;
			m_pMeshes[nMesh].pTriangleList = NULL;
		}
	}

	if(m_pMeshes != NULL)
	{
		#ifdef MSVERBOSE
		printf("Delete m_pMeshes\n");
		#endif
		delete [] m_pMeshes;
		m_pMeshes = NULL;
	}
	
	if(m_pVertexList != NULL)
	{
		#ifdef MSVERBOSE
		printf("Delete m_pVertexList\n");
		#endif
		delete [] m_pVertexList;
		m_pVertexList = NULL;
	}
	
	// do more delete here
}

void CMs3dModel::GetNextLOC(char * pStr1024)
{
	bool foundcode = false;

	while(foundcode != true)
	{
		fgets(pStr1024, 1024, m_fp);

		if((pStr1024[0] != '/' &&
		   pStr1024[1] != '/') &&
		   pStr1024[0] != '\r')
		   foundcode = true;
	}
}

bool CMs3dModel::Load(const char *strFilename)
{
	m_fp = fopen(strFilename, "ra");

	if(m_fp == NULL)
	{
		printf("Failed to load model %s.\n", strFilename);
		exit(-1);
	}

	char strLine[1024];

	// Frames
	GetNextLOC(strLine);
	sscanf(strLine, "Frames: %d", &m_nFrames);

#ifdef MSVERBOSE
	printf("Model: %s\nFrames: %d\n", strFilename, m_nFrames);
#endif

	// Current frame
	GetNextLOC(strLine);
	sscanf(strLine, "Frame: %d", &m_nCurrentFrame);

#ifdef MSVERBOSE
	printf("Current Frame: %d\n", m_nCurrentFrame);
#endif

	// No. meshes
	GetNextLOC(strLine);
	sscanf(strLine, "Meshes: %d", &m_nMeshes);

#ifdef MSVERBOSE
	printf("Meshes: %d\n", m_nMeshes);
#endif

	m_pMeshes = new MSMesh[m_nMeshes];
	memset(m_pMeshes, 0, m_nMeshes * sizeof(MSMesh));

	for(int nMesh = 0; nMesh < m_nMeshes; nMesh++)
	{
		GetNextLOC(strLine);
		// mesh: name, flags, material index
		// eg	"Box02" 0 -1
		sscanf(strLine,	"\"%[^\"]\" %d %d", m_pMeshes[nMesh].strName,
						&m_pMeshes[nMesh].flags,
						&m_pMeshes[nMesh].matindex);

#ifdef MSVERBOSE
		printf("%s %d %d\n",		m_pMeshes[nMesh].strName,
						m_pMeshes[nMesh].flags,
						m_pMeshes[nMesh].matindex);
#endif

		// No. vertices
		GetNextLOC(strLine);
		sscanf(strLine, "%d", &m_pMeshes[nMesh].nVerts);

#ifdef MSVERBOSE
		printf("Vertices: %d\n", m_pMeshes[nMesh].nVerts);
#endif

		m_pMeshes[nMesh].pVertices = new MSVertex[m_pMeshes[nMesh].nVerts];

		// The vertices
		for(int nVert = 0; nVert < m_pMeshes[nMesh].nVerts; nVert++)
		{
			GetNextLOC(strLine);
			// flags, x, y, z, u, v, bone index
			sscanf(strLine, "%d %f %f %f %f %f %d", 
				&m_pMeshes[nMesh].pVertices[nVert].flags,
				&m_pMeshes[nMesh].pVertices[nVert].x,
				&m_pMeshes[nMesh].pVertices[nVert].y,
				&m_pMeshes[nMesh].pVertices[nVert].z,
				&m_pMeshes[nMesh].pVertices[nVert].u,
				&m_pMeshes[nMesh].pVertices[nVert].v,
				&m_pMeshes[nMesh].pVertices[nVert].bone);

#ifdef MSVERBOSE
			printf("%d %f %f %f %f %f %d\n", 
				m_pMeshes[nMesh].pVertices[nVert].flags,
				m_pMeshes[nMesh].pVertices[nVert].x,
				m_pMeshes[nMesh].pVertices[nVert].y,
				m_pMeshes[nMesh].pVertices[nVert].z,
				m_pMeshes[nMesh].pVertices[nVert].u,
				m_pMeshes[nMesh].pVertices[nVert].v,
				m_pMeshes[nMesh].pVertices[nVert].bone);
#endif
		}

		// No. normals
		GetNextLOC(strLine);
		sscanf(strLine, "%d", &m_pMeshes[nMesh].nNormals);

#ifdef MSVERBOSE
		printf("Normals: %d\n", m_pMeshes[nMesh].nNormals);
#endif

		m_pMeshes[nMesh].pNormals = new Vector3[m_pMeshes[nMesh].nNormals];

		// The normals
		for(int nNormal = 0; nNormal < m_pMeshes[nMesh].nNormals; nNormal++)
		{
			GetNextLOC(strLine);

			// x, y, z
			sscanf(strLine, "%f %f %f",
				&m_pMeshes[nMesh].pNormals[nNormal].x,
				&m_pMeshes[nMesh].pNormals[nNormal].y,
				&m_pMeshes[nMesh].pNormals[nNormal].z);

#ifdef MSVERBOSE
			printf("%f %f %f\n",
				m_pMeshes[nMesh].pNormals[nNormal].x,
				m_pMeshes[nMesh].pNormals[nNormal].y,
				m_pMeshes[nMesh].pNormals[nNormal].z);
#endif
		}

		// No. triangles
		GetNextLOC(strLine);
		sscanf(strLine, "%d", &m_pMeshes[nMesh].nTriangles);

#ifdef MSVERBOSE
		printf("Triangles: %d\n", m_pMeshes[nMesh].nTriangles);
#endif

		m_pMeshes[nMesh].pTriangles = new MSTriangle[m_pMeshes[nMesh].nTriangles];

		// The triangles
		for(int nTriangle = 0; nTriangle < m_pMeshes[nMesh].nTriangles; nTriangle++)
		{
			GetNextLOC(strLine);

			// triangle: flags, vertex index1, vertex index2, vertex index3, 
			// normal index1, normal index 2, normal index 3, smoothing group
			sscanf(strLine, "%d %d %d %d %d %d %d %d",
				&m_pMeshes[nMesh].pTriangles[nTriangle].flags,
				&m_pMeshes[nMesh].pTriangles[nTriangle].vert1,
				&m_pMeshes[nMesh].pTriangles[nTriangle].vert2,
				&m_pMeshes[nMesh].pTriangles[nTriangle].vert3,
				&m_pMeshes[nMesh].pTriangles[nTriangle].norm1,
				&m_pMeshes[nMesh].pTriangles[nTriangle].norm2,
				&m_pMeshes[nMesh].pTriangles[nTriangle].norm3,
				&m_pMeshes[nMesh].pTriangles[nTriangle].smoothinggroup);

#ifdef MSVERBOSE
			printf("%d %d %d %d %d %d %d %d\n",
				m_pMeshes[nMesh].pTriangles[nTriangle].flags,
				m_pMeshes[nMesh].pTriangles[nTriangle].vert1,
				m_pMeshes[nMesh].pTriangles[nTriangle].vert2,
				m_pMeshes[nMesh].pTriangles[nTriangle].vert3,
				m_pMeshes[nMesh].pTriangles[nTriangle].norm1,
				m_pMeshes[nMesh].pTriangles[nTriangle].norm2,
				m_pMeshes[nMesh].pTriangles[nTriangle].norm3,
				m_pMeshes[nMesh].pTriangles[nTriangle].smoothinggroup);
#endif
		}

		m_pMeshes[nMesh].pTriangleList = new TriangleList[m_pMeshes[nMesh].nTriangles];
		
		for(int nTriangle = 0; nTriangle < m_pMeshes[nMesh].nTriangles; nTriangle++)
		{
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.x = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert1].x;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.y = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert1].y;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.z = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert1].z;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.s = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert1].u;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.t = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert1].v;
										
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.x = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert2].x;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.y = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert2].y;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.z = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert2].z;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.s = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert2].u;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.t = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert2].v;
					
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.x = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert3].x;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.y = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert3].y;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.z = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert3].z;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.s = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert3].u;
			m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.t = \
				m_pMeshes[nMesh].pVertices[m_pMeshes[nMesh].pTriangles[nTriangle].vert3].v;
		}
	
		
#ifdef MSVERBOSE
	
		printf("Number of Triangles: %d\n", m_pMeshes[nMesh].nTriangles);
	
	
		for(int nTriangle = 0; nTriangle < m_pMeshes[nMesh].nTriangles; nTriangle++)
		{
			printf("%3.1f %3.1f %3.1f %3.1f %3.1f -- ", 
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.x,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.y,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.z,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.s,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.t);
										
			printf("%3.1f %3.1f %3.1f %3.1f %3.1f -- ", 
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.x,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.y,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.z,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.s,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.t);
										
			printf("%3.1f %3.1f %3.1f %3.1f %3.1f\n", 
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.x,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.y,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.z,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.s,
										m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.t);
		}

#endif

	}

	m_nVertex = 0;
	
	for(int nMesh = 0; nMesh < m_nMeshes; nMesh++)
		m_nVertex += m_pMeshes[nMesh].nTriangles;
		
	m_nVertex *= 3;
	
	
//#ifdef MSVERBOSE
	printf("Total number of vertices = %d\n", m_nVertex);
//#endif
	
	m_pVertexList = new MSPS2Vertex[m_nVertex];
	
	for(int nMesh = 0; nMesh < m_nMeshes; nMesh++)
		{
		for(int nTriangle = 0; nTriangle < m_pMeshes[nMesh].nTriangles; nTriangle++)
			{
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].Vertex.v[0] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.x;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].Vertex.v[1] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.y;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].Vertex.v[2] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.z;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].Vertex.v[3] = \
					1.0f;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].s = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.s;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].t = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex1.t;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].r = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].g = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].b = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 0].a = 0x80;
					
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].Vertex.v[0] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.x;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].Vertex.v[1] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.y;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].Vertex.v[2] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.z;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].Vertex.v[3] = \
					1.0f;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].s = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.s;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].t = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex2.t;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].r = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].g = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].b = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 1].a = 0x80;
					
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].Vertex.v[0] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.x;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].Vertex.v[1] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.y;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].Vertex.v[2] = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.z;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].Vertex.v[3] = \
					1.0f;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].s = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.s;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].t = \
					m_pMeshes[nMesh].pTriangleList[nTriangle].Vertex3.t;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].r = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].g = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].b = 0x80;
				m_pVertexList[nMesh * m_nMeshes + nTriangle * 3 + 2].a = 0x80;
			
			}		
		}	

#ifdef MSVERBOSE

	for(int i = 0; i < m_nVertex; i++)
	{
		printf("Vertex %d => %3.1f %3.1f %3.1f %3.1f %3.1f %3.1f\n",
				i,
				m_pVertexList[i].Vertex.v[0],
				m_pVertexList[i].Vertex.v[1],
				m_pVertexList[i].Vertex.v[2],
				m_pVertexList[i].Vertex.v[3],
				m_pVertexList[i].s,
				m_pVertexList[i].t);
	}
				
#endif

	fclose(m_fp);
	
	return true;
}


int CMs3dModel::BuildGSPacket(void *& pMem, CPipe3d * pPipe3d)
{
	static int tout = 1;
	FILE *tmpOut;
	//Temp is the identity matrix used for rotation
	FMatrix Temp;
	IdentityMatrix(&Temp);
	
	//get the world positional matrix for the sprite
	
	//The Scale matrix
	ScaleMatrix(&World, &Temp, m_Scale);
	
	// The rotation
	RotMatrix(&World, &World, m_RotZ, m_RotY, m_RotX);
	
	//now the position
	World.m[0][3] = Pos.v[0];
	World.m[1][3] = Pos.v[1];
	World.m[2][3] = Pos.v[2];	
	
	FVector testVec;
	SetVector(&testVec, 1.0f, 1.0f, 1.0f, 1.0f);
	FVector testVect;
	SetVector(&testVect, 1.0f, 1.0f, 1.0f, 1.0f);
	

	// Apply World matrix to the untransformed coordinates
	for(int i = 0; i < m_nVertex; i++)
		ApplyMatrix(&m_pVertexList[i].TVertex, &World, &m_pVertexList[i].Vertex);
#ifdef DVOUT

	printf("Untransformed: X: %f Y: %f Z: %f W: %f\n", testVec.v[0], testVec.v[1],testVec.v[2], testVect.v[3]);
	ApplyMatrix(&testVect, &World, &testVect);
	printf("Model transfomed: X: %f Y: %f Z: %f W: %f\n", testVect.v[0], testVect.v[1],testVect.v[2], testVect.v[3]);
#endif

	
/*
	// Apply Camera matrix and perspective transformation matrices
	for(int i = 0; i < 4; i++)
		ApplyMatrix(&pSprite->Vertex[i].TVertex, &Pipe3d.WS, &pSprite->Vertex[i].TVertex);
*/

	// Apply Camera matrix
	for(int i = 0; i < m_nVertex; i++)
		ApplyMatrix(&m_pVertexList[i].TVertex, &pPipe3d->WC, &m_pVertexList[i].TVertex);
#ifdef DVOUT

	ApplyMatrix(&testVect, &pPipe3d->WC, &testVect);
	printf("Camera transfomed: X: %f Y: %f Z: %f W: %f\n", testVect.v[0], testVect.v[1],testVect.v[2], testVect.v[3]);
#endif
	// Apply perspective transformation matrix
	for(int i = 0; i < m_nVertex; i++)
		ApplyMatrix(&m_pVertexList[i].TVertex, &pPipe3d->VS, &m_pVertexList[i].TVertex);
#ifdef DVOUT

	ApplyMatrix(&testVect, &pPipe3d->VS, &testVect);
	printf("Projection transfomed: X: %f Y: %f Z: %f W: %f\n", testVect.v[0], testVect.v[1],testVect.v[2], testVect.v[3]);
#endif
	// Get Perspective correct texture coordinates
	for(int i = 0; i < m_nVertex; i++)
	{	
		m_pVertexList[i].Q = 1.0f/m_pVertexList[i].TVertex.v[3];
		m_pVertexList[i].S = m_pVertexList[i].s * m_pVertexList[i].Q;
		m_pVertexList[i].T = m_pVertexList[i].t * m_pVertexList[i].Q;
	}
	
	// Do Homogeneous divide 
	for(int i = 0; i < m_nVertex; i++)	
		pPipe3d->HomoDivide(&m_pVertexList[i].TVertex);

	#ifdef DVOUT

	pPipe3d->HomoDivide(&testVect);
	printf("H Divide transfomed: X: %f Y: %f Z: %f W: %f\n", testVect.v[0], testVect.v[1],testVect.v[2], testVect.v[3]);
	#endif
	// Clip Check the point and print out some stats
	for(int i = 0; i < m_nVertex; i++)
	{
		//static int iClip = 0;	
		if(pPipe3d->ClipCheck(&m_pVertexList[i].TVertex))
		{
			//printf("%d *********** Vertex %d has been clipped\n", iClip++, i);
			m_pVertexList[i].ADC = 1;
		}
		else
		{
			//printf("*********** This point will be drawn\n\n");
			m_pVertexList[i].ADC = 0;
		}
	}
	
	// Do Viewport transformation
	for(int i = 0; i < m_nVertex; i++)
		ApplyMatrix(&m_pVertexList[i].TVertex, &pPipe3d->VT, &m_pVertexList[i].TVertex);
	

	#ifdef DVOUT
	ApplyMatrix(&testVect, &pPipe3d->VT, &testVect);
	printf("View transfomed: X: %f Y: %f Z: %f W: %f\n", testVect.v[0], testVect.v[1],testVect.v[2], testVect.v[3]);
	#endif

	// Create a textured model.
	sps2GIFTag_t * pGIFTag = (sps2GIFTag_t *)pMem;
	pGIFTag->i128 = 0;
	pGIFTag->s.NLOOP = m_nVertex;
	pGIFTag->s.PRE = 1;		// Transfer PRIM to the GS
	pGIFTag->s.EOP = 1;		// This is the last giftag in the packet
	 
	pGIFTag->s.PRIM = GS_PRIM(PRIM_TRI, 
							PRIM_IIP_FLAT, 
							PRIM_TME_ON,
							PRIM_FGE_OFF, 
							PRIM_ABE_ON, 
							PRIM_AA1_OFF, 
							PRIM_FST_STQ, 
							PRIM_CTXT_CONTEXT1, 
							PRIM_FIX_NOFIXDDA);
							
	pGIFTag->s.FLG = GIF_FLG_PACKED;
	pGIFTag->s.NREG = 3; 
	// This is a faster way of setting
	// the REGS field (1 = RGBA, 3 = UV, 5 = XYZ2, 2 = ST)
	pGIFTag->s.REGS = uint64(0x512);
		
	// Set the register pointer to the memory just after the GIFTag
	// pGIFTag + 1 will move the address of the pointer
	// ahead by the sizeof(sps2GIFTag_t) which is 1 quadword (128 bits)
	sps2GIFPackedRegister_t	* pRegister = (sps2GIFPackedRegister_t *)(pGIFTag + 1);
	
	if (tout == 1)
	{
		tmpOut = fopen("Test.txt", "w");
		if (tmpOut == NULL)tout = 0;
		fprintf(tmpOut, "Spawn Vertex Data Dump \n\n");
	}
	int *tmpReg;
	for(int i = 0; i < m_nVertex; i++)
	{
		AddSTQ(pRegister, m_pVertexList[i].S, m_pVertexList[i].T, m_pVertexList[i].Q);
		AddColour(pRegister, m_pVertexList[i].r, m_pVertexList[i].g, m_pVertexList[i].b, m_pVertexList[i].a);
		tmpReg = (int *)&pRegister->i128;
		AddVertexFPo(pRegister, (int)(m_pVertexList[i].TVertex.v[0]), 
								(int)(m_pVertexList[i].TVertex.v[1]), 
								(int)(m_pVertexList[i].TVertex.v[2]), 
								m_pVertexList[i].ADC);
		if (tout == 1)
		{
			fprintf(tmpOut, "%d: %.8x %.8x %.8x %.8x\n",i, tmpReg[0], 
								tmpReg[1], 
								tmpReg[2],
								tmpReg[3]);
		}

	}
	if (tout == 1)
	{
		tout = 0;
		fclose(tmpOut);
	}
	// Work out how much we just wrote
	int iWC = (int)pRegister - (int)pMem;
	
	// Assert that we haven't written more data than we allocated
	//assert(iWC < 4096);
	
	//Set the memory pointer we used to the next free location
	pMem = (char *)pRegister;
	
	//return the number of bytes written to memory
	return iWC;	
}


void CMs3dModel::SetPosition(float x, float y, float z)
{
		Pos.v[0] = x;
		Pos.v[1] = y;
		Pos.v[2] = z;
		Pos.v[3] = 1.0f;
}


FMatrix CMs3dModel::GetWorldMatix(void)
{
	return World;	
}

void CMs3dModel::MoveLX(float Delta)
{
	//this moves the sprite along its local axis by an amount Delta
	Pos.v[0] += World.m[0][0]* Delta;
	Pos.v[1] += World.m[1][0]* Delta;
	Pos.v[2] += World.m[2][0]* Delta;
}

void CMs3dModel::MoveLY(float Delta)
{
	//this moves the sprite along its local axis by an amount Delta
	Pos.v[0] += World.m[0][1]* Delta;
	Pos.v[1] += World.m[1][1]* Delta;
	Pos.v[2] += World.m[2][1]* Delta;
}

void CMs3dModel::MoveLZ(float Delta)
{
	//this moves the sprite along its local axis by an amount Delta
	Pos.v[0] += World.m[0][2]* Delta;
	Pos.v[1] += World.m[1][2]* Delta;
	Pos.v[2] += World.m[2][2]* Delta;
}

void CMs3dModel::MoveWX(float Delta)
{
	// This moves the sprite along the World by an amount Delta
	Pos.v[0] +=  Delta;
}

void CMs3dModel::MoveWY(float Delta)
{
	// This moves the sprite along the World by an amount Delta
	Pos.v[1] +=  Delta;
}

void CMs3dModel::MoveWZ(float Delta)
{
	// This moves the sprite along the World by an amount Delta
	Pos.v[2] +=  Delta;
}



void CMs3dModel::Reset(void)
{
		m_RotX = 0.0f;
		m_RotY = 0.0f;
		m_RotZ = 0.0f;
		
		Pos.v[0] = 0.0f;
		Pos.v[1] = -30.0f;
		Pos.v[2] = 0.0f;
}


void CMs3dModel::SetScale(float Scale)
{
		m_Scale = Scale;
}
