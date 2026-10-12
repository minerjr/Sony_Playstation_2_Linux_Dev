#include "Ms3dFile.h"

bool Ms3dFile::Load(char *filename)
{
	FILE* file;
	MS3D_HEADER header;
	int loop;

	if((file = fopen(filename, "rb")) == NULL)
	{
		return false;
	}

	fread(&header.id, sizeof(char), 10, file);
	fread(&header.version,1, sizeof(int), file);
	if(strncmp(header.id, "MS3D000000", 10) != 0)
	{
		return false;
	}

	if(header.version != 3 && header.version != 4)
	{
		return false;
	}
	fread(&numVertices, sizeof(unsigned short), 1, file);
	vertices = new MS3D_VERTEX[numVertices];
	for (loop = 0; loop < numVertices; loop++)
	{
		fread(&vertices[loop].flags, sizeof(BYTE), 1, file);
		fread( vertices[loop].vertex, sizeof(float), 3, file);
		fread(&vertices[loop].boneID, sizeof(char), 1, file);
		fread(&vertices[loop].refCount, sizeof(BYTE), 1, file);
	}
	fread(&numTriangles, sizeof(unsigned short),1,file);
	triangles = new MS3D_TRIANGLE[numTriangles];
	for (loop = 0; loop < numTriangles; loop++)
	{
		fread(&triangles[loop].flags, sizeof(unsigned short), 1, file);
		fread( triangles[loop].vertexIndices, sizeof(unsigned short), 3, file);
		fread( triangles[loop].vertexNormals[0], sizeof(float), 3, file);
		fread( triangles[loop].vertexNormals[1], sizeof(float), 3, file);
		fread( triangles[loop].vertexNormals[2], sizeof(float), 3, file);
		fread( triangles[loop].u, sizeof(float), 3, file);
		fread( triangles[loop].v, sizeof(float), 3, file);
		fread(&triangles[loop].smoothingGroup, sizeof(unsigned char), 1, file);
		fread(&triangles[loop].groupIndex, sizeof(unsigned char), 1, file);
	}
	fread(&numGroups, sizeof(unsigned short), 1, file);
	groups = new MS3D_GROUP[numGroups];
	for (loop = 0; loop < numGroups; loop++)
	{
		fread(&groups[loop].flags, sizeof(unsigned char), 1, file);
		fread( groups[loop].name, sizeof(char), 32, file);
		fread(&groups[loop].numTriangles, sizeof(unsigned short), 1, file);
		
		groups[loop].triangleIndices = new unsigned short[groups[loop].numTriangles];
		
		fread( groups[loop].triangleIndices, sizeof(unsigned short), groups[loop].numTriangles, file);
		fread(&groups[loop].materialIndex, sizeof(char), 1, file);
	}

	return true;

}

void Ms3dFile::Render(void)
{

	int loop1;
	int loop2;
	int loop3;

	for (loop1 = 0; loop1 < numGroups; loop1++)
	{
		int index = atoi(&(groups[loop1].materialIndex));
//		glBindTexture(GL_TEXTURE_2D, Maintexture[index]);

//		glBegin(GL_TRIANGLES);
		for (loop2 = 0; loop2 < groups[loop1].numTriangles; loop2++)
		{
			int triangleIndex = groups[loop1].triangleIndices[loop2];
			const MS3D_TRIANGLE* tri = &triangles[triangleIndex];

			for (loop3 = 0; loop3 < 3; loop3++)
			{
				int index = tri->vertexIndices[loop3];
			
//				glNormal3fv(tri->vertexNormals[loop3]);
//				glTexCoord2f(tri->u[loop3], tri->v[loop3]);
//				glVertex3fv(vertices[index].vertex);
			}
		}
//		glEnd();
	}


}

