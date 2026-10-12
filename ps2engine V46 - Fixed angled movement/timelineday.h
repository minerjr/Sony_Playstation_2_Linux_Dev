#include "timelineevent.h"
#include "timestamp.h"

#ifndef TIMELINEDAY_H
#define TIMELINEDAY_H

class Timeline_Event;

class Timeline_Day
{

private:
	Timeline_Event *head;
	char *name;

public:
	Timeline_Day();
	Timeline_Day(char *teName);
	~Timeline_Day();

	int addEvent(Timeline_Event *addEvent);
	int moveTimelineEvent(Timeline_Event *moveEvent, Timeline_Day *newDay, Time_Stamp *newTime);
	Timeline_Event *getTimelineEvent(Time_Stamp *currentTime);


};

#endif