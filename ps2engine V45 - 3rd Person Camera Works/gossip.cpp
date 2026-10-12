#include "gossip.h"

#ifndef NULL
#define NULL 0
#endif

Gossip::Gossip(NPC *gFirstPerson, NPC *gSecondPerson, Event *gGossipEvent, float gLikeAmount, Time_Stamp *currentTime)
{
	firstPerson = gFirstPerson;
	secondPerson = gSecondPerson;
	gossipEvent = gGossipEvent;
	likeAmount = gLikeAmount;
	gossipAge = new Time_Stamp(currentTime->getDay(), currentTime->getHour(), currentTime->getYear());
	next = NULL;
}

Gossip::~Gossip()
{
	delete gossipAge;
}

Event *Gossip::getEvent()
{
	return gossipEvent;
}

NPC *Gossip::getFirstPerson()
{
	return firstPerson;
}

NPC *Gossip::getSecondPerson()
{
	return secondPerson;
}

Time_Stamp *Gossip::getGossipAge()
{
	return gossipAge;
}

float Gossip::getLikeAmount()
{
	return likeAmount;
}

Gossip *Gossip::getNext()
{
	return next;
}

int Gossip::setNext(Gossip *newNext)
{
	next = newNext;
	return 1;
}
