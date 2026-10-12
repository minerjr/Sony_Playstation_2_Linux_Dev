#ifndef TIMELINEEVENT_H
#define TIMELINEEVENT_H

class NPC;//#include "npc.h"
#include "event.h"
#include "area.h"
#include "timestamp.h"

class Timeline_Event
{

private:
	NPC *otherNPC;
	Event *theEvent;
	Area *eventArea;
	int priority;
	bool altered;
	bool mustDo;
	int duration;
	Time_Stamp startTime;
	Timeline_Event *next;
public:

	Timeline_Event(Time_Stamp *teStartTime, NPC *teOtherNPC, Event *teTheEvent, 
		Area *teEventArea, int tePriority, bool teAltered, bool teMustDo, int teDuration);
	~Timeline_Event();

	Time_Stamp *getStartTime();
	NPC *getOtherNPC();
	Area *getEventArea();
	int getPriority();
	bool getAltered();
	bool getMustDo();
	Event *getEvent();

	int setStartTime(Time_Stamp *newTime);

	int setNext(Timeline_Event *newNext);
	Timeline_Event *getNext();


};

#endif