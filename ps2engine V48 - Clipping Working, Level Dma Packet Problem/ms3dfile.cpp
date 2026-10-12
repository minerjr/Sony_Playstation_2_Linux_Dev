#include "ms3dfile.h"


//Ms3dFile:Load(char *filename)
//Takes the name of a ms3d file to load and loads the file into memory to be
//parsed out into ps2 style code. Orginally Dreamcast code. Just goes to show
//how portablilty is a good thing. Saved alot of writing on my part by thinking
//ahead.
//
//Returns true if successfull else return false for anyother reason.

bool Ms3dFile::load(char *filename)
{
	FILE* file;
	MS3D_HEADER header;
	int loop;

	if((file = fopen(filename, "rb")) == NULL)
	{
		return false;
	}
	//Read in the header.
	fread(&header.id, sizeof(char), 10, file);
	fread(&header.version,1, sizeof(int), file);
	//Check to see if the id of the header matches that of a Milkshape file
	if(strncmp(header.id, "MS3D000000", 10) != 0)
	{
		return false;
	}
	//Check to see if the version matches version 3 or 4 of the file.
	if(header.version != 3 && header.version != 4)
	{
		return false;
	}
	//Read in the number of vertices stored in the file
	fread(&numVertices, sizeof(unsigned short), 1, file);
	//Create a MS3D_VERTEX array to hold the vertices.
	vertices = new MS3D_VERTEX[numVertices];
	//Read in the data for every vertex
	for (loop = 0; loop < numVertices; loop++)
	{
		fread(&vertices[loop].flags, sizeof(BYTE), 1, file);
		fread( vertices[loop].vertex, sizeof(float), 3, file);
		fread(&vertices[loop].boneID, sizeof(char), 1, file);
		fread(&vertices[loop].refCount, sizeof(BYTE), 1, file);
	}
	//Read in the number of triangles stored in the file.
	fread(&numTriangles, sizeof(unsigned short),1,file);
	//Create a MS3D_TRIANGLE array to hold the triangles.
	triangles = new MS3D_TRIANGLE[numTriangles];
	//Read in the data for every vertex
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
	//Read in the number of groups stored in the file.
	fread(&numGroups, sizeof(unsigned short), 1, file);
	//Create a MS3D_GROUP array to hold all the groups.
	groups = new MS3D_GROUP[numGroups];
	//Read in the data for every group.
	for (loop = 0; loop < numGroups; loop++)
	{
		fread(&groups[loop].flags, sizeof(unsigned char), 1, file);
		fread( groups[loop].name, sizeof(char), 32, file);
		fread(&groups[loop].numTriangles, sizeof(unsigned short), 1, file);
		
		groups[loop].triangleIndices = new unsigned short[groups[loop].numTriangles];
		
		fread( groups[loop].triangleIndices, sizeof(unsigned short), groups[loop].numTriangles, file);
		fread(&groups[loop].materialIndex, 1, 1, file);
	}
	//Read in the number of materials stored in the file.
	fread(&numMaterials, sizeof(unsigned short), 1, file);
	//This is used for debuging. I was having alot of problems with
	//reading the materials from the file. Ended up being I did not 
	//have my material structure properly filled out with all the 
	//data types and ended up under reading in the data for each material
	//and ended up crupting all the material data quit badly.
	FILE *test;
	test = fopen ("mat.txt", "w+");
	fprintf(test,"Number of materials: %d\n", numMaterials);
	fclose(test);
	test = fopen ("mat.txt", "a+");
	//The material index in this file was crupt but
	//was ok in model.cc. Could never figure out why that was.
	//This was just for trying to dump the values to see what was going
	//on with the file. Ended up being me dumbly not reading all the 
	//data into the file.
	int materialIndex = groups[loop].materialIndex;
	fprintf(test, "Material Index %d\n", materialIndex);
	//If the number of materials is greater then 1 do the following.
	//else do nothing. This is so i don't read in data i am not supose to.
	if (numMaterials > 0)
	{
		//Create an array of MS3D_MATERIAL to hold all the materials.
		materials = new MS3D_MATERIAL[numMaterials];
		//Read in the data for all the materials.
		for (loop = 0; loop < numMaterials; loop++)
		{
			fread(&materials[loop].name, sizeof(char), 32, file);		
			fread(&materials[loop].ambient, sizeof(float), 4, file);
			fread(&materials[loop].diffuse, sizeof(float), 4, file);
			fread(&materials[loop].specular, sizeof(float), 4, file);
			fread(&materials[loop].emissive, sizeof(float), 4, file);
	
			fread(&materials[loop].shininess, sizeof(float), 1, file);
			fread(&materials[loop].transparency, sizeof(float), 1, file);
			
			fread(&materials[loop].mode, sizeof(BYTE), 1, file);
			
			fread(&materials[loop].texture, sizeof(char), 128, file);
			fread( materials[loop].alphamap, sizeof(char), 128, file);
			
			//More debug stuff. can be comment out after.
			fprintf(test, "Ambient Color = %f %f %f %f\n", materials[loop].ambient[0], materials[loop].ambient[1], materials[loop].ambient[2], materials[loop].ambient[3]);
			fprintf(test, "Diffuse Color = %f %f %f %f\n", materials[loop].diffuse[0], materials[loop].diffuse[1], materials[loop].diffuse[2], materials[loop].diffuse[3]);
			fprintf(test, "Specular Color = %f %f %f %f\n", materials[loop].specular[0], materials[loop].specular[1], materials[loop].specular[2], materials[loop].specular[3]);
			fprintf(test, "Emissive Color = %f %f %f %f\n", materials[loop].emissive[0], materials[loop].emissive[1], materials[loop].emissive[2], materials[loop].emissive[3]);


			fprintf(test, "Shininess = %f\n", materials[loop].shininess);
			//fprintf(test, "Texture = %d : Texure Name =  \n", materials[loop].texture);
			
		}

	}
	else
	{
		//If there are no materials to be loaded set materials to NULL
		materials = NULL;
	}
	//Close off the debug file and also the actual file. Don't want to
	//cause a seg fault like i kept on doing cause i forgot to close
	//the file.
	fclose(test);

	fclose(file);
	return true;

}


