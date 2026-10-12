#include "timeline.h"
#include <stdio.h>

Timeline::Timeline(char *fileName, Gossip_Engine *theGossipEngine)
{
	TLGossipEngine = theGossipEngine;
	head = new Timeline_Day[7];
	load(fileName);
}

Timeline::~Timeline()
{
	delete [] head;
}

int Timeline::addEvent(Timeline_Event *addEvent)
{
	int workDay = addEvent->getStartTime()->getDay() % 7;
	int testWorkDay = head[workDay].getTimelineEvent(NULL)->getStartTime()->getDay();
	
	if (addEvent->getStartTime()->getDay() == testWorkDay)
	{
		head[workDay].addEvent(addEvent);
		return 1;
	}
	
	if (next == NULL)
	{
		//If the date could not match the event, then just add the event to the current week
		head[workDay].addEvent(addEvent);
	}
	else
	{
		next->addEvent(addEvent);
	}


	return 1;
	
}

Timeline *Timeline::getNext()
{
	return next;
}

Timeline_Event *Timeline::getTimelineEvent(Time_Stamp *currentTime)
{
	int workDay = currentTime->getDay() % 7;

	if (currentTime->getDay() == head[workDay].getTimelineEvent(NULL)->getStartTime()->getDay())
	{
		return head[workDay].getTimelineEvent(currentTime);
	}
	else if (next != NULL)
	{
		return next->getTimelineEvent(currentTime);

	}
	else
	{
		return head[workDay].getTimelineEvent(currentTime);
	}

	return NULL;
}

int Timeline::setNext(Timeline *newNext)
{
	next = newNext;
	return 1;

}

bool Timeline::load(char *fileName)
{
	FILE *input = fopen(fileName, "r");
	if (input == NULL)return false;

	Event *newEvent = NULL;
	Time_Stamp *newTimeStamp = NULL;
	
	


	fclose(input);
	return true;
}