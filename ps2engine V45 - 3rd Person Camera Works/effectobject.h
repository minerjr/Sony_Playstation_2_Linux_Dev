class Effect_Object
{
private:
	Custom_Model *effectModel;
	vectorf position;
	vectorf direction;
	Collision_Detection *collisionMap;	
	Effect_Object *next;
	int lifeSpan;

public:

	Effect_Object();
	Effect_Object(char *fileName);
	~Effect_Object();

	bool load(char *fileName);

	Effect_Object *getNext();
	int setNext(Effect_Object *newNext);

	Custom_Model *getCustomModel();

	vectorf getPosition();
	vectorf getDirection();
	int setDirection(vectorf newDirection);
	int setPosition(vectorf newPosition);

	int getLifeSpan();
	int setLifeSpan(int newLifeSpan);
	
	Collision_Detection *getCollisionMap();
	int setCollisionMap(Collision_Detection *newCollisionMap);

};