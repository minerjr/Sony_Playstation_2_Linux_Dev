#ifndef ACTION_H
#define ACTION_H

class Action
{
private:

	char *verb;
	float likeAmount;
	int areaTypeID;
	float lowerDuration;//shortest time the event can take place 
	float upperDuration;//longest time the event can take place

	Action *next;

public:


	Action(char *actionVerb, float actionLikeAmount, float actionLowerDuration, float actionUpperDuration, int actionAreaTypeID);
	~Action();

	char *getVerb();
	float getLikeAmount();
	float getLowerDuration();
	float getUpperDuration();

	int getAreaTypeID();
	

	int setNext(Action *newNext);
	Action *getNext();
};

#endif