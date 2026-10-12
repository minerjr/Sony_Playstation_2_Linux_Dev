#include "custommodel.h"
#include "addregisters.h"

Custom_Model::Custom_Model(Matrix_Manager *tmpMatrixManager, int tmpChannel,Dma_Memory_Engine *DmaMemoryEngine)
{
	MatrixManager = tmpMatrixManager;
	//If there is no matrix manager exit, there needs to be one for the custom model to work
	if (MatrixManager == NULL)return;

	addCount = 0;

	channel = tmpChannel;

	addFlg = 0;
	
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

int Custom_Model::begin(int tmpMode, int tmpDataType)
{
	int oldMode;
	matrixf workMatrix;

	sps2GIFTag_t GifTag;
	sps2GIFRegPRIM_t Prim;
	
	sps2VIFCodeBASE_t vifBase;
	sps2VIFCodeOFFSET_t vifOffset;
	sps2VIFCodeMSCALF_t vifMSCalf;

	sps2VIFCodeUNPACK_t vifUnpack;

	mode = tmpMode;
	dataType = tmpDataType;


	addCount = 0;
	addFlg = 0;
	if (modelPacket == NULL)return 0;

	modelPacket->reset();

	
	
	Prim.i64 = 0;
	Prim.s.IIP = 1;
		
	//If the mode is not valid then
	if (!(mode >= 0 && mode <= 6))return 0;
	Prim.s.PRIM = mode;
	

	GifTag = AddGifTag(0, 1, 1, 0, GIF_FLG_PACKED, 0);
	GifTag.s.PRIM = Prim.i64;


	if ((dataType & ST_T) == ST_T)
	{
		SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_ST);
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
		vifBase.i32 = 0;
		vifBase.s.BASE = 0;
		vifBase.s.CMD = VIF_CMD_BASE;

		vifOffset.i32 = 0;
		vifOffset.s.OFFSET = 512;
		vifOffset.s.CMD = VIF_CMD_OFFSET;

		modelPacket->add((uint32)vifBase.i32);
		modelPacket->add((uint32)vifOffset.i32);

		
		//Setup to unpack data into the vu memory (3 matrixs)
		vifUnpack.i32 = 0;
		vifUnpack.s.CMD = VIF_CMD_UNPACK;
		vifUnpack.s.FORMAT = VIF_UNPACK_V4_32;
		vifUnpack.s.ADDR = 32768;
		vifUnpack.s.FLG = 1;
		vifUnpack.s.SIZE = 12;//12x 128 bit values

		modelPacket->add((uint32)vifUnpack.i32);


		//Get the projection matrix
		oldMode = MatrixManager->setMode(PROJECTION);
		workMatrix = *MatrixManager->getMatrix();
		
		//Multiply the view matrix by the projection matrix and store it in the work matrix
		MatrixManager->setMode(VIEW);
		ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);

		//Add the work screen (view * projection) matrix to the packet
		modelPacket->add((uint128 *)&workMatrix, 4);


		//Do the light matrix


		//Get the model view matrix and store it
		MatrixManager->setMode(MODELVIEW);
		workMatrix = *MatrixManager->getMatrix();

		//Get the camera matrix and apply it to the model view matrix and store it.
		MatrixManager->setMode(CAMERA);
		ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);

		//Remove the tanslation from the matrix, just need the rotations
		workMatrix[3][0] = 0.0f;
		workMatrix[3][1] = 0.0f;
		workMatrix[3][2] = 0.0f;

		//Now apply the world view matrix to the light matrix and store it
		MatrixManager->setMode(LIGHT);
		ps2_vu0_mul_matrix(workMatrix, *MatrixManager->getMatrix(), workMatrix);

		//Add the work screen (view * projection) matrix to the packet
		modelPacket->add((uint128 *)&workMatrix, 4);


		//Add the light color matrix

		//Get the model view matrix and store it
		MatrixManager->setMode(COLOR);
		workMatrix = *MatrixManager->getMatrix();

		//Add the work color matrix to the packet
		modelPacket->add((uint128 *)&workMatrix, 4);


		//Set the matrix manager back to the orginal mode
		MatrixManager->setMode(oldMode);

		

		vifMSCalf.i32 = 0;
		vifMSCalf.s.CMD = VIF_CMD_MSCALF;
		vifMSCalf.s.EXECADDR = 0;

		modelPacket->add((uint32)vifMSCalf.i32);

		//Setup to unpack data into the vu memory (3 matrixs)
		//vifUnpack.i32 = 0;
		//vifUnpack.s.CMD = VIF_CMD_UNPACK;
		//vifUnpack.s.FORMAT = VIF_UNPACK_V4_32;
		//vifUnpack.s.ADDR = 32768;
		//vifUnpack.s.FLG = 1;
		vifUnpack.s.SIZE = 1;//12x 128 bit values

		modelPacket->add((uint32)vifUnpack.i32);


	}

	modelGifHeader = modelPacket->getCurrent();

	modelPacket->add((uint128)GifTag.i128);

	return 1;
}

int Custom_Model::end()
{
	if (modelGifHeader == NULL)return 0;

	((sps2GIFTag_t *)modelGifHeader)->s.NLOOP = addCount;
	
	modelPacket->finish();

	return 1;
}

int Custom_Model::doVuAdd(int flg)
{
	sps2GIFTag_t GifTag;
	sps2GIFRegPRIM_t Prim;
	sps2VIFCodeUNPACK_t vifUnpack;

	switch(flg)
	{
	case VERTEX_T:
		//IF there are 60 vertex's in the packet then
		if (addCount % 60 == 0 && addCount != 0)
		{
			//Set the add count of the orginal gif tag
			((sps2GIFTag_t *)modelGifHeader)->s.NLOOP = addCount;
			//Reset the add count
			addCount = 0;

			//Add a new gif tag to the list
			Prim.i64 = 0;
			Prim.s.IIP = 1;
		
			//If the mode is not valid then
			if (!(mode >= 0 && mode <= 6))return 0;
			Prim.s.PRIM = mode;
			GifTag = AddGifTag(0, 1, 1, 0, GIF_FLG_PACKED, 0);
			GifTag.s.PRIM = Prim.i64;

			if ((dataType & ST_T) == ST_T)
			{
				SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_ST);
			}
			if ((dataType & COLOR_T) == COLOR_T)
			{
				SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
			}
			if ((dataType & VERTEX_T) == VERTEX_T)
			{
				SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
			}
			
			((sps2VIFCodeUNPACK_t *)modelVertexHeader)->s.SIZE = addCount / 3;

			//Setup to unpack data into the vu memory (3 matrixs)
			vifUnpack.i32 = 0;
			vifUnpack.s.CMD = VIF_CMD_UNPACK;
			vifUnpack.s.FORMAT = VIF_UNPACK_V3_32;
			vifUnpack.s.ADDR = 32769;
			vifUnpack.s.FLG = 1;
			//vifUnpack.s.SIZE = 11;//12x 128 bit values

			modelPacket->add((uint32)vifUnpack.i32);
			modelGifHeader = modelPacket->getCurrent();

			modelPacket->add((uint128)GifTag.i128);
		}
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
	
	//printf("Untransformed: X: %f Y: %f Z: %f\n",workVector[0], workVector[1], workVector[2]);
	
	//Get the current matrix mode, store it and also set the mode to model view
	oldMode = MatrixManager->setMode(MODELVIEW);
	//Apply the current model view matrix to the inputed vertex

	ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
	//printf("Transformed Model: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);

	//Apply the current camera matrix
	MatrixManager->setMode(CAMERA);
	ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
	//printf("Transformed CAMERA: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);


	//If channel 2 is used, then apply the projection, perspective correction and view matrix 
	if (channel == CHANNEL2)
	{

		//Apply the current projection matrix
		MatrixManager->setMode(PROJECTION);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		//printf("Transformed PROJECTION: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);

	
		//Homogenos Division (ie divide xyz by w)
		ps2_vu0_div_vector_xyz(workVector, workVector, workVector[3]);
		workVector[3] = 1.0f;
		//printf("Transformed CORECTION: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);
		

		//Apply the current view port matrix
		MatrixManager->setMode(VIEW);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);

		//printf("Transformed VIEW: X: %f Y: %f Z: %f W: %f\n",workVector[0], workVector[1], workVector[2], workVector[3]);
			
		//Add the new transformed vertex to the model packet
		modelPacket->add((uint128)AddVertexFPo((int)(workVector[0]), (int)(workVector[1]), (int)(workVector[2]), 0).i128 );


	}
	else if (channel == CHANNEL1)
	{
		doVuAdd(VERTEX_T);
		if ((addFlg & VERTEX_T) == 0)
		{
			
		}
		modelPacket->add((uint32 *)&workVector, 3);
	}
	
	//Reset the matrix mode back to the old mode
	MatrixManager->setMode(oldMode);

	//Increase the vertex count by 1
	addCount++;

	return 1;
}

int Custom_Model::addVertex(vectorf *vertexData)
{
	int oldMode;
	//If there is no model packet, return 0
	if (modelPacket == NULL)return 0;

	//Get the current matrix mode, store it and also set the mode to model view
	oldMode = MatrixManager->setMode(MODELVIEW);
	//Apply the current model view matrix to the inputed vertex
	ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);

	//Apply the current camera matrix
	MatrixManager->setMode(CAMERA);
	ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
	//printf("Transformed CAMERA: X: %f Y: %f Z: %f W: %f\n",*vertexData[0], *vertexData[1], *vertexData[2], vertexData[3]);


	//If channel 2 is used, then apply the also projection matrix to the vertex
	if (channel == CHANNEL2)
	{

		//Apply the current projection matrix
		MatrixManager->setMode(PROJECTION);
		ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
		//printf("Transformed PROJECTION: X: %f Y: %f Z: %f W: %f\n",*vertexData[0], *vertexData[1], *vertexData[2], *vertexData[3]);

	
		//Homogenos Division (ie divide xyz by w)
		ps2_vu0_div_vector_xyz(*vertexData, *vertexData, *vertexData[3]);
		*vertexData[3] = 1.0f;
		//printf("Transformed CORECTION: X: %f Y: %f Z: %f W: %f\n",*vertexData[0], *vertexData[1], *vertexData[2], *vertexData[3]);
		

		//Apply the current view port matrix
		MatrixManager->setMode(VIEW);
		ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
		//printf("Transformed VIEW: X: %f Y: %f Z: %f W: %f\n",*vertexData[0], *vertexData[1], *vertexData[2], vertexData[3]);
	}
	//Reset the matrix mode back to the old mode
	MatrixManager->setMode(oldMode);
	//Add the new transformed vertex to the model packet

	if (channel == CHANNEL1)
	{
		doVuAdd(VERTEX_T);
	}
	
	modelPacket->add((uint128)AddVertexFPo((int)(*vertexData[0]), (int)(*vertexData[1]), (int)(*vertexData[2]), 0).i128 );

	addCount++;

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

		//Get the camera matrix and apply it to the model view matrix and store it.
		MatrixManager->setMode(CAMERA);
		ps2_vu0_mul_matrix(worldView, *MatrixManager->getMatrix(), worldView);

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
		if (work[3] < 0.0f)work[3] = 0.0f;

		//Apply the work transformed vector through the color matrix
		MatrixManager->setMode(COLOR);
		ps2_vu0_apply_matrix(work, *MatrixManager->getMatrix(), work);

		//Multiply the RGB by the light and store it in RGB
		ps2_vu0_mul_vector(RGB, RGB, work);

		//Set the maximum to 1.0f
		if (RGB[0] > 1.0f)RGB[0] = 1.0f;
		if (RGB[1] > 1.0f)RGB[1] = 1.0f;
		if (RGB[2] > 1.0f)RGB[2] = 1.0f;
		if (RGB[3] > 1.0f)RGB[3] = 1.0f;

		MatrixManager->setMode(oldMode);
	}
	
	
	

	modelPacket->add((uint128)AddColour((int)(RGB[0] * 255.0f), (int)(RGB[1] * 255.0f), (int)(RGB[2] * 255.0f), (int)(RGB[3] * 255.0f)).i128 );

	return 1;
}


int Custom_Model::addColor(vectorf *colorData)
{

	int oldMode;
	qword_t workQword;

	if (channel == CHANNEL2)
	{
		oldMode = MatrixManager->setMode(COLOR);
		ps2_vu0_apply_matrix(*colorData, *MatrixManager->getMatrix(), *colorData);

		MatrixManager->setMode(oldMode);
	}

	workQword.f32[0] = *colorData[0];
	workQword.f32[1] = *colorData[1];
	workQword.f32[2] = *colorData[2];
	workQword.f32[3] = *colorData[3];

	modelPacket->add(workQword.ul128);


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

		modelPacket->add((uint128)tmpNormal);
		break;
	case CHANNEL2:
		normal[0] = normX;
		normal[1] = normY;
		normal[2] = normZ;
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
		modelPacket->add((uint128)*norm);
		break;
	case CHANNEL2:
		normal = *norm;
		break;	
	}

	return 1;
}

