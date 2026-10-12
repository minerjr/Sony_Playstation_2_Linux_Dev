/****************************************************************************\
|* PS2 framework definitions
|* =========================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|*
|* Author					: SjG, various others
|* First created			: Thu Jul 18 13:25:57 GMT 2002
|* Modification history		: See below
|*
\****************************************************************************/
#include "model.h"


/****************************************************************************\
|* Constructor
\****************************************************************************/
Model::Model()
	{
	initialise();
	}

/****************************************************************************\
|* Constructor
\****************************************************************************/
Model::Model(char *filename)
	{
	initialise();
	loadFile(filename);
	}
	
/****************************************************************************\
|* Destructor
\****************************************************************************/
Model::~Model()
	{
	erase();
	}


/****************************************************************************\
|* Method - initialise the model
\****************************************************************************/
void Model::initialise(void)
	{
	mesh				= NULL;
	nmeshes				= 0;
	nfaces				= 0;
	vpkt.buffer			= NULL;
	vpkt.buffer_size	= 0;
	type				= MODEL_BAD_FORMAT;

	objTrans			= (ps2_vu0_fvector){0,0,0,0};
	objRot				= (ps2_vu0_fvector){0,0,0,0};
	objRot0				= (ps2_vu0_fvector){0,0,0,0};
	
	localScreen			= NULL;
	localLight			= NULL;
	lightColour			= NULL;
	localClip			= NULL;
	outlineFactor		= NULL;
	}

/****************************************************************************\
|* Method - delete the model data
\****************************************************************************/
void Model::erase(void)
	{
	int i;
	if (vpkt.buffer != NULL)
		free(vpkt.buffer);
		
	for (i=nmeshes-1; i>=0; i--)
		{
		free(mesh[i].st);
		free(mesh[i].normal);
		free(mesh[i].vertex);
		}
		
	free(mesh);
	initialise();
	}
/****************************************************************************\
|* Method - dump the model data to a file
\****************************************************************************/
void Model::dump(void)
	{
	unsigned int i, j, mesh_verts;
	qword_t *TmpVertex, *TmpNormal;
	//if (vpkt.buffer != NULL)
	//	free(vpkt.buffer);
	FILE *output;
	output = fopen("ModelData.txt","w+");
	fprintf(output, "Number of meshes: %d\n", nmeshes);
	fclose(output);
	output = fopen("ModelData.txt","a");
	fprintf(output, "Number of faces: %d\n", nfaces);
	fprintf(output, "Type: %d\n", type);

	
	
	for (i=0; i< nmeshes; i++)
	{
		fprintf(output, "Mesh Number: %d\n", i);
		fprintf(output, "RGBA: %f %f %f %f\n", mesh[i].rgb.f32[0], mesh[i].rgb.f32[1], mesh[i].rgb.f32[2], mesh[i].rgb.f32[3]);
		fprintf(output, "Texture Index: %d\n", mesh[i].tex_index);
		fprintf(output, "Number of faces: %d\n", mesh[i].nfaces);
		fprintf(output, "Texture: %d\n", mesh[i].texture);		
		fprintf(output, "Number of vertexs: %d\n", mesh[i].nfaces * 3);
		if (type == MODEL_STRIPPED)
		{
			mesh_verts = mesh[i].nfaces+2;
		}
		else if (type == MODEL_TRIMODEL)
		{
			mesh_verts = mesh[i].nfaces * 3;
		}
		for (j = 0; j < mesh_verts; j+=1)
		{
			unsigned int vu_verts = VU_VERTS_STRIP;
			//memcpy(&TmpX, &mesh[i].vertex + j, 4);
			//memcpy(&TmpY, &mesh[i].vertex + j + 5, 4);
			//memcpy(&TmpZ, &mesh[i].vertex + j + 10, 4);
			if (j + VU_VERTS_STRIP > mesh[i].nfaces+2)
				vu_verts = mesh_verts % VU_VERTS_STRIP;
			TmpVertex = (qword_t*)(mesh[i].vertex +j * VERT_DATA_LEN_STRIP);
			TmpNormal = (qword_t*)(mesh[i].normal +j * NORM_DATA_LEN_STRIP);
			fprintf(output, "Vertex: %f %f %f\n", TmpVertex->fvect[0], TmpVertex->fvect[1], TmpVertex->fvect[2]);	
			fprintf(output, "Normal: %f %f %f\n", TmpNormal->fvect[0], TmpNormal->fvect[1], TmpNormal->fvect[2]);	
						
		}
		
		fprintf(output, "Size of Vertex Array: %d\n", sizeof(mesh[i].vertex));
		//free(mesh[i].st);
		//free(mesh[i].normal);
		//free(mesh[i].vertex);
		}
	fclose(output);	
		
	//free(mesh);
	//initialise();
	}
	
/****************************************************************************\
|* Method - load a file
\****************************************************************************/
void Model::loadFile(char *name)
	{
	if (strstr(name, ".ms3d") != NULL)
		loadMS3D(name);
	else if (strstr(name, ".ms") != NULL)
		loadStripped(name);
	else
		type = MODEL_BAD_FORMAT;
	}
	

	
/****************************************************************************\
|* Method - load a stripped-triangle file
\****************************************************************************/
void Model::loadStripped(char *name)
	{
	char tmpPath[1024];
	unsigned int *indexes = NULL;

	erase();

	FILE *sf = fopen(name, "r");
	if (sf==NULL)
		{
		_log << "Could not load file '" << name << "'\n";
		type=MODEL_BAD_FORMAT;
		return;
		}

	fseek(sf, 0, SEEK_END);
	int filesize=ftell(sf);
	fseek(sf, 0, SEEK_SET);
	_log << name << " size: " << filesize << " bytes\n";

	if (filesize<13)
		{
		_log <<"Unexpected end of stripped file\n";
		fclose(sf);
		type=MODEL_BAD_FORMAT;
		}
	else
		{
		unsigned int i;
		struct
			{
			char magic[8];
			unsigned char version;
			unsigned int nmeshes;
			unsigned int ntextures;
			} header;

		fread(&header, 1, 9, sf);

		if (strncmp(header.magic, STRIPPED_MAGIC, strlen(STRIPPED_MAGIC))!=0)
			{
			_log << "Invalid file format\n";
			fclose(sf);
			type=MODEL_BAD_FORMAT;
			return;
			}

		if (header.version!=STRIPPED_VERSION)
			{
			_log << "Incorrect file version\n";
			fclose(sf);
			type=MODEL_BAD_FORMAT;
			return;
			}

		fread(&header.nmeshes, 4, 1, sf);
		fread(&header.ntextures, 4, 1, sf);
		indexes=(unsigned int *)malloc(header.ntextures*sizeof(unsigned int));

		for (i=0; i<header.ntextures; i++)
			{
			unsigned char len;
			fread(&len, 1, 1, sf);
			*(tmpPath+len)=0;
			fread(tmpPath, 1, len, sf);
			// indexes[i]=AddTexture(tmp_path);
			}

		nfaces=0;
		nmeshes=header.nmeshes;

		mesh=(mesh_t*) memalign(64, nmeshes * sizeof(mesh_t));
		memset(mesh, 0, nmeshes * sizeof(mesh_t));

		/********************************************************************\
		|* Read in each mesh
		\********************************************************************/
		for (i=0; i<nmeshes; i++)
			{
			fread(&mesh[i].texture, sizeof(unsigned int), 1, sf);

			if (mesh[i].texture>=0)
				mesh[i].texture=indexes[mesh[i].texture];
			
			fread(&mesh[i].rgb, sizeof(float), 4, sf);
			fread(&mesh[i].nfaces, sizeof(unsigned int), 1, sf);
			
			/****************************************************************\
			|* Decompress the vertex strip
			\****************************************************************/
			lzo_uint out_len1 = mesh[i].nfaces * VERT_DATA_LEN_STRIP;
			lzo_uint overhead = out_len1/64+16+3;
			mesh[i].vertex    = (char *) memalign(64, out_len1);
			lzo_byte *out     = (lzo_byte *) mesh[i].vertex;
			
			lzo_uint in_len;
			fread(&in_len, sizeof(lzo_uint), 1, sf);
			lzo_byte *in      = (lzo_byte *) memalign(64, in_len);
			lzo_byte *wrkmem  = (lzo_byte *) memalign(64, overhead);
			
			lzo_uint out_len2;
			fread(in, 1, in_len, sf);
			lzo1x_decompress(in, in_len, out, &out_len2, NULL);
			
			free(wrkmem);
			free(in);
			
			if(out_len1!=out_len2)
				{
				_log << "Error decompressing stripped file (vertices)\n";
				erase();
				fclose(sf);
				return;
				}
			
			/****************************************************************\
			|* Decompress the normals
			\****************************************************************/
			out_len1       = mesh[i].nfaces * NORM_DATA_LEN_STRIP;
			overhead       = out_len1/64+16+3;
			mesh[i].normal = (char *) memalign(64, out_len1);
			out            = (lzo_byte *) mesh[i].normal;

			fread(&in_len, sizeof(lzo_uint), 1, sf);
			in             = (lzo_byte *) memalign(64, in_len);
			wrkmem         = (lzo_byte *) memalign(64, overhead);
			
			fread(in, 1, in_len, sf);
			lzo1x_decompress(in, in_len, out, &out_len2, NULL);
			free(wrkmem);
			free(in);
			
			if(out_len1!=out_len2)
			    {
				_log << "Error decompressing stripped file (normals)\n";
				erase();
				fclose(sf);
				return;
			    }
			
			/****************************************************************\
			|* Decompress the texture co-ords if they're present
			\****************************************************************/
			if (mesh[i].texture >= 0)
				{
				out_len1    = mesh[i].nfaces * ST_DATA_LEN_STRIP;
				overhead    = out_len1/64+16+3;
				mesh[i].st  = (char *) memalign(64, out_len1);
				out         = (lzo_byte *) mesh[i].st;

				fread(&in_len, sizeof(lzo_uint), 1, sf);
				in          = (lzo_byte *) memalign(64, in_len);
				wrkmem      = (lzo_byte *) memalign(64, overhead);
				
				fread(in, 1, in_len, sf);
				lzo1x_decompress(in, in_len, out, &out_len2, NULL);
				free(wrkmem);
				free(in);
				
				if(out_len1!=out_len2)
					{
					_log << "Error decompressing stripped file (textures)\n";
					erase();
					fclose(sf);
					return;
					}
				}

			// we actually read the number of verts, correct that
			mesh[i].nfaces -= 2;
			nfaces         += mesh[i].nfaces;
			}
			
		fread(&header.magic, 1, 8, sf);
		if (strncmp(header.magic, "STRIPPED", 8) != 0)
			{
			_log << "Unexpected string at the end of stripped file.\n";
			erase();
			fclose(sf);
			type=MODEL_BAD_FORMAT;
			return;
			}

		fclose(sf);
		free(indexes);
		}
	
	_log << "Loading done, " << nmeshes << " meshes, " << nfaces << " polys\n";
	type=MODEL_STRIPPED;
	}
		
/****************************************************************************\
|* Method - load a ms3d file
\****************************************************************************/
void Model::loadMS3D(char *name)
{

	Ms3dFile *TmpMs3dFile;
	TmpMs3dFile = new Ms3dFile();
	int loop1, loop2, loop3;
	_log << "Opened ms3d file.\n";
	if (TmpMs3dFile->Load(name) == false)
	{
	
		_log << "Bad ms3d file.\n";
		erase();
		type=MODEL_BAD_FORMAT;
		return;
	}

	type = MODEL_TRIMODEL;
	mesh=(mesh_t*) memalign(64, TmpMs3dFile->numGroups * sizeof(mesh_t));
	memset(mesh, 0, TmpMs3dFile->numGroups * sizeof(mesh_t));
		nmeshes = TmpMs3dFile->numGroups;
		nfaces = 0;
		
		_log << "MS3D: Number of groups:" << TmpMs3dFile->numGroups << "\n";
		for (loop1=0; loop1<TmpMs3dFile->numGroups; loop1++)
		{
			_log << "MS3D: Group Number:" << loop1 << "\n";

			_log << "MS3D: Number of Triangles:" << TmpMs3dFile->groups[loop1].numTriangles << "\n";
			//log << "MS3D: Number of groups:" << TmpMs3dFile->numGroups << "\n";
			
			mesh[loop1].nfaces = TmpMs3dFile->groups[loop1].numTriangles;
			mesh[loop1].texture = atoi(&(TmpMs3dFile->groups[loop1].materialIndex));
			nfaces = nfaces + mesh[loop1].nfaces;
			//mesh[loop1].rgb = new qword_t;
			mesh[loop1].rgb.f32[0] = 1.0f;
			mesh[loop1].rgb.f32[1] = 1.0f;
			mesh[loop1].rgb.f32[2] = 1.0f;
			mesh[loop1].rgb.f32[3] = 1.0f;
			/****************************************************************\
			|* Decompress the vertex strip
			\****************************************************************/
			mesh[loop1].vertex = (char *) memalign(64, (mesh[loop1].nfaces + 2) * 3 * VERT_DATA_LEN_TRI + 2);
			mesh[loop1].normal = (char *) memalign(64, (mesh[loop1].nfaces + 2) * 3 *  NORM_DATA_LEN_TRI);
			memset(mesh[loop1].vertex, 0, (mesh[loop1].nfaces + 2) * 3 *  VERT_DATA_LEN_TRI);
			memset(mesh[loop1].normal, 0, (mesh[loop1].nfaces + 2) * 3 *  NORM_DATA_LEN_TRI);
			if (mesh[loop1].texture >= 0)
			{
				mesh[loop1].st  = (char *) memalign(64, mesh[loop1].nfaces * ST_DATA_LEN_TRI);
			}
			int vertexcounter = 0;
			int uvcounter = 0;
			for (loop2 = 0; loop2 < TmpMs3dFile->groups[loop1].numTriangles ; loop2++)
			{
				int triangleIndex = TmpMs3dFile->groups[loop1].triangleIndices[loop2];
				const MS3D_TRIANGLE* tri = &TmpMs3dFile->triangles[triangleIndex];
				
				for (loop3 = 0; loop3 < 3; loop3++)
				{
					_log << "MS3D: Vertex #:" << loop2 << "\n";

					int vertexindex = tri->vertexIndices[loop3];
 					((qword_t*)(mesh[loop1].vertex +((loop2 * 3) + loop3) * VERT_DATA_LEN_STRIP))->fvect[0] = TmpMs3dFile->vertices[vertexindex].vertex[0];  
					((qword_t*)(mesh[loop1].vertex +((loop2 * 3) + loop3) * VERT_DATA_LEN_STRIP))->fvect[1] = TmpMs3dFile->vertices[vertexindex].vertex[1];
  					((qword_t*)(mesh[loop1].vertex +((loop2 * 3) + loop3) * VERT_DATA_LEN_STRIP))->fvect[2] = TmpMs3dFile->vertices[vertexindex].vertex[2];  


 					((qword_t*)(mesh[loop1].normal +((loop2 * 3) + loop3) * VERT_DATA_LEN_STRIP))->fvect[0] = tri->vertexNormals[loop3][0];  
					((qword_t*)(mesh[loop1].normal +((loop2 * 3) + loop3) * VERT_DATA_LEN_STRIP))->fvect[1] = tri->vertexNormals[loop3][1];
  					((qword_t*)(mesh[loop1].normal +((loop2 * 3) + loop3) * VERT_DATA_LEN_STRIP))->fvect[2] = tri->vertexNormals[loop3][2];  

					//mesh[loop1].vertex + (vertexcounter * 4) + loop3 = (char)TmpMs3dFile->vertices[vertexindex].vertex;
					//mesh[loop1].normal[vertexcounter] = (char)tri->vertexNormals[loop3];
					_log << "MS3D: Vertex X Y Z:" << TmpMs3dFile->vertices[vertexindex].vertex[0] << " " <<  TmpMs3dFile->vertices[vertexindex].vertex[1] << " " << TmpMs3dFile->vertices[vertexindex].vertex[2] << "\n"; 
					
					//if (mesh[loop1].texture >=0)
					//{
					//	mesh[loop1].st[uvcounter] = (char)tri->u[loop3];
					//	uvcounter++;
					//	mesh[loop1].st[uvcounter] = (char)tri->v[loop3];
					//	uvcounter++;

					//}
					

					vertexcounter++;
				}



			}

		}
			
			
	delete TmpMs3dFile;


}


/****************************************************************************\
|* Method - build a VIF packet for the model. Needs to be called whenever
|* the structure of the model changes (not it's orientation)
\****************************************************************************/
void Model::buildPacket(void)
	{
	unsigned int mesh_verts;
	unsigned int m, v, cnt1;

	int tags=0;
	int packet_size=10;
	if (type == MODEL_STRIPPED)
	{
		for (m=0; m<nmeshes; m++)
			{
			packet_size+=10;

			mesh_verts=mesh[m].nfaces+2;
			for (v=0; v<mesh_verts; v+=VU_VERTS_STRIP)
				packet_size+=VU_VERTS_STRIP+VU_VERTS_STRIP*2*3/4+4;
			}
	}
	else if (type == MODEL_TRIMODEL)
	{
		_log << "Triangle Model Type: " << type << "\n";
		for (m=0; m<nmeshes; m++)
			{
			packet_size+=10;

			mesh_verts=mesh[m].nfaces * 3;
			for (v=0; v<mesh_verts; v+=VU_VERTS_TRI)
				packet_size+=VU_VERTS_TRI+VU_VERTS_TRI*2*3/4+4;
			}
	}


	_log << " .. allocating " << packet_size << " qwords (" 
	     << packet_size*16 << " bytes)\n";

	if (vpkt.buffer != NULL)
		free(vpkt.buffer);
	vpkt.buffer = NULL;
	vpkt.buffer = (u_long128 *) memalign(64, 16 * packet_size);

	// init memory for data VIF PKT
	vpkt.vp.init(vpkt.buffer);
	vpkt.vp.end(0);
	tags++;

	if (type == MODEL_BAD_FORMAT)
		{
		_log << "bad model format\n";
		vpkt.vp.terminate();
		return;
		}
		
	_log << "good model format\n";

	vpkt.vp.addCode(0x3000000); // set vu1 base
	vpkt.vp.addCode(0x2000200); // set vu1 offset

	vpkt.vp.openDirectCode(0);
	vpkt.vp.addq(GIF_SET_TAG(1,1,0,0,GIF_PACKED,4));
	vpkt.vp.addq(0xEEEEL);
	vpkt.vp.addq(PS2_GS_SETREG_ALPHA( 0,2,2,2,0xA0));
	vpkt.vp.addq(PS2_GS_ALPHA_1);
	vpkt.vp.addq(PS2_GS_SETREG_PABE( 1 ));
	vpkt.vp.addq(PS2_GS_PABE);
	vpkt.vp.addq(PS2_GS_SETREG_FBA( 0 ));
	vpkt.vp.addq(PS2_GS_FBA_1);
	vpkt.vp.addq(PS2_GS_SETREG_CLAMP(0, 0, 0, 0, 0, 0));
	vpkt.vp.addq(PS2_GS_CLAMP_1);
	vpkt.vp.closeDirectCode();
	int prim_type;
	if (type == MODEL_STRIPPED)
	{
		prim_type=PS2_GS_SETREG_PRIM(PS2_GS_PRIM_PRIM_TRISTRIP, GOURAUD(1),
	                             TEXTURED(0), 0, 0, 0, 0, 0, 0 );
	}
	else if (type == MODEL_TRIMODEL)
	{
		prim_type=PS2_GS_SETREG_PRIM(PS2_GS_PRIM_PRIM_TRIANGLE, GOURAUD(1),
	                             TEXTURED(0), 0, 0, 0, 0, 0, 0 );
	}
	vpkt.vp.openUpkCode((0 | 0x8000), V4_32, 4, 4 );

	localScreen = vpkt.vp.current;
	for (cnt1=0; cnt1<4; cnt1++)
		vpkt.vp.add2q(0);	// 128-bit add (2 qwords)

	localLight = vpkt.vp.current;
	for (cnt1=0; cnt1<4; cnt1++)
		vpkt.vp.add2q(0);	// 128-bit add (2 qwords)

	lightColour = vpkt.vp.current;
	for (cnt1=0; cnt1<4; cnt1++)
		vpkt.vp.add2q(0);	// 128-bit add (2 qwords)

	vpkt.vp.closeUpkCode();
	
	vpkt.vp.add(VIF1_SET_MSCALF(0,0));

	for (m=0; m<nmeshes; m++)
		{

		int pre=1;
		if (type == MODEL_STRIPPED)
		{
			mesh_verts=mesh[m].nfaces+2;

			for (v=0; v<mesh_verts; v+=VU_VERTS_STRIP)
			{
				unsigned int vu_verts=VU_VERTS_STRIP;
	
				if (v + VU_VERTS_STRIP > mesh_verts)
					vu_verts = mesh_verts % VU_VERTS_STRIP; // -v;

				vpkt.vp.openUpkCode((0|0x8000), V4_32, 4, 4);
				vpkt.vp.addq(PS2_GIFTAG_SET_TOPHALF( NLOOP(vu_verts), EOP(1),
			                                    PRE(pre), prim_type,
												PS2_GIFTAG_FLG_PACKED, 2));
				vpkt.vp.addq(0x41L);
				vpkt.vp.closeUpkCode();

				vpkt.vp.openUpkCode((1|0x8000), V3_32, 4, 4);
				vpkt.vp.add2q((u_long128*)(mesh[m].vertex +v * VERT_DATA_LEN_STRIP),
			                           VU_VERTS_STRIP*3/4);
				vpkt.vp.closeUpkCode();

				vpkt.vp.openUpkCode((101|0x8000), V3_32, 4, 4);
				vpkt.vp.add2q((u_long128*)(mesh[m].normal +v * VERT_DATA_LEN_STRIP),
			 							VU_VERTS_STRIP*3/4);
				vpkt.vp.closeUpkCode();

				vpkt.vp.openUpkCode((201|0x8000), V4_32, 4, 4);
				for (cnt1=0; cnt1<VU_VERTS_STRIP; cnt1++)
					vpkt.vp.add2q(*((u_long128*)(&(mesh[m].rgb))));
				vpkt.vp.closeUpkCode();

				vpkt.vp.addCode(VIF1_SET_MSCNT(0));
				pre=0;
				
			}
		}
		else if (type == MODEL_TRIMODEL)
		{				
			mesh_verts=mesh[m].nfaces * 3;

			for (v=0; v<mesh_verts; v+=VU_VERTS_TRI)
				{
				unsigned int vu_verts=VU_VERTS_TRI;
	
				if (v + VU_VERTS_TRI > mesh_verts)
					vu_verts = mesh_verts % VU_VERTS_TRI; // -v;

				vpkt.vp.openUpkCode((0|0x8000), V4_32, 4, 4);
				vpkt.vp.addq(PS2_GIFTAG_SET_TOPHALF( NLOOP(vu_verts), EOP(1),
			                                    PRE(pre), prim_type,
												PS2_GIFTAG_FLG_PACKED, 2));
				vpkt.vp.addq(0x41L);
				vpkt.vp.closeUpkCode();

				vpkt.vp.openUpkCode((1|0x8000), V3_32, 4, 4);
				vpkt.vp.add2q((u_long128*)(mesh[m].vertex +v * VERT_DATA_LEN_TRI),
			                           VU_VERTS_TRI*3/4);
				vpkt.vp.closeUpkCode();

				vpkt.vp.openUpkCode((101|0x8000), V3_32, 4, 4);
				vpkt.vp.add2q((u_long128*)(mesh[m].normal +v * VERT_DATA_LEN_TRI),
			 							VU_VERTS_TRI*3/4);
				vpkt.vp.closeUpkCode();

				vpkt.vp.openUpkCode((201|0x8000), V4_32, 4, 4);
				for (cnt1=0; cnt1<VU_VERTS_TRI; cnt1++)
					vpkt.vp.add2q(*((u_long128*)(&(mesh[m].rgb))));
				vpkt.vp.closeUpkCode();

				vpkt.vp.addCode(VIF1_SET_MSCNT(0));
				pre=0;
				}
			}
		}
	
	vpkt.vp.terminate();
	
	_log << "finished packet: " << vpkt.vp.size() << " qwords, "
	     << tags << " tags\n";
	//vpkt.vp.dump();
	}
			
/****************************************************************************\
|* Method - draw the model onto the screen
\****************************************************************************/
void Model::draw(Framework *f)
	{
	f->dmaStartOnVu1(NULL, (ps2_dmatag*)vpkt.vp.base);
	}
	
