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
		fread(&groups[loop].materialIndex, 1, 1, file);
	}
	
	fread(&numMaterials, sizeof(unsigned short), 1, file);
	FILE *test;
	test = fopen ("mat.txt", "w+");
	fprintf(test,"Number of materials: %d\n", numMaterials);
	fclose(test);
	test = fopen ("mat.txt", "a+");
	int materialIndex = groups[loop].materialIndex;
	fprintf(test, "Material Index %d\n", materialIndex);
	if (numMaterials > 0)
	{
		materials = new MS3D_MATERIAL[numMaterials];
		for (loop = 0; loop < numMaterials; loop++)
		{
			fread(&materials[loop].name, sizeof(char), 32, file);		
			fread(&materials[loop].ambient, sizeof(float), 4, file);
			fread(&materials[loop].diffuse, sizeof(float), 4, file);
			fread(&materials[loop].specular, sizeof(float), 4, file);
			fread(&materials[loop].emissive, sizeof(float), 4, file);
	
			fread(&materials[loop].shininess, sizeof(float), 1, file);
			fread(&materials[loop].transparency, sizeof(float), 1, file);
			
			fread(&materials[loop].mode, sizeof(byte), 1, file);
			
			fread(&materials[loop].texture, sizeof(char), 128, file);
			//if (materials[loop].texture > 0)
			//{
				
			//	materials[loop].TextureFileName = new char[materials[loop].texture + 1]; 
			fread( materials[loop].alphamap, sizeof(char), 128, file);
			//}
			fprintf(test, "Ambient Color = %f %f %f %f\n", materials[loop].ambient[0], materials[loop].ambient[1], materials[loop].ambient[2], materials[loop].ambient[3]);
			fprintf(test, "Diffuse Color = %f %f %f %f\n", materials[loop].diffuse[0], materials[loop].diffuse[1], materials[loop].diffuse[2], materials[loop].diffuse[3]);
			fprintf(test, "Specular Color = %f %f %f %f\n", materials[loop].specular[0], materials[loop].specular[1], materials[loop].specular[2], materials[loop].specular[3]);
			fprintf(test, "Emissive Color = %f %f %f %f\n", materials[loop].emissive[0], materials[loop].emissive[1], materials[loop].emissive[2], materials[loop].emissive[3]);


			fprintf(test, "Shininess = %f\n", materials[loop].shininess);
			fprintf(test, "Texture = %d : Texure Name =  \n", materials[loop].texture);
			
		}

	}
	else
	{
		materials = NULL;
	}
	fclose(test);

	fclose(file);
	return true;

}


