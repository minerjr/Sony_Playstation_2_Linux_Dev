#include "action.h"

#ifndef EVENT_H
#define EVENT_H

class Event
{
private:
	Action *eventAction;

	float goodness;
	int duration;
	int type;

	Event *next;

public:

	Event(Action *evAction, float evGoodness, int evDuration, int evType);
	~Event();

	float getGoodness();
	int getDuration();
	int getType();
	Action *getAction();

	int setNext(Event *newNext);
	Event *getNext();

};

#endif