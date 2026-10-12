#include "timelineday.h"
#include <string.h>

Timeline_Day::Timeline_Day()
{
	head = NULL;
	name = NULL;
}

Timeline_Day::Timeline_Day(char *teName)
{
	name = new char[strlen(teName) + 1];
	strcpy(name,teName);
	head = NULL;

}

Timeline_Day::~Timeline_Day()
{
	Timeline_Event *workEvent;
	Timeline_Event *deleteEvent;
	
	delete [] name;

	workEvent = head;
	deleteEvent = head;
	while(workEvent != NULL)
	{
		workEvent = workEvent->getNext();
		delete deleteEvent;
		deleteEvent = workEvent;
	}
	if(deleteEvent)delete deleteEvent;
}

Timeline_Event *Timeline_Day::getTimelineEvent(Time_Stamp *currentTime)
{
	Timeline_Event *workEvent = head;

	if (head == NULL)return NULL;
	
	if (currentTime == NULL)
	{
		return head;
	}

	while (workEvent->getStartTime()->getHour() != currentTime->getHour())
	{
		workEvent = workEvent->getNext();
	}

	return workEvent;
	
}

int Timeline_Day::moveTimelineEvent(Timeline_Event *moveEvent, Timeline_Day *newDay, Time_Stamp *newTime)
{
	Timeline_Event *workEvent = head;
	Timeline_Event *removeEvent = NULL;

	if (head == NULL)return 0;

	if (moveEvent == head)
	{
		head = workEvent->getNext();
		workEvent->setNext(NULL);

		workEvent->setStartTime(newTime);
		newDay->addEvent(workEvent);
	}
	else
	{
		while (workEvent->getNext() != moveEvent)
		{
			workEvent = workEvent->getNext();
		}
		
		removeEvent = workEvent->getNext();
		workEvent->setNext(removeEvent->getNext());

		removeEvent->setNext(NULL);
		removeEvent->setStartTime(newTime);
		
		newDay->addEvent(removeEvent);
	}
	
	return 1;
}

int Timeline_Day::addEvent(Timeline_Event *addEvent)
{
	Timeline_Event *workEvent = head;

	if (head == NULL)
	{
		head = addEvent;
		return 1;
	}

	while (workEvent->getNext() != NULL)
	{
		workEvent = workEvent->getNext();
	}

	workEvent->setNext(addEvent);

	return 1;
}

