#ifndef GOSSIP_H
#define GOSSIP_H

class NPC;
//#include "npc.h"
#include "event.h"
#include "timestamp.h"


class Gossip
{

private:
	NPC *firstPerson;
	NPC *secondPerson;
	Event *gossipEvent;
	float likeAmount;
	Time_Stamp *gossipAge;

	Gossip *next;
public:

	Gossip(NPC *gFirstPerson, NPC *gSecondPerson, Event *gGossipEvent, float gLikeAmount, Time_Stamp *currentTime);
	~Gossip();

	NPC *getFirstPerson();
	NPC *getSecondPerson();
	Event *getEvent();
	float getLikeAmount();
	Time_Stamp *getGossipAge();

	Gossip *getNext();
	int setNext(Gossip *newNext);

};

#endif