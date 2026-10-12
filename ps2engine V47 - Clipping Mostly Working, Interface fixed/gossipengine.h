#ifndef GOSSIPENDINE_H
#define GOSSIPENDINE_H

#include "npc.h"
#include "action.h"
#include "actionlist.h"
#include "event.h"
#include "gossip.h"
#include "gameclock.h"
#include <fstream>
using std::ofstream;

class Gossip_Engine
{
private:
	Event *eventHead;

	Action_List *actionList;
	Game_Clock_Engine *GEGameClock;
	ofstream out;
	
public:

	Gossip_Engine(Game_Clock_Engine *GEGameClock);
	~Gossip_Engine();

	Event *addEvent(Action *eActionVerb, float eGoodness, int eDuration, int eAreaTypeID);
	Action *addAction(char *actionVerb, float actionLikeAmount, float actionLowerDuration, float actionUpperDuration, int actionAreaTypeID);
	int loadActionList(char *fileName);
	int meet(NPC *firstNPC, NPC *secondNPC);
	int talk(NPC *firstNPC, NPC *secondNPC);
	int doSomething(NPC *firstNPC, NPC *secondNPC);
	int tell(NPC *firstNPC, NPC *secondNPC, Gossip *newGossip);
	Action *getAction(float range);
	Action *getAction(int index);
		
};

#endif