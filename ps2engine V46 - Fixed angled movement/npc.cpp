#include <math.h>
#include "npc.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

NPC::NPC(char *npcFirstName, char *npcLastName, int baseStr, int baseDef, int baseMaxHP, int npcLevel,
		int npcExperience, Time_Stamp *npcBirthDate, int npcGender, int npcAreaID, int npcModelID,
		int npcTextureID, int npcMaxNumberOfGossip, Demon *npcDemonInfo)
{

	firstName = new char[strlen(npcFirstName) + 1];
	strcpy(firstName, npcFirstName);
	
	lastName = new char[strlen(npcLastName) + 1];
	strcpy(lastName, npcLastName);

	birthDate = new Time_Stamp(npcBirthDate->getDay(), npcBirthDate->getHour(), npcBirthDate->getYear());

	str = baseStr;
	def = baseDef;
	maxhp = baseMaxHP;
	hp = maxhp;
	level = npcLevel;
	experience = npcExperience;
	gender = npcGender;
	areaID = npcAreaID;
	modelID = npcModelID;
	textureID = npcTextureID;
	maxNumberOfGossip = npcMaxNumberOfGossip;
	demonInfo = npcDemonInfo;
	gossipList = NULL;
	acquaintances = NULL;
	next = NULL;
}

NPC::~NPC()
{
	Gossip *workGossip = gossipList;
	Gossip *deleteGossip = gossipList;
	Acquaintance *workAcquaintances = acquaintances;
	Acquaintance *deleteAcquaintances = acquaintances;

	delete [] firstName;
	delete [] lastName;

	if (demonInfo)delete demonInfo;
	if (birthDate)delete birthDate;

	while (workGossip != NULL)
	{
		workGossip = workGossip->getNext();
		delete deleteGossip;
		deleteGossip = workGossip;
	}

	if (deleteGossip)delete deleteGossip;

	while (workAcquaintances != NULL)
	{
		workAcquaintances = workAcquaintances->getNext();
		delete deleteAcquaintances;
		deleteAcquaintances = workAcquaintances;
	}

	if (deleteAcquaintances)delete deleteAcquaintances;

}

int NPC::addGossip(Gossip *gossip, Time_Stamp *currentTime)
{

	Gossip *newGossip = NULL; 
	Gossip *workGossip = NULL;
	Gossip *removeGossip = NULL;
	int counter = 0;
	float juiciness = 99999.0f;

	newGossip = new Gossip(gossip->getFirstPerson(), gossip->getSecondPerson(), gossip->getEvent(), gossip->getLikeAmount(), gossip->getGossipAge());

	workGossip = gossipList;

	if (workGossip == NULL)
	{
		gossipList = newGossip;
		counter++;
	}
	else
	{
		while (workGossip->getNext() != NULL)
		{
			workGossip = workGossip->getNext();
			counter++;
		}
		workGossip->setNext(newGossip);
	}


	//If there are too many pieces of gossip, remove the least juicey gossip from the gossip list
	if (counter > maxNumberOfGossip)
	{

		workGossip = gossipList;

		while (workGossip != NULL)
		{
			if (findJuiceyness(workGossip, currentTime) < juiciness)
			{
				juiciness = findJuiceyness(workGossip, currentTime);
				removeGossip = workGossip;
			}
			workGossip = workGossip->getNext();
		}

		this->removeGossip(removeGossip);

	}

	return 1;


}

int NPC::compareGossip(Gossip *src, Gossip *dst)
{
	Gossip *workGossip = gossipList;

	if(dst == NULL)
	{
		if (src == NULL)
		{
			return -1;
		}

		while (workGossip != NULL)
		{
			if(src->getFirstPerson() == workGossip->getFirstPerson() && src->getSecondPerson() == workGossip->getSecondPerson() 
				&& src->getEvent() == workGossip->getEvent())
			{
				return 1;
			}
			workGossip = workGossip->getNext();
		}
	}
	else if (src == NULL)
	{
		return -1;
	}
	else
	{
		return (src->getFirstPerson() == dst->getFirstPerson() && src->getSecondPerson() == dst->getSecondPerson() 
			&& src->getEvent() == dst->getEvent());
	}

	return 0;
}

float NPC::findJuiceyness(Gossip *gossip, Time_Stamp *currentTime)
{

	//If either the gossip or the current time is NULL retorn a really bad value for the juiciness
	if (gossip == NULL || currentTime == NULL)return -99999.0f;

	//Get the goodness value of the event in the gossip
	float goodnessValue = gossip->getEvent()->getGoodness();
	//Find the current npc's likeness value towards the first npc in the gossip
	float likeFirstPersonValue = getLike(gossip->getFirstPerson());
	//Fint the current npc's likeness value towards the second npc in the gossip
	float likeSecondPersonValue = getLike(gossip->getSecondPerson());

	//Get the pointer tot he gossips time stamp
	Time_Stamp *gossipTime = gossip->getGossipAge();

	//Calculate the age of the gossip in hours
	float gossipAge = gossipTime->getHour() + 24.0f * gossipTime->getDay() + 8760.0f * gossipTime->getYear();
	//Calculate the age of the world clock in hours
	float worldAge = currentTime->getHour() + 24.0f * currentTime->getDay() + 8760.0f * currentTime->getYear();

	//Calculate and store the difference between the two ages
	float ageDifference = worldAge - gossipAge;
	
	//Var used to store the actual calulated 
	float totalTime = 0.0f;

	//Set the age difference to work between a range from 1 to 8760
	if (ageDifference <= 0.0f) ageDifference = 1.0f;
	else if (ageDifference > 8760.0f) ageDifference = 8760.0f;

	totalTime = 1.0f - (ageDifference / 8760.0f);
	totalTime = (2.0f * totalTime) - (totalTime * totalTime);

	return (abs(goodnessValue) + abs(likeFirstPersonValue) + abs(likeSecondPersonValue)) * totalTime;

}

float NPC::getLike(NPC *friendNPC)
{

	Acquaintance *work = acquaintances;

    while(work != NULL)
	{
		if(work->getNPC() == friendNPC)
		{
			return work->getLike();
		}
		work = work->getNext();
	}

	return 0.0f;
}

int NPC::removeGossip(Gossip *gossip)
{
	Gossip *workGossip = gossipList;
	Gossip *removeGossip = NULL;

	if (workGossip == gossip)
	{
		gossipList = gossipList->getNext();
		removeGossip = workGossip;
		removeGossip->setNext(NULL);

	}
	else
	{
		while (workGossip->getNext() != NULL)
		{
			if (workGossip->getNext() == gossip)
				break;
			workGossip = workGossip->getNext();
		}
		if (workGossip->getNext() == NULL) return 0;

		removeGossip = workGossip->getNext();

		workGossip->setNext(removeGossip->getNext());

		removeGossip->setNext(NULL);
	}

	delete removeGossip;
	return 1;
}

Gossip *NPC::getRandomGossip()
{
	if (gossipList == NULL)return NULL;

	Gossip *workGossip = gossipList;

	while (workGossip->getNext() != NULL)
	{
	
		if (rand() % 3)
		{
			return workGossip;
		}
		workGossip = workGossip->getNext();
	}


	return workGossip;
}

Gossip *NPC::getBadGossip(NPC *secondNPC)
{
	if (gossipList == NULL)return NULL;

	float firstPersonLikeValue;
	float secondPersonLikeValue;
	
	Gossip *workGossip = gossipList;

	while (workGossip != NULL)
	{

		firstPersonLikeValue = secondNPC->getLike(workGossip->getFirstPerson());
		secondPersonLikeValue = secondNPC->getLike(workGossip->getSecondPerson());
		if (firstPersonLikeValue + secondPersonLikeValue < 0.0f)
		{
			return workGossip;
		}
		workGossip = workGossip->getNext();
	}

	return getRandomGossip();
}

Gossip *NPC::getGoodGossip(NPC *secondNPC)
{
	if (gossipList == NULL)return NULL;

	float firstPersonLikeValue;
	float secondPersonLikeValue;
	
	Gossip *workGossip = gossipList;

	while (workGossip != NULL)
	{

		firstPersonLikeValue = secondNPC->getLike(workGossip->getFirstPerson());
		secondPersonLikeValue = secondNPC->getLike(workGossip->getSecondPerson());
		if (firstPersonLikeValue + secondPersonLikeValue > 0.0f)
		{
			return workGossip;
		}
		workGossip = workGossip->getNext();
	}

	return getRandomGossip();
}

int NPC::updateAcquaintance(NPC *friendNPC, float likeAmount)
{
	Acquaintance *work = acquaintances;

	//If the acquaintance list is empty then add a new acquaintance.
	if (work == NULL)
	{
		acquaintances = new Acquaintance(friendNPC);
		acquaintances->setLike(likeAmount);
	}
	else
	{
		//Loop through every acquaintance to try to find the npc passed in
		while (work != NULL)
		{
		
			//If the npc is found then update the like amount for the npc
			if (work->getNPC() == friendNPC)
			{
				work->setLike(work->getLike() + likeAmount);
				break;
			}
			work = work->getNext();
		}

		//If the npc was not found, then add the npc to the end of the acquaintance list
		if (work == NULL)
		{
			work = acquaintances;
			while (work->getNext() != NULL)
			{
				work = work->getNext();
			}
			work->setNext(new Acquaintance(friendNPC));
			work->getNext()->setLike(likeAmount);
		}
	}

	return 1;
}

int NPC::checkSchedule(Time_Stamp *checkTime, int prioriyt, int duration)
{

	return 1;
}

int NPC::getAreaID()
{
	return areaID;
}

Time_Stamp *NPC::getBirthDate()
{
	return birthDate;
}

Collision_Detection *NPC::getCollisionMap()
{
	return npcCollisionMap;
}

int NPC::getCombatState()
{
	return combatState;
}

int NPC::getDef()
{
	return def;
}

Demon *NPC::getDemon()
{
	return demonInfo;
}

vectorf *NPC::getDirection()
{
	return &npcDirection;
}

Event *NPC::getEvent(Time_Stamp *currentTime)
{
	return NULL;
}

char *NPC::getFirstName()
{
	return firstName;
}

int NPC::getGender()
{
	return gender;
}

int NPC::getHP()
{
	return hp;
}

char *NPC::getLastName()
{
	return lastName;
}

int NPC::getMaxHP()
{
	return maxhp;
}

int NPC::getModelID()
{
	return modelID;
}

int NPC::getMoveType()
{
	return moveType;
}

char *NPC::getName()
{
	char *name = new char[strlen(firstName) + strlen(lastName) + 2];
	sprintf(name, "%s %s", firstName, lastName);

	return name;
	
}

NPC *NPC::getNext()
{
	return next;
}

vectorf *NPC::getPosition()
{
	return &npcPosition;
}

int NPC::getState()
{
	return state;
}

int NPC::getStr()
{
	return str;
}

int NPC::getTextureID()
{
	return textureID;
}

Timeline_Event *NPC::getTimelineEvent(Time_Stamp *currentTime)
{
	return NULL;
}

int NPC::move()
{
	return 1;
}

int NPC::moveTimelineEvent(Timeline_Event *moveEvent, Time_Stamp *newTime)
{
	return 1;
}

int NPC::rescheduleTimelineEvent(Timeline_Event *moveEvent)
{
	return 1;
}

int NPC::scheduleEvent(Event *newEvent, NPC *otherNPC, int priority, bool mustDo, Area *eventArea, int areaTypeID)
{
	return 1;
}

int NPC::setAreaID(int newAreaID)
{
	areaID = newAreaID;
	return 1;
}

int NPC::setCollisionMap(Collision_Detection *newCollisionMap)
{
	npcCollisionMap = newCollisionMap;
	return 1;
}

int NPC::setCombatState(int newCombatState)
{
	combatState = newCombatState;
	return 1;
}	

int NPC::setDemon(Demon *newDemon)
{
	demonInfo = newDemon;
	return 1;
}

int NPC::setDirection(vectorf *newDirection)
{
	npcDirection[0] = *newDirection[0];
	npcDirection[1] = *newDirection[1];
	npcDirection[2] = *newDirection[2];
	npcDirection[3] = *newDirection[3];
	return 1;
}

int NPC::setHP(int newHP)
{
	hp = newHP;
	return 1;
}

int NPC::setMoveType(int newMoveType)
{
	moveType = newMoveType;
	return 1;
}

int NPC::setNext(NPC *newNext)
{
	next = newNext;
	return 1;
}

int NPC::setPosition(vectorf *newPosition)
{
	npcPosition[0] = *newPosition[0];
	npcPosition[1] = *newPosition[1];
	npcPosition[2] = *newPosition[2];
	npcPosition[3] = *newPosition[3];
	return 1;
}

int NPC::setState(int newState)
{
	state = newState;
	return 1;
}

