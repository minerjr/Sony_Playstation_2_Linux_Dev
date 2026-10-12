#include "timelineevent.h"

Timeline_Event::Timeline_Event(Time_Stamp *teStartTime, NPC *teOtherNPC, Event *teTheEvent, 
		Area *teEventArea, int tePriority, bool teAltered, bool teMustDo, int teDuration)
{
	startTime = *teStartTime;
	otherNPC = teOtherNPC;
	theEvent = teTheEvent;
	eventArea = teEventArea;
	priority = tePriority;
	altered = teAltered;
	mustDo = teMustDo;
	duration = teDuration;
	
	next = NULL;
}

Timeline_Event::~Timeline_Event()
{

}

bool Timeline_Event::getAltered()
{
	return altered;
}

Event *Timeline_Event::getEvent()
{
	return theEvent;
}

Area *Timeline_Event::getEventArea()
{
	return eventArea;
}

bool Timeline_Event::getMustDo()
{
	return mustDo;
}

Timeline_Event *Timeline_Event::getNext()
{
	return next;
}

NPC *Timeline_Event::getOtherNPC()
{
	return otherNPC;
}

int Timeline_Event::getPriority()
{
	return priority;
}

Time_Stamp *Timeline_Event::getStartTime()
{
	return &startTime;
}

int Timeline_Event::setNext(Timeline_Event *newNext)
{
	next = newNext;
	return 1;
}

int Timeline_Event::setStartTime(Time_Stamp *newTime)
{
	startTime = *newTime;
	altered = true;

	return 1;
}

