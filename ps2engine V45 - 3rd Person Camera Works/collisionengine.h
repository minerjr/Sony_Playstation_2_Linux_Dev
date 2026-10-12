struct Collision_Node
{

	Collision_Detection *collisionMap;
	Collision_Node *next;

};

class Collision_Engine
{
private:
	Collision_Node *collisionList;
public:
	Collision_Engine();
	~Collsion_Engine();

	int addCollisionMap(Collision_Detection *newCollisionMap);
	int clear();

	bool testCollision(Collision_Detection *checkCollisionMap);
	



};