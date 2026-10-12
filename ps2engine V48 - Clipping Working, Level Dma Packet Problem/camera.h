class Camera
{

private:

	vectorf position;
	vectorf lookAt;
	
	float distance;

	bool locked;

	Collison_Detection *collisionMap;

	Matrix_Manager *matrixManager;

public:

	Camera(Matrix_Manager *cMatrixManager, vectorf cPosition, vectorf cLookAt, Collision_Detection *cCollisionMap);
	~Camera();

	int setPosition(vectorf newPosition);
	int setLookAt(vectorf newLookAt);
	int setDistance(float newDistance);
	int setLocked(bool newLocked);
	int setCollisionMap(Collision_Detection *newCollisionMap);
	
	vectorf getPosition();
	vectorf getLookAt();
	float getDistance();
	bool getLocked();
	Collision_Detection *getCollisionMap();


	int update();
	int rotate(float rotation);
	int zoom(float newDistance);

};