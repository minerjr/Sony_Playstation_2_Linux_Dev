#include "custommodel.h"
#include "addregisters.h"
#include "ps2defines.h"
#include "texture.h"

Custom_Model::Custom_Model(Matrix_Manager *tmpMatrixManager, int tmpChannel,Dma_Memory_Engine *DmaMemoryEngine, Texture_Engine *tmpTextureEngine)
{
	MatrixManager = tmpMatrixManager;
	//If there is no matrix manager exit, there needs to be one for the custom model to work
	if (MatrixManager == NULL)return;

	TextureEngine = tmpTextureEngine;
	if (TextureEngine == NULL)return;

	addCount = 0;

	channel = tmpChannel;

	addFlg = 0;
	addChangeValue = 60 + 1;
	
	modelGifHeader = NULL;

	modelVertexHeader = NULL;
	modelNormalHeader = NULL;
	modelColorHeader = NULL;
	modelSTHeader = NULL;

	//If the user entered a valid channel then
	if (channel == CHANNEL1 || channel == CHANNEL2)
	{
		//Create a new dma packet
		modelPacket = new Dma_Packet(1,channel, 0, DmaMemoryEngine);
	}
	else
	{
		//Else set the dma packet to NULL
		modelPacket = NULL;
	}

}

Custom_Model::~Custom_Model()
{
	if (modelPacket)
		delete modelPacket;
	
}

Dma_Packet *Custom_Model::getModelPacket()
{
	return modelPacket;
}


/*

//This is a test packet function
void Custom_Model::createDummy()
{
	sps2VIFCodeNOP_t vifNOP;

	vifNOP.i32 = 0;

	vifNOP.s.CMD = VIF_CMD_NOP;

	//modelPacket->reset();


	for (int x = 0; x < 132; x++)
	{
		modelPacket->add((uint32)vifNOP.i32);
	}


	modelPacket->finish();

}

*/

int Custom_Model::begin(int tmpMode, int tmpDataType)
{
	int oldMode;
	matrixf workMatrix;
	qword_t workVector;

	sps2GIFTag_t GifTag;
	sps2GIFPackedRegister_t Register;
	sps2GIFRegPRIM_t Prim;

	sps2VIFCodeFLUSH_t vifFlush;
	sps2VIFCodeBASE_t vifBase;
	sps2VIFCodeOFFSET_t vifOffset;
	sps2VIFCodeMSCALF_t vifMSCalf;

	sps2VIFCodeSTCYCL_t vifSTcycl;
	sps2VIFCodeUNPACK_t vifUnpack;
	
	//uint32 *tmpHeader = NULL;

	mode = tmpMode;
	dataType = tmpDataType;


	addCount = 0;
	addFlg = 0;
	if (modelPacket == NULL)return 0;

	if ((dataType & NORESET_T) != NORESET_T)modelPacket->reset();
	//tmpHeader = (uint32 *)modelPacket->getCurrent();
	//tmpHeader -= 2;
		
	if ((dataType & ST_T) == ST_T && channel == CHANNEL2)
	{

		uint32 textureLocationID = TextureEngine->getTexture(textureID)->textureLocationID;
		uint32 clutDst = sps2UScreenGetFirstFreeGSPage() << 5;

			//Calculate the number of blocks the clut takes and set
			//the next texture address to it.
			//Cacluation works in this formula.
			//((texture width + block width - 1) / block width) * 
			//	((texture height + block height - 1) / block height)
			//32 bit textures uses 8 for the block height and width.
			//The clut is a 16 by 16 32bit image.
		uint32 textureDst = clutDst + ((16 + 8 - 1) / 8) * ((16 + 8 - 1) / 8);
		//Calculate  vram locations for the clut and the texture data
		for (unsigned int x = 0; x < textureLocationID; x++)
		{
			//256 Color clut calculation
			//8bit textures use 16 for the block height and width
			clutDst = textureDst + ((256 + 16 - 1) / 16) * ((256 + 16 - 1) / 16);
			textureDst = clutDst + ((16 + 8 - 1) / 8) * ((16 + 8 -1) / 8);
		}

		if (channel == CHANNEL1)
		{
		//	printf("Called directStart\n");
			modelPacket->startDirect();
		}
		//Gif tag for setting the actual use registers
		GifTag.i128 = 0;
		GifTag.s.NLOOP = 3;
		GifTag.s.PRE = 0;
		GifTag.s.EOP = 0;
		GifTag.s.PRIM = 0;
		GifTag.s.FLG = GIF_FLG_PACKED;
		GifTag.s.NREG = 1;
		GifTag.s.REGS = GIF_REG_A_D;
		modelPacket->add((uint128)GifTag.i128);

		Register.i128 = 0;
		Register.A_D.s.DATA = ALPHA_SET(0, 1, 0, 1, 128);
		Register.A_D.s.ADDR = GIF_A_D_REG_ALPHA_1;
		modelPacket->add((uint128)Register.i128);


		Register.i128 = 0;
		Register.A_D.s.DATA = TEX1_SET(1, 0, 1, 1, 0, 0, 0);
		Register.A_D.s.ADDR = GIF_A_D_REG_TEX1_1;
		modelPacket->add((uint128)Register.i128);


		Register.i128 = 0;
		Register.A_D.s.DATA = TEX0_SET(textureDst, 4, PSM_PSMT8, 8, 8, 1, 0, clutDst, 0, 0, 0, 1);
		Register.A_D.s.ADDR = GIF_A_D_REG_TEX0_1;
		modelPacket->add((uint128)Register.i128);
		if (channel == CHANNEL1)modelPacket->finishDirect();
	}

  
	
	Prim.i64 = 0;
	Prim.s.IIP = 1;
		
	//If the mode is not valid then
	if (!(mode >= 0 && mode <= 6))return 0;
	if ((dataType & ALPHA_T) == ALPHA_T)
	{
		Prim.i64 = GS_PRIM(mode, 
						PRIM_IIP_GOURAUD, 
						PRIM_TME_OFF,
						PRIM_FGE_OFF, 
						PRIM_ABE_ON, 
						PRIM_AA1_OFF, 
						PRIM_FST_UV, 
						PRIM_CTXT_CONTEXT1, 
						PRIM_FIX_NOFIXDDA);
	}
	else
	{
		Prim.i64 = GS_PRIM(mode, 
						PRIM_IIP_GOURAUD, 
						PRIM_TME_OFF,
						PRIM_FGE_OFF, 
						PRIM_ABE_OFF, 
						PRIM_AA1_OFF, 
						PRIM_FST_UV, 
						PRIM_CTXT_CONTEXT1, 
						PRIM_FIX_NOFIXDDA);
	}
	if ((dataType & ST_T) == ST_T)
	{
		Prim.s.TME = PRIM_TME_ON;
		//Prim.s.ABE = PRIM_ABE_ON;
	}
	

	GifTag = AddGifTag(0, 1, 1, 0, GIF_FLG_PACKED, 0);
	GifTag.s.PRIM = Prim.i64;

	if ((dataType & ST_T) == ST_T)
	{
		SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_UV);
	}
	if ((dataType & COLOR_T) == COLOR_T)
	{
		SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
	}
	if ((dataType & VERTEX_T) == VERTEX_T)
	{
		SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
	}
	
	if (channel == CHANNEL1)
	{
		//Do the screen matrix

		//Vu1 base and offset (change after to readable values)
		vifFlush.i32 = 0;
		vifFlush.s.CMD = VIF_CMD_FLUSH;

		vifBase.i32 = 0;
		vifBase.s.BASE = 0;
		vifBase.s.CMD = VIF_CMD_BASE;

		vifOffset.i32 = 0;
		vifOffset.s.OFFSET = 512;
		vifOffset.s.CMD = VIF_CMD_OFFSET;

		modelPacket->add((uint32)vifFlush.i32);

		//Add a vif base
		
		modelPacket->add((uint32)vifBase.i32);
		
		//Add a vif offset
		
		modelPacket->add((uint32)vifOffset.i32);
	
		
		vifSTcycl.i32 = 0;
		vifSTcycl.s.CMD = VIF_CMD_STCYCL;
		vifSTcycl.s.CL = 4;
		vifSTcycl.s.WL = 4;
		
		modelPacket->add((uint32)vifSTcycl.i32);

		//Setup to unpack data into the vu memory (3 matrixs)
		vifUnpack.i32 = 0;
		vifUnpack.s.CMD = VIF_CMD_UNPACK;
		vifUnpack.s.FORMAT = VIF_UNPACK_V4_32;
		vifUnpack.s.ADDR = 32768;
		vifUnpack.s.FLG = 1;
		vifUnpack.s.SIZE = 13;//12x 128 bit values

		modelPacket->add((uint32)vifUnpack.i32);

		//Start the update from before the matrixs are added
		modelPacket->updateSetup();

		if ((dataType & DONTAPPLYMODELCAMERA_T) == DONTAPPLYMODELCAMERA_T)
		{
 			//Get the projection matrix and store it
			oldMode = MatrixManager->setMode(PROJECTION);
			workMatrix = *MatrixManager->getMatrix();

		}
		else
		{
 			//Get the model view matrix and store it
			oldMode = MatrixManager->setMode(MODELVIEW);
			workMatrix = *MatrixManager->getMatrix();

			/*
			//Get the camera matrix and apply it to the model view matrix and store it.
			MatrixManager->setMode(CAMERA);
			
			ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
			*/
			//Multiply the projection onto the workMatrix
			MatrixManager->setMode(PROJECTION);
			ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
		}
		

		//Add the work screen (view * projection) matrix to the packet
		modelPacket->add((uint128 *)&workMatrix, 4);

		

		//Do the light matrix

		//Get the model view matrix and store it
		MatrixManager->setMode(MODELVIEW);
		workMatrix = *MatrixManager->getMatrix();

		/*
		//Get the camera matrix and apply it to the model view matrix and store it.
		MatrixManager->setMode(CAMERA);
		ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);
		*/
		//Remove the tanslation from the matrix, just need the rotations
		workMatrix[3][0] = 0.0f;
		workMatrix[3][1] = 0.0f;
		workMatrix[3][2] = 0.0f;

		//Now apply the world view matrix to the light matrix and store it
		MatrixManager->setMode(LIGHT);
		ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);


		modelPacket->add((uint128 *)&workMatrix, 4);

		//Add the light color matrix

		//Get the model view matrix and store it
		MatrixManager->setMode(COLOR);
		workMatrix = *MatrixManager->getMatrix();

		//Add the work color matrix to the packet
		//ps2_vu0_transpose_matrix(workMatrix, workMatrix);
		modelPacket->add((uint128 *)&workMatrix, 4);


		//Set the matrix manager back to the orginal mode
		MatrixManager->setMode(oldMode);

		ps2_vu0_copy_vector(workVector.fvect, MatrixManager->getProjectionScale());
		modelPacket->add(workVector.ul128);
		

		vifMSCalf.i32 = 0;
		vifMSCalf.s.CMD = VIF_CMD_MSCALF;
		vifMSCalf.s.EXECADDR = 0;
		modelPacket->add((uint32)vifMSCalf.i32);

		modelPacket->add((uint32)vifSTcycl.i32);


		vifUnpack.s.SIZE = 1;//1x 128 bit values
		modelPacket->add((uint32)vifUnpack.i32);


	}

	modelGifHeader = modelPacket->getCurrent();

	modelPacket->add((uint128)GifTag.i128);
	
	return 1;
}

int Custom_Model::end()
{
	sps2VIFCodeMSCNT_t vifMSCnt;
	sps2VIFCodeFLUSHE_t	vifMSFlushE;

	if (modelGifHeader == NULL)return 0;

	//If using channel 2 then set the gif header size
	if (channel == CHANNEL2)
		((sps2GIFTag_t *)modelGifHeader)->s.NLOOP = addCount;
	//Else if using channel 1 then set the color header size to addcount / 4
	else if (channel == CHANNEL1)
	{
		//Add cnt vif flag to actually execute the program
		vifMSCnt.i32 = 0;
		vifMSCnt.s.CMD = VIF_CMD_MSCNT;
		vifMSFlushE.i32 = 0;
		vifMSFlushE.s.CMD = VIF_CMD_FLUSHE;
		modelPacket->add((uint32)vifMSCnt.i32);
		modelPacket->add((uint32)vifMSFlushE.i32);
		//printf("Finish AddCount: %d\n", addCount);
		if (modelSTHeader != NULL)((sps2VIFCodeUNPACK_t *)modelSTHeader)->s.SIZE = addCount;
	}

	addFlg = 0;
	
	modelPacket->finish();

	return 1;
}

int Custom_Model::setAddChangeValue(int newAddChangeValue)
{
	//Remeber to add one to change after the added values
	if (newAddChangeValue > 61)return -1;
	else if (newAddChangeValue < 2)return -1;
	
	addChangeValue = newAddChangeValue;
	return 1;
}

int Custom_Model::doVuAdd(int flg)
{
	sps2GIFTag_t GifTag;
	sps2GIFRegPRIM_t Prim;
	sps2VIFCodeUNPACK_t vifUnpack;

	sps2VIFCodeMSCNT_t vifMSCnt;

	sps2VIFCodeSTCYCL_t vifSTcycl;

	switch(flg)
	{
	case VERTEX_T:
		//IF there are 60 vertex's in the packet then
		if ((addFlg & VERTEX_T) == 0)
		{
			//printf("AddCount: 0\n");
			addFlg = addFlg | VERTEX_T;

			vifSTcycl.i32 = 0;
			vifSTcycl.s.CMD = VIF_CMD_STCYCL;
			vifSTcycl.s.CL = 4;
			vifSTcycl.s.WL = 4;
			
			modelPacket->add((uint32)vifSTcycl.i32);

			//Add vertex unpack command
			vifUnpack.i32 = 0;
			vifUnpack.s.CMD = VIF_CMD_UNPACK;
			vifUnpack.s.FORMAT = VIF_UNPACK_V3_32;
			vifUnpack.s.ADDR = 32769;
			vifUnpack.s.FLG = 1;
			vifUnpack.s.SIZE = 1;//1x 128 bit values
			
			modelVertexHeader = modelPacket->getCurrent();
			
			modelPacket->add((uint32)vifUnpack.i32);
			//addCount = 0;

		}
		else if (addCount % addChangeValue == 0 && addCount != 0)
		{
			//printf("AddCount: %d\n", addCount);
			//Set the add count of the orginal gif tag
			((sps2VIFCodeUNPACK_t *)modelSTHeader)->s.SIZE = addCount - 1;
			//Reset the add count
			addCount = 1;
			addFlg = VERTEX_T;

			//Add cnt vif flag to actually execute the program
			vifMSCnt.i32 = 0;
			vifMSCnt.s.CMD = VIF_CMD_MSCNT;
			modelPacket->add((uint32)vifMSCnt.i32);
			

			Prim.i64 = 0;
			Prim.s.IIP = 1;
			
			//If the mode is not valid then
			if (!(mode >= 0 && mode <= 6))return 0;
			Prim.i64 = GS_PRIM(mode, 
								PRIM_IIP_GOURAUD, 
								PRIM_TME_OFF,
								PRIM_FGE_OFF, 
								PRIM_ABE_OFF, 
								PRIM_AA1_OFF, 
								PRIM_FST_UV, 
								PRIM_CTXT_CONTEXT1, 
								PRIM_FIX_NOFIXDDA);
			if ((dataType & ST_T) == ST_T)
			{
				Prim.s.TME = PRIM_TME_ON;
				//Prim.s.ABE = PRIM_ABE_ON;
			}
			

			GifTag = AddGifTag(0, 1, 1, 0, GIF_FLG_PACKED, 0);
			GifTag.s.PRIM = Prim.i64;

			if ((dataType & ST_T) == ST_T)
			{
				SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_UV);
			}
			if ((dataType & COLOR_T) == COLOR_T)
			{
				SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
			}
			if ((dataType & VERTEX_T) == VERTEX_T)
			{
				SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
			}
			
			//Set the add size of the vif unpack to the add count / 3 for how many
			//packs to send
			//((sps2VIFCodeUNPACK_t *)modelVertexHeader)->s.SIZE = addCount;
			
			vifSTcycl.i32 = 0;
			vifSTcycl.s.CMD = VIF_CMD_STCYCL;
			vifSTcycl.s.CL = 4;
			vifSTcycl.s.WL = 4;
			
			modelPacket->add((uint32)vifSTcycl.i32);

			//Setup to unpack data into the vu memory gif tag
			vifUnpack.i32 = 0;
			vifUnpack.s.CMD = VIF_CMD_UNPACK;
			vifUnpack.s.FORMAT = VIF_UNPACK_V4_32;
			vifUnpack.s.ADDR = 32768;
			vifUnpack.s.FLG = 1;
			vifUnpack.s.SIZE = 1;//1x 128 bit values

			//Add gif unpack command
			
			modelPacket->add((uint32)vifUnpack.i32);
			modelGifHeader = modelPacket->getCurrent();

			modelPacket->add((uint128)GifTag.i128);

			//Add vertex unpack command
			vifUnpack.s.FORMAT = VIF_UNPACK_V3_32;
			vifUnpack.s.ADDR = 32769;

			vifSTcycl.i32 = 0;
			vifSTcycl.s.CMD = VIF_CMD_STCYCL;
			vifSTcycl.s.CL = 4;
			vifSTcycl.s.WL = 4;
			
			modelPacket->add((uint32)vifSTcycl.i32);
			
			modelVertexHeader = modelPacket->getCurrent();
			
			modelPacket->add((uint32)vifUnpack.i32);

		}
		
		break;
	case NORMAL_T:
		//If its the first call for normal then setup the first the normal vu unpack tag
		if ((addFlg & NORMAL_T) == 0)
		{
			//printf("Normal AddCount: %d\n", addCount);
			((sps2GIFTag_t *)modelGifHeader)->s.NLOOP = addCount;
			((sps2VIFCodeUNPACK_t *)modelVertexHeader)->s.SIZE = addCount;

			addFlg = addFlg | NORMAL_T;

			vifSTcycl.i32 = 0;
			vifSTcycl.s.CMD = VIF_CMD_STCYCL;
			vifSTcycl.s.CL = 4;
			vifSTcycl.s.WL = 4;
			
			modelPacket->add((uint32)vifSTcycl.i32);

			//Add vertex unpack command
			vifUnpack.i32 = 0;
			vifUnpack.s.CMD = VIF_CMD_UNPACK;
			vifUnpack.s.FORMAT = VIF_UNPACK_V3_32;
			vifUnpack.s.ADDR = 32829;
			vifUnpack.s.FLG = 1;
			vifUnpack.s.SIZE = 1;//1x 128 bit values

			modelNormalHeader = modelPacket->getCurrent();
			
			modelPacket->add((uint32)vifUnpack.i32);
			addCount = 0;
		}
		addCount++;
		break;

	case COLOR_T:
		if ((addFlg & COLOR_T) == 0)
		{
			//printf("Color AddCount: %d\n", addCount);
			((sps2VIFCodeUNPACK_t *)modelNormalHeader)->s.SIZE = addCount;

			addFlg = addFlg | COLOR_T;

			vifSTcycl.i32 = 0;
			vifSTcycl.s.CMD = VIF_CMD_STCYCL;
			vifSTcycl.s.CL = 4;
			vifSTcycl.s.WL = 4;
			
			modelPacket->add((uint32)vifSTcycl.i32);

			//Add vertex unpack command
			vifUnpack.i32 = 0;
			vifUnpack.s.CMD = VIF_CMD_UNPACK;
			vifUnpack.s.FORMAT = VIF_UNPACK_V4_32;
			vifUnpack.s.ADDR = 32889;
			vifUnpack.s.FLG = 1;
			vifUnpack.s.SIZE = 1;//1x 128 bit values

			modelColorHeader = modelPacket->getCurrent();
			
			modelPacket->add((uint32)vifUnpack.i32);
			addCount = 0;
		}
		addCount++;
		break;
	case ST_T:
		if ((addFlg & ST_T) == 0)
		{
			//printf("ST AddCount: %d\n", addCount);
			((sps2VIFCodeUNPACK_t *)modelColorHeader)->s.SIZE = addCount;

			addFlg = addFlg | ST_T;

			vifSTcycl.i32 = 0;
			vifSTcycl.s.CMD = VIF_CMD_STCYCL;
			vifSTcycl.s.CL = 4;
			vifSTcycl.s.WL = 4;
			
			modelPacket->add((uint32)vifSTcycl.i32);

			//Add vertex unpack command
			vifUnpack.i32 = 0;
			vifUnpack.s.CMD = VIF_CMD_UNPACK;
			vifUnpack.s.FORMAT = VIF_UNPACK_V2_32;
			vifUnpack.s.ADDR = 32949;
			vifUnpack.s.FLG = 1;
			vifUnpack.s.SIZE = 1;//1x 128 bit values

			modelSTHeader = modelPacket->getCurrent();
			
			modelPacket->add((uint32)vifUnpack.i32);
			addCount = 0;
		}
		addCount++;
		break;
	}

	return 1;
}

int Custom_Model::addVertex(float verX, float verY, float verZ, float verW)
{
	vectorf workVector;
	int oldMode;

	//If there is no model packet, return 0
	if (modelPacket == NULL)return 0;

	workVector[0] = verX;
	workVector[1] = verY;
	workVector[2] = verZ;
	workVector[3] = verW;
	
	//Increase the vertex count by 1
	addCount++;
	//If channel 2 is used, then apply the projection, perspective correction and view matrix 
	if (channel == CHANNEL2)
	{

		//Get the current matrix mode, store it and also set the mode to model view
		oldMode = MatrixManager->setMode(MODELVIEW);
		//Apply the current model view matrix to the inputed vertex
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		//printf("Transformed Model: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);

		/*
		//Apply the current camera matrix
		MatrixManager->setMode(CAMERA);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		//printf("Transformed CAMERA: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);
		*/

		
		//Apply the current projection matrix
		MatrixManager->setMode(PROJECTION);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		//printf("Transformed PROJECTION: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);

		
		//Homogenos Division (ie divide xyz by w)
		ps2_vu0_div_vector_xyz(workVector, workVector, workVector[3]);
		workVector[3] = 1.0f;

		ps2_vu0_mul_vector(workVector, workVector, MatrixManager->getProjectionScale());
		
		ps2_vu0_add_vector(workVector, workVector, MatrixManager->getProjectionScale());
		//printf("X %f Y %f Z %f z%f\n", workVector[0], workVector[1], workVector[2], workVector[3]);
		workVector[3] = 1.0f;
		

		
		//Add the new transformed vertex to the model packet
		modelPacket->add((uint128)AddVertexFPo((int)(workVector[0]) << 4, (int)(workVector[1])  << 4, (int)(workVector[2])  << 4, 0).i128 );

		//Reset the matrix mode back to the old mode
		MatrixManager->setMode(oldMode);


	}
	else if (channel == CHANNEL1)
	{
		doVuAdd(VERTEX_T);
		modelPacket->add((uint32 *)&workVector, 3);
	}
	

	return 1;
}

int Custom_Model::addVertex(vectorf *vertexData)
{
	int oldMode;

	//If there is no model packet, return 0
	if (modelPacket == NULL)return 0;

		//Increase the vertex count by 1
		addCount++;

	//If channel 2 is used, then apply the projection, perspective correction and view matrix 
	if (channel == CHANNEL2)
	{

		//Get the current matrix mode, store it and also set the mode to model view
		oldMode = MatrixManager->setMode(MODELVIEW);
		//Apply the current model view matrix to the inputed vertex
		ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
		//printf("Transformed Model: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);

		/*
		//Apply the current camera matrix
		MatrixManager->setMode(CAMERA);
		ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
		//printf("Transformed CAMERA: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);
		*/

		
		//Apply the current projection matrix
		MatrixManager->setMode(PROJECTION);
		ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
		//printf("Transformed PROJECTION: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);

	
		//Homogenos Division (ie divide xyz by w)
		ps2_vu0_div_vector_xyz(*vertexData, *vertexData, *vertexData[3]);
		*vertexData[3] = 1.0f;
		//printf("Transformed CORECTION: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);
		
		//Apply the projection scale value
		ps2_vu0_mul_vector(*vertexData, *vertexData, MatrixManager->getProjectionScale());

		ps2_vu0_add_vector(*vertexData, *vertexData, MatrixManager->getProjectionScale());

		*vertexData[3] = 1.0f;
			
		//Add the new transformed vertex to the model packet
		modelPacket->add((uint128)AddVertexFPo((int)(*vertexData[0]) << 4, (int)(*vertexData[1]) << 4, (int)(*vertexData[2]) << 4, 0).i128 );

		//Reset the matrix mode back to the old mode
		MatrixManager->setMode(oldMode);


	}
	else if (channel == CHANNEL1)
	{
		doVuAdd(VERTEX_T);
		modelPacket->add((uint32 *)vertexData, 3);
	}

	return 1;

}

int Custom_Model::addColor(float red, float green, float blue, float alpha)
{
	vectorf RGB;
	matrixf worldView;
	vectorf work;

	int oldMode;

	RGB[0] = red;
	RGB[1] = green;
	RGB[2] = blue;
	RGB[3] = alpha;

	if (channel == CHANNEL2 && (dataType & NORMAL_T) == NORMAL_T)
	{

		//Get the model view matrix and store it
		oldMode = MatrixManager->setMode(MODELVIEW);
		worldView = *MatrixManager->getMatrix();

		/*
		//Get the camera matrix and apply it to the model view matrix and store it.
		MatrixManager->setMode(CAMERA);
		ps2_vu0_mul_matrix(worldView, *MatrixManager->getMatrix(), worldView);
		*/
		//Remove the tanslation from the matrix, just need the rotations
		worldView[3][0] = 0.0f;
		worldView[3][1] = 0.0f;
		worldView[3][2] = 0.0f;

		//Now apply the world view matrix to the light matrix and store it
		MatrixManager->setMode(LIGHT);
		ps2_vu0_mul_matrix(worldView, *MatrixManager->getMatrix(), worldView);

		//Get the stored normal from the matrix class
		work = normal;
		
		//Send the normal throught the calculated matrix
		ps2_vu0_apply_matrix(work,worldView, work);

		
		//Set negative values to 0 (vu maxx)
		if (work[0] < 0.0f)work[0] = 0.0f;
		if (work[1] < 0.0f)work[1] = 0.0f;
		if (work[2] < 0.0f)work[2] = 0.0f;
		//Must set the work vectors w to 1.0f to color properly
		work[3] = 1.0f;
		

		//Apply the work transformed vector through the color matrix
		MatrixManager->setMode(COLOR);		
		ps2_vu0_apply_matrix(work, *MatrixManager->getMatrix(), work);

				//Set the color to be from 0 to 255 
		RGB[3] = alpha;
		RGB[0] *= 255.0f;
		RGB[1] *= 255.0f;
		RGB[2] *= 255.0f;
		RGB[3] *= 255.0f;

		//Multiply the RGB by the light and store it in RGB
		ps2_vu0_mul_vector(RGB, RGB, work);

		//Set the maximum to 1.0f
		if (RGB[0] > 255.0f)RGB[0] = 255.0f;
		if (RGB[1] > 255.0f)RGB[1] = 255.0f;
		if (RGB[2] > 255.0f)RGB[2] = 255.0f;
		if (RGB[3] > 255.0f)RGB[3] = 255.0f;

		MatrixManager->setMode(oldMode);
		
		modelPacket->add((uint128)AddColour((int)RGB[0], (int)RGB[1], (int)RGB[2], (int)RGB[3] ).i128 );
	}
	else if (channel == CHANNEL2)
	{
		modelPacket->add((uint128)AddColour((int)(RGB[0] * 255.0f), (int)(RGB[1] * 255.0f), (int)(RGB[2] * 255.0f), (int)(RGB[3] * 255.0f)).i128 );
	}
	else if (channel == CHANNEL1)
	{
		doVuAdd(COLOR_T);
		RGB[0] *=  255.0f;
		RGB[1] *=  255.0f;
		RGB[2] *=  255.0f;
		RGB[3] *=  255.0f;

		modelPacket->add((uint32 *)&RGB, 4);
		//modelPacket->add((uint128)AddColour((RGB[0] * 255.0f), (int)(RGB[1] * 255.0f), (int)(RGB[2] * 255.0f), (int)(RGB[3] * 255.0f)).i128 );		
	}

	return 1;
}


int Custom_Model::addColor(vectorf *colorData)
{

	matrixf worldView;
	vectorf work;

	int oldMode;

	if (channel == CHANNEL2 && (dataType & NORMAL_T) == NORMAL_T)
	{

		//Get the model view matrix and store it
		oldMode = MatrixManager->setMode(MODELVIEW);
		worldView = *MatrixManager->getMatrix();

		/*
		//Get the camera matrix and apply it to the model view matrix and store it.
		MatrixManager->setMode(CAMERA);
		ps2_vu0_mul_matrix(worldView, *MatrixManager->getMatrix(), worldView);
		*/
		//Remove the tanslation from the matrix, just need the rotations
		worldView[3][0] = 0.0f;
		worldView[3][1] = 0.0f;
		worldView[3][2] = 0.0f;

		//Now apply the world view matrix to the light matrix and store it
		MatrixManager->setMode(LIGHT);
		ps2_vu0_mul_matrix(worldView, *MatrixManager->getMatrix(), worldView);

		//Get the stored normal from the matrix class
		work = normal;
		
		//Send the normal throught the calculated matrix
		ps2_vu0_apply_matrix(work,worldView, work);

		
		//Set negative values to 0 (vu maxx)
		if (work[0] < 0.0f)work[0] = 0.0f;
		if (work[1] < 0.0f)work[1] = 0.0f;
		if (work[2] < 0.0f)work[2] = 0.0f;
		//Must set the work vectors w to 1.0f to color properly
		work[3] = 1.0f;
		

		//Apply the work transformed vector through the color matrix
		MatrixManager->setMode(COLOR);		
		ps2_vu0_apply_matrix(work, *MatrixManager->getMatrix(), work);

				//Set the color to be from 0 to 255
		*colorData[0] *= 255.0f;
		*colorData[1] *= 255.0f;
		*colorData[2] *= 255.0f;
		*colorData[3] *= 255.0f;

		//Multiply the RGB by the light and store it in RGB
		ps2_vu0_mul_vector(*colorData, *colorData, work);

		//Set the maximum to 1.0f
		if (*colorData[0] > 255.0f)*colorData[0] = 255.0f;
		if (*colorData[1] > 255.0f)*colorData[1] = 255.0f;
		if (*colorData[2] > 255.0f)*colorData[2] = 255.0f;
		if (*colorData[3] > 255.0f)*colorData[3] = 255.0f;

		MatrixManager->setMode(oldMode);
		
		modelPacket->add((uint128)AddColour((int)*colorData[0], (int)*colorData[1], (int)*colorData[2], (int)*colorData[3] ).i128 );
	}
	else if (channel == CHANNEL2)
	{
		modelPacket->add((uint128)AddColour((int)(*colorData[0] * 255.0f), (int)(*colorData[1] * 255.0f), (int)(*colorData[2] * 255.0f), (int)(*colorData[3] * 255.0f)).i128 );
	}
	else if (channel == CHANNEL1)
	{
		doVuAdd(COLOR_T);
		*colorData[0] *=  255.0f;
		*colorData[1] *=  255.0f;
		*colorData[2] *=  255.0f;
		*colorData[3] *=  255.0f;

		modelPacket->add((uint32 *)colorData, 4);
		//modelPacket->add((uint128)AddColour((RGB[0] * 255.0f), (int)(RGB[1] * 255.0f), (int)(RGB[2] * 255.0f), (int)(RGB[3] * 255.0f)).i128 );		
	}


	return 1;
}


int Custom_Model::addNormal(float normX, float normY, float normZ)
{
	switch(channel)
	{
	case CHANNEL1:
		vectorf tmpNormal;
		tmpNormal[0] = normX;
		tmpNormal[1] = normY;
		tmpNormal[2] = normZ;
		tmpNormal[3] = 0.0f;
		//Normalize just in case
		ps2_vu0_normalize(tmpNormal, tmpNormal);
	
		doVuAdd(NORMAL_T);
		modelPacket->add((uint32*)&tmpNormal, 3);
		break;
	case CHANNEL2:
		normal[0] = normX;
		normal[1] = normY;
		normal[2] = normZ;
		normal[3] = 0.0f;
		
		ps2_vu0_normalize(normal, normal);
		normal[3] = 0.0f;
		
		break;

	}

	return 1;
}

int Custom_Model::addNormal(vectorf *norm)
{


	switch(channel)
	{
	case CHANNEL1:
		//Normalize the normal, just in case
		ps2_vu0_normalize(*norm, *norm);
		doVuAdd(NORMAL_T);
		modelPacket->add((uint128)*norm);
		break;
	case CHANNEL2:
		normal = *norm;
		ps2_vu0_normalize(normal, normal);
		normal[3] = 0.0f;
		break;	
	}

	return 1;
}


int Custom_Model::addST(float s, float t)
{

	switch(channel)
	{
	case CHANNEL1:
		doVuAdd(ST_T);
		modelPacket->add((uint32)(int)(s * 255.0f) << 4);
		modelPacket->add((uint32)(int)(t * 255.0f) << 4);
		break;
	case CHANNEL2:
		modelPacket->add((uint128)AddUV((int)(s * 255.0f), (int)(t * 255.0f)).i128);
		
		break;

	}

	return 1;
}

int Custom_Model::addST(vectorf *stData)
{

	switch(channel)
	{
	case CHANNEL1:
		doVuAdd(ST_T);
		modelPacket->add((uint32)(int)(*stData[0] * 255.0f) << 4);
		modelPacket->add((uint32)(int)(*stData[1] * 255.0f) << 4);
		break;
	case CHANNEL2:
		modelPacket->add((uint128)AddUV((int)(*stData[0] * 255.0f), (int)(*stData[1] * 255.0f)).i128);
		break;

	}

	return 1;
}

int Custom_Model::setTextureID(int tID)
{
	textureID = tID;
	return 1;
}