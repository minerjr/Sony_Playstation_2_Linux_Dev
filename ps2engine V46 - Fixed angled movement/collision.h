#ifndef COLLISION_H
#define COLLISION_H

#include "matrix.h"

#define MATERIAL_T_NONE 0
#define MATERIAL_T_WOOD 1
#define MATERIAL_T_BRICK 2
#define MATERIAL_T_CEMENT 3
#define MATERIAL_T_STEEL 4
#define MATERIAL_T_HIGHGSTEEL 5
#define MATERIAL_T_TITANIUM 6

class Collision_Detection
{
private:
	float radiusSquared;
	vectorf boundingBox[8];

	vectorf position;
	bool doTest;
	int materialTypeID;

	Collision_Detection *next;

public:
	Collision_Detection(float cRadiusSquared, float **cBoudingBox, float *cPosition);
	~Collision_Detection();

	float getRadiusSquared();
	float *getBoundingBox();
	float *getPosition();
	bool getDoTest();

	int getMaterialTypeID();
	int setMaterialTypeID(int newMaterialTypeID);

	int setPosition(float *newPosition);
	int setDoTest(bool newDoTest);

	bool testRadiusSquared(Collision_Detection *otherCollisionMap);
	bool testBoundingBox(Collision_Detection *otherCollisionMap);
	bool testCollision(Collision_Detection *otherCollisionMap);

	int setNext(Collision_Detection *newNext);
	Collision_Detection *getNext();
};

#endif