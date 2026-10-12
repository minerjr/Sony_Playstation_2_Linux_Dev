class Acquaintance
{

private:
	NPC *person;
	float like;

	Acquaintance *next;

public:

	Acquaintance(NPC *newPerson);
	~Acquaintance();

	NPC *getNPC();
	float getLike();

	int setLike(float newLike);
	int setNext(Acquaintance *newAcquaintance);
	Acquaintance *getNext();

};