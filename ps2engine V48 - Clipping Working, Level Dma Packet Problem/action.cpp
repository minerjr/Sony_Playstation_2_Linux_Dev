#include "action.h"
#include <string.h>

Action::Action(char *actionVerb, float actionLikeAmount, float actionLowerDuration, float actionUpperDuration, int actionAreaTypeID)
{

	verb = new char[strlen(actionVerb) + 1];
	strcpy(verb, actionVerb);
	likeAmount = actionLikeAmount;
	lowerDuration = actionLowerDuration;
	upperDuration = actionUpperDuration;
	areaTypeID = actionAreaTypeID;
	next = NULL;

}

Action::~Action()
{

	delete [] verb;
}

float Action::getLikeAmount()
{
	return likeAmount;
}

float Action::getLowerDuration()
{
	return lowerDuration;
}

float Action::getUpperDuration()
{
	return upperDuration;
}

char *Action::getVerb()
{

	return verb;
}

int Action::setNext(Action *newNext)
{
	next = newNext;
	return 1;
}

Action *Action::getNext()
{
	return next;
}

int Action::getAreaTypeID()
{
	return areaTypeID;
}