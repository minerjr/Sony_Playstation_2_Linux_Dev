class Entrance
{

private:
	Area *newArea;
	vectorf position;
	Collision_Detection *collisionMap;
	Entrance *next;

public:

	Entrance(char *fileName);
	~Entrance();

	bool load(char *fileName);

	Area *getNewArea();
	vectorf getPosition();
	Collision_Detection *getCollisionMap();

	Entrance *getNext();
	int setNext(Entrance *newNext);


};