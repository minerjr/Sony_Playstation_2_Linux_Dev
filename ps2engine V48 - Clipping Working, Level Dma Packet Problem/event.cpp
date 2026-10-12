#include "event.h"

#ifndef NULL
#define NULL 0
#endif

Event::Event(Action *evAction, float evGoodness, int evDuration, int evType)
{
	eventAction  = evAction;
	goodness = evGoodness;
	duration = evDuration;
	type = evType;
	next = NULL;
}

Event::~Event()
{
	eventAction = NULL;
	goodness = 0.0f;
	duration = 0;
	type = 0;
}

Action *Event::getAction()
{
	return eventAction;
}

float Event::getGoodness()
{
	return goodness;
}

int Event::getDuration()
{
	return duration;
}

int Event::getType()
{
	return type;
}


Event *Event::getNext()
{
	return next;
}

int Event::setNext(Event *newNext)
{
	next = newNext;
	return 1;
}