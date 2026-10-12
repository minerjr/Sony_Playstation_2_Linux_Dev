#include "gossipengine.h"
#include <stdlib.h>
#include <fstream>


using std::ofstream;
using std::endl;

#define STEPVALUE 0.01f
#define LIKERANDVALUE 4

#define EVENT_T_INSTANT 0
#define EVENT_T_NORMAL 1
#define EVENT_T_QUEST 2

Gossip_Engine::Gossip_Engine(Game_Clock_Engine *GEGameClock)
{
	actionList = new Action_List();
	eventHead = NULL;
	this->GEGameClock = GEGameClock;
	out.open("output.txt");
}

Gossip_Engine::~Gossip_Engine()
{
	Event *deleteEvent, *workEvent;

	//Delete the action list
	delete actionList;

	deleteEvent = eventHead;
	workEvent = eventHead;
	while (workEvent != NULL)
	{
		workEvent = workEvent->getNext();
		delete deleteEvent;
		deleteEvent = workEvent;
	}
	if (deleteEvent)delete deleteEvent;

	out.close();

	
}

Action *Gossip_Engine::addAction(char *actionVerb, float actionLikeAmount, float actionLowerDuration, float actionUpperDuration,int actionAreaTypeID)
{
	return actionList->addAction(actionVerb, actionLikeAmount, actionLowerDuration, actionUpperDuration, actionAreaTypeID);
}

Event *Gossip_Engine::addEvent(Action *eActionVerb, float eGoodness, int eDuration, int eAreaTypeID)
{
	Event *newEvent = new Event(eActionVerb, eGoodness, eDuration, eAreaTypeID);
	Event *workEvent;

	workEvent = eventHead;

	if (eventHead == NULL)
	{
		eventHead = newEvent;
	}
	else
	{
		while(workEvent->getNext() != NULL)
		{
			workEvent = workEvent->getNext();
		}
		workEvent->setNext(newEvent);
	}

	return newEvent;
	
}

Action *Gossip_Engine::getAction(int index)
{
	if (index >= 0)
	{
		return actionList->getGoodAction(index);
	}
	else
	{
		return actionList->getBadAction(-index);
	}
}

Action *Gossip_Engine::getAction(float range)
{
	if (range >= 0.0f)
	{
		return actionList->getGoodAction(range);
	}
	else
	{
		return actionList->getBadAction(range);
	}
}

int Gossip_Engine::loadActionList(char *fileName)
{
	return actionList->loadActionList(fileName);
}


//Could be taken out
int Gossip_Engine::tell(NPC *firstNPC, NPC *secondNPC, Gossip *newGossip)
{
	Time_Stamp *tmpCurrentTime = GEGameClock->getGameClock();

	//we pass in who first person is so that we can judge the gossip based on who's talking to us
	secondNPC->updateAcquaintance(firstNPC, newGossip->getLikeAmount());
	if (!(secondNPC->addGossip(newGossip, tmpCurrentTime)))return 0;

	return 1;
}

int Gossip_Engine::meet(NPC *firstNPC, NPC *secondNPC)
{
	int action;
	//we met, now.....what do we do?
	action = rand()%100;
	if(action < 50) // we gossip
	{
		//the first npc tries to tell the second npc something

		talk(firstNPC,secondNPC);
		
		//the second npc tries to tell the first npc something

		talk(secondNPC,firstNPC);
		
	}
	else if(action < 65) //we do something together
	{
		if(rand()%2) 
		{

			doSomething(firstNPC, secondNPC);
			

		}
		else 
		{

			doSomething(secondNPC, firstNPC);
			
		}
	}
	return 1;
}


int Gossip_Engine::talk(NPC *firstNPC, NPC *secondNPC)
{

	float tmpLike = firstNPC->getLike(secondNPC);
	int counter = 0; 

	Gossip *workGossip = NULL;

	//If the person is not on the list of acquaintences then
	if(tmpLike == 0.0f)
	{

		//Try to give the second npc a new piece of gossip they don't know.
		while(workGossip == NULL && counter < 5)
		{
			workGossip = firstNPC->getRandomGossip();
			if (!secondNPC->compareGossip(workGossip))
			{
				secondNPC->addGossip(workGossip, GEGameClock->getGameClock());
				out << firstNPC->getFirstName() << " " << firstNPC->getLastName() << " told "
					<< secondNPC->getFirstName() << " " << secondNPC->getLastName() << " that "
					<< workGossip->getFirstPerson()->getFirstName() << " " << workGossip->getFirstPerson()->getLastName() << " "
					<< workGossip->getEvent()->getAction()->getVerb() << " "
					<< workGossip->getSecondPerson()->getFirstName() << " " << workGossip->getSecondPerson()->getLastName() << endl;
				secondNPC->updateAcquaintance(firstNPC, workGossip->getLikeAmount());
			}
			counter++;
			workGossip = NULL;
		}
	}
	else
	{
		//Else if the first npc likes the second npc then 
		if (tmpLike >= 0.0f)
		{
			//Find the juiciest gossip on the first npc's gossip list
			workGossip = firstNPC->getGoodGossip(secondNPC);
			//And try to give it to the second npc
			if (!secondNPC->compareGossip(workGossip))
			{
				secondNPC->addGossip(workGossip, GEGameClock->getGameClock());
				out << firstNPC->getFirstName() << " " << firstNPC->getLastName() << " told "
					<< secondNPC->getFirstName() << " " << secondNPC->getLastName() << " that "
					<< workGossip->getFirstPerson()->getFirstName() << " " << workGossip->getFirstPerson()->getLastName() << " "
					<< workGossip->getEvent()->getAction()->getVerb() << " "
					<< workGossip->getSecondPerson()->getFirstName() << " " << workGossip->getSecondPerson()->getLastName() << endl;

				secondNPC->updateAcquaintance(firstNPC, workGossip->getLikeAmount());
			}
			//IF the npc allready has that piece of gossip then
			else
			{
				//Try to give them a random piece of gossip
				while(workGossip == NULL && counter < 5)
				{
					workGossip = firstNPC->getRandomGossip();
					if (!secondNPC->compareGossip(workGossip))
					{
						secondNPC->addGossip(workGossip, GEGameClock->getGameClock());
						out << firstNPC->getFirstName() << " " << firstNPC->getLastName() << " told "
							<< secondNPC->getFirstName() << " " << secondNPC->getLastName() << " that "
							<< workGossip->getFirstPerson()->getFirstName() << " " << workGossip->getFirstPerson()->getLastName() << " "
							<< workGossip->getEvent()->getAction()->getVerb() << " "
							<< workGossip->getSecondPerson()->getFirstName() << " " << workGossip->getSecondPerson()->getLastName() << endl;

						secondNPC->updateAcquaintance(firstNPC, workGossip->getLikeAmount());
					}
					counter++;
					workGossip = NULL;
				}
			}
		}
		else
		{
			//Find the juiciest gossip on the first npc's gossip list
			workGossip = firstNPC->getBadGossip(secondNPC);
			//And try to give it to the second npc
			if (!secondNPC->compareGossip(workGossip))
			{
				secondNPC->addGossip(workGossip, GEGameClock->getGameClock());
				out << firstNPC->getFirstName() << " " << firstNPC->getLastName() << " told "
					<< secondNPC->getFirstName() << " " << secondNPC->getLastName() << " that "
					<< workGossip->getFirstPerson()->getFirstName() << " " << workGossip->getFirstPerson()->getLastName() << " "
					<< workGossip->getEvent()->getAction()->getVerb() << " "
					<< workGossip->getSecondPerson()->getFirstName() << " " << workGossip->getSecondPerson()->getLastName() << endl;

				secondNPC->updateAcquaintance(firstNPC, workGossip->getLikeAmount());
			}
			//IF the npc allready has that piece of gossip then
			else
			{
				//Try to give them a random piece of gossip
				while(workGossip == NULL && counter < 5)
				{
					workGossip = firstNPC->getRandomGossip();
					if (!secondNPC->compareGossip(workGossip))
					{
						secondNPC->addGossip(workGossip, GEGameClock->getGameClock());
						out << firstNPC->getFirstName() << " " << firstNPC->getLastName() << " told "
							<< secondNPC->getFirstName() << " " << secondNPC->getLastName() << " that "
							<< workGossip->getFirstPerson()->getFirstName() << " " << workGossip->getFirstPerson()->getLastName() << " "
							<< workGossip->getEvent()->getAction()->getVerb() << " "
							<< workGossip->getSecondPerson()->getFirstName() << " " << workGossip->getSecondPerson()->getLastName() << endl;


						secondNPC->updateAcquaintance(firstNPC, workGossip->getLikeAmount());
					}
					counter++;
					workGossip = NULL;
				}
			}
		}
	}

	return 1;
}

int Gossip_Engine::doSomething(NPC *firstNPC, NPC *secondNPC)
{

	float range;
	float likeValue = firstNPC->getLike(secondNPC);
	Action *eventAction = NULL;

	if (likeValue == 0.0f)
	{
		range = (2.0f * STEPVALUE) - (STEPVALUE * STEPVALUE);
		if (rand() % LIKERANDVALUE)
		{
			range = -range;
		}
	}
	else
	{
		if (likeValue > 0.0f)
		{
			range = likeValue + ((1.0f - likeValue) * STEPVALUE);
		}
		else
		{
			range = likeValue - ((1.0f - likeValue) * STEPVALUE);
		}
		
	}

	if (range >= 0.0f)
	{
		eventAction = actionList->getGoodAction(range);
	}
	else
	{
		eventAction = actionList->getBadAction(range);
	}

	if (eventAction->getLikeAmount() >= 0.0f)
	{
		likeValue = STEPVALUE;
	}
	else
	{
		likeValue = -STEPVALUE;
	}


	firstNPC->updateAcquaintance(secondNPC, likeValue);

	if (eventAction->getLowerDuration() == 0)
	{
		Event *newEvent = addEvent(eventAction, eventAction->getLikeAmount(), 0, EVENT_T_INSTANT);
		out << firstNPC->getFirstName() << " " << firstNPC->getLastName() << " " << newEvent->getAction()->getVerb() << " "
			<< secondNPC->getFirstName() << " " << secondNPC->getLastName() << endl;

		firstNPC->addGossip(&Gossip(firstNPC, secondNPC, newEvent, eventAction->getLikeAmount(), GEGameClock->getGameClock()), GEGameClock->getGameClock());
		secondNPC->addGossip(&Gossip(firstNPC, secondNPC, newEvent, eventAction->getLikeAmount(), GEGameClock->getGameClock()), GEGameClock->getGameClock());
	}
	else
	{
		//Schedule Event with the npc's timeline
	}

	return 1;
}