#include "timelineday.h"
#include "timelineevent.h"
#include "timestamp.h"
#include "gossipengine.h"

#ifndef TIMELINE_H
#define TIMELINE_H

class Gossip_Engine;

class Timeline
{


private:
	Timeline *next;
	Timeline_Day *head;
	Gossip_Engine *TLGossipEngine;

public:

	Timeline(char *fileName, Gossip_Engine *theGossipEngine);
	~Timeline();

	bool load(char *fileName);


	int setNext(Timeline *newNext);
	Timeline *getNext();

	Timeline_Event *getTimelineEvent(Time_Stamp *currentTime);
	int addEvent(Timeline_Event *addEvent);

};

#endif