#include "collision.h"
#include "matrix.h"



Collision_Detection::Collision_Detection(float cRadiusSquared, float **cBoundingBox, float *cPosition)
{
	radiusSquared = cRadiusSquared;
	position[0] = cPosition[0];
	position[1] = cPosition[1];
	position[2] = cPosition[2];
	position[3] = cPosition[3];
	if (cBoundingBox != NULL)
	{
		boundingBox[0][0] = cBoundingBox[0][0];
		boundingBox[0][1] = cBoundingBox[0][1];
		boundingBox[0][2] = cBoundingBox[0][2];
		boundingBox[0][3] = cBoundingBox[0][3];

		boundingBox[1][0] = cBoundingBox[1][0];
		boundingBox[1][1] = cBoundingBox[1][1];
		boundingBox[1][2] = cBoundingBox[1][2];
		boundingBox[1][3] = cBoundingBox[1][3];

		boundingBox[2][0] = cBoundingBox[2][0];
		boundingBox[2][1] = cBoundingBox[2][1];
		boundingBox[2][2] = cBoundingBox[2][2];
		boundingBox[2][3] = cBoundingBox[2][3];

		boundingBox[3][0] = cBoundingBox[3][0];
		boundingBox[3][1] = cBoundingBox[3][1];
		boundingBox[3][2] = cBoundingBox[3][2];
		boundingBox[3][3] = cBoundingBox[3][3];

		boundingBox[4][0] = cBoundingBox[4][0];
		boundingBox[4][1] = cBoundingBox[4][1];
		boundingBox[4][2] = cBoundingBox[4][2];
		boundingBox[4][3] = cBoundingBox[4][3];

		boundingBox[5][0] = cBoundingBox[5][0];
		boundingBox[5][1] = cBoundingBox[5][1];
		boundingBox[5][2] = cBoundingBox[5][2];
		boundingBox[5][3] = cBoundingBox[5][3];

		boundingBox[6][0] = cBoundingBox[6][0];
		boundingBox[6][1] = cBoundingBox[6][1];
		boundingBox[6][2] = cBoundingBox[6][2];
		boundingBox[6][3] = cBoundingBox[6][3];

		boundingBox[7][0] = cBoundingBox[7][0];
		boundingBox[7][1] = cBoundingBox[7][1];
		boundingBox[7][2] = cBoundingBox[7][2];
		boundingBox[7][3] = cBoundingBox[7][3];
	}

	doTest = true;
	materialTypeID = MATERIAL_T_NONE;
	next = NULL;
	
}

Collision_Detection::~Collision_Detection()
{
	
}

float *Collision_Detection::getBoundingBox()
{
	return &boundingBox[0][0];
}

bool Collision_Detection::getDoTest()
{
	return doTest;
}

int Collision_Detection::getMaterialTypeID()
{
	return materialTypeID;
}

Collision_Detection *Collision_Detection::getNext()
{
	return next;
}

float *Collision_Detection::getPosition()
{
	return position;
}

float Collision_Detection::getRadiusSquared()
{
	return radiusSquared;
}

int Collision_Detection::setDoTest(bool newDoTest)
{
	doTest = newDoTest;
	return 1;
}

int Collision_Detection::setMaterialTypeID(int newMaterialTypeID)
{
	materialTypeID = newMaterialTypeID;
	return 1;
}

int Collision_Detection::setNext(Collision_Detection *newNext)
{
	next = newNext;
	return 1;
}

int Collision_Detection::setPosition(float *newPosition)
{
	position[0] = newPosition[0];
	position[1] = newPosition[1];
	position[2] = newPosition[2];
    position[3] = newPosition[3];

	return 1;
}

bool Collision_Detection::testBoundingBox(Collision_Detection *otherCollisionMap)
{
	return true;
}

bool Collision_Detection::testCollision(Collision_Detection *otherCollisionMap)
{
	return true;
}

bool Collision_Detection::testRadiusSquared(Collision_Detection *otherCollisionMap)
{
	float *otherPosition;
	vectorf workDistance;
	float distanceSquared;

	if (otherCollisionMap == NULL)return false;
	
	otherPosition = otherCollisionMap->getPosition();

	workDistance[0] = otherPosition[0] - position[0];
	workDistance[1] = otherPosition[1] - position[1];
	workDistance[2] = otherPosition[2] - position[2];
	//printf("Position = %f %f %f\n", position[0], position[1], position[2]);
	//printf("Other Position = %f %f %f\n", otherPosition[0], otherPosition[1], otherPosition[2]);

	distanceSquared = workDistance[0] * workDistance[0] +
		workDistance[1] * workDistance[1] +
		workDistance[2] * workDistance[2];
	
	//printf("Collsion 1:%f 2:%f\n", distanceSquared, radiusSquared + otherCollisionMap->getRadiusSquared());
	if (distanceSquared <= radiusSquared + otherCollisionMap->getRadiusSquared())
	{
		//printf("Collide\n");
		return true;
	}

	return false;
}