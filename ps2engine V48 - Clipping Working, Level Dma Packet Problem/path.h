struct Move_Node
{

	vectorf direction;
	vectorf startPosition;
	vectorf endPosition;
	Area *moveArea;

	Move_Node *next;
};

class Path
{
private:
	Move_Node *head;
	vectorf finalPosition;
	Area *finalArea;

public:

	Path(vectorf pFinalPosition, Area *pFinalArea);
	~Path();

	int addMovement(vectorf aDirection, vectorf aStartPosition, vectorf aEndPosition, Area *aMoveArea);

	int setFinalPostition(vectorf newFinalPosition);
	vectorf getFinalPosition();
	
	Move_Node *getNextMovement();

	int setFinalArea(Area *newFinalArea);
	Area *getFinalArea();

	bool checkPathLocation(NPC *checkNPC);

};