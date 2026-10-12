#include "custommodel.h"
#include "addregisters.h"

Custom_Model::Custom_Model(Matrix_Manager *tmpMatrixManager, int tmpChannel,Dma_Memory_Engine *DmaMemoryEngine)
{
	MatrixManager = tmpMatrixManager;
	//If there is no matrix manager exit, there needs to be one for the custom model to work
	if (MatrixManager == NULL)return;

	addCount = 0;

	channel = tmpChannel;
	
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
	mode = tmpMode;
	dataType = tmpDataType;
	sps2GIFTag_t GifTag;
	sps2GIFRegPRIM_t Prim;

	if (modelPacket == NULL)return 0;

	modelPacket->reset();

	
	
	Prim.i64 = 0;
	Prim.s.IIP = 1;	
	//If the mode is not valid then
	if (!(mode >= 0 && mode <= 6))return 0;
	Prim.s.PRIM = mode;

	printf("Mode = %d\n", ((uint32 *)&Prim.i64)[0]);

	
	switch(channel)
	{
	case CHANNEL1:
		GifTag = AddGifTag(0, 1, 1, ((uint32 *)&Prim.i64)[0], GIF_FLG_PACKED, 0);
		break;
	case CHANNEL2:
		GifTag = AddGifTag(0, 1, 1, ((uint32 *)&Prim.i64)[0], GIF_FLG_PACKED, 0);
		break;
	default:
		return 0;
		break;
	}


	if ((tmpDataType & ST_T) == ST_T)
	{
		SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_ST);
	}
	if ((tmpDataType & COLOR_T) == COLOR_T)
	{
		SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_RGBAQ);
	}
	if ((tmpDataType & VERTEX_T) == VERTEX_T)
	{
		SPS2_SET_GIF_REG(GifTag, GifTag.s.NREG++, GIF_REG_XYZ2);
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

int Custom_Model::addVertex(float verX, float verY, float verZ, float verW)
{
	vectorf workVector;
	//qword_t workQword;
	sps2GIFPackedRegister_t pReg;
	//matrixf workMatrix;
	int oldMode;

	//If there is no model packet, return 0
	if (modelPacket == NULL)return 0;

	workVector[0] = verX;
	workVector[1] = verY;
	workVector[2] = verZ;
	workVector[3] = verW;
	
	//Get the current matrix mode, store it and also set the mode to model view
	oldMode = MatrixManager->setMode(MODELVIEW);
	//Apply the current model view matrix to the inputed vertex
	//workMatrix = *MatrixManager->getMatrix();
	ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);

	//If channel 2 is used, then apply the camera and also projection matrix to the vertex
	if (channel == CHANNEL2)
	{
		MatrixManager->setMode(CAMERA);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		

		MatrixManager->setMode(PROJECTION);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		

		workVector[0] /= workVector[3];
		workVector[1] /= workVector[3];
		workVector[2] /= workVector[3];
		workVector[3] = 1.0f;
		

		MatrixManager->setMode(VIEW);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		


	}
	//Reset the matrix mode back to the old mode
	MatrixManager->setMode(oldMode);

	printf("X: %f Y: %f Z: %f\n",workVector[0], workVector[1], workVector[2]);
	//Add the new transformed vertex to the model packet
	pReg.i128 = 0;
	pReg.XYZ2.s.X = (int)workVector[0];
	pReg.XYZ2.s.Y = (int)workVector[1];
	pReg.XYZ2.s.Z = (int)workVector[2];
	pReg.XYZ2.s.ADC = 0;

	MatrixManager->setMode(oldMode);

	modelPacket->add(pReg.i128);
	addCount++;

	return 1;
}

int Custom_Model::addVertex(vectorf *vertexData)
{
	int oldMode;
	qword_t workQword;
	//If there is no model packet, return 0
	if (modelPacket == NULL)return 0;

	//Get the current matrix mode, store it and also set the mode to model view
	oldMode = MatrixManager->setMode(MODELVIEW);
	//Apply the current model view matrix to the inputed vertex
	ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);

	//If channel 2 is used, then apply the camera and also projection matrix to the vertex
	if (channel == CHANNEL2)
	{
		MatrixManager->setMode(CAMERA);
		ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
		MatrixManager->setMode(PROJECTION);
		ps2_vu0_apply_matrix(*vertexData, *MatrixManager->getMatrix(), *vertexData);
	}
	//Reset the matrix mode back to the old mode
	MatrixManager->setMode(oldMode);
	//Add the new transformed vertex to the model packet

	workQword.ui32[0] = *vertexData[0];
	workQword.ui32[1] = *vertexData[1];
	workQword.ui32[2] = *vertexData[2];
	workQword.ui32[3] = *vertexData[3];

	workQword.ui32[0] = workQword.ui32[0] << 4;
	workQword.ui32[1] = workQword.ui32[1] << 4;

	modelPacket->add(workQword.ul128);
	addCount++;

	return 1;

}

int Custom_Model::addColor(float red, float green, float blue, float alpha)
{
	vectorf workVector;
	qword_t workQword;
	int oldMode;

	workVector[0] = red;
	workVector[1] = green;
	workVector[2] = blue;
	workVector[3] = alpha;

	if (channel == CHANNEL2)
	{
		oldMode = MatrixManager->setMode(COLOR);
		ps2_vu0_apply_matrix(workVector, *MatrixManager->getMatrix(), workVector);
		
		MatrixManager->setMode(oldMode);
	}
	
	workQword.ui32[0] = (int)(workVector[0] * 255.0f);
	workQword.ui32[1] = (int)(workVector[1] * 255.0f);
	workQword.ui32[2] = (int)(workVector[2] * 255.0f);
	workQword.ui32[3] = (int)(workVector[3] * 128.0f);

	modelPacket->add(workQword.ul128);

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
