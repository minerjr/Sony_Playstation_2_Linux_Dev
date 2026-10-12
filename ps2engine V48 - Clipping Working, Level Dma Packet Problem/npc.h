#ifndef NPC_H
#define NPC_H


#include "timestamp.h"
#include "gossip.h"
#include "timeline.h"
#include "timelineevent.h"
#include "collision.h"
//#include "path.h"
//#include "item.h"
#include "acquaintance.h"
#include "demon.h"
#include "area.h"

#define NPC_STANDING 0
#define NPC_WALKING 1
#define NPC_RUNNING 2
#define NPC_FLYING 3
#define NPC_FLYING 3

#define MALE 0
#define FEMALE 1

#define NPC_NOTHING 0
#define NPC_PANICKING 1
#define NPC_ATTACKING 2
#define NPC_MOVING 3
#define NPC_MEETING 4
#define NPC_DOINGEVENT 5
#define NPC_CRYING 6
#define NPC_DYING 7
#define NPC_TRANSFORMING 8


#define NPC_COMBAT_NOTHING 0
#define NPC_COMBAT_PUNCH 1
#define NPC_COMBAT_KICK 2
#define NPC_COMBAT_CLAWSWING 3
#define NPC_COMBAT_FIREBALLSHOOT 4
#define NPC_COMBAT_WALK 5

class NPC
{

private:
	char *firstName;
	char *lastName;
	int str;
	int def;
	int hp;
	int maxhp;
	int level;
	int experience;

	vectorf npcPosition;
	vectorf npcDirection;
	int moveType;
	
	Time_Stamp *birthDate;

	int areaID;

	int gender;
	int maxNumberOfGossip;

	int modelID;
	int textureID;

	int state;
	int combatState;
	
	
	Gossip *gossipList;

	Demon *demonInfo;

	Acquaintance *acquaintances;

	Timeline *mainTimeline;

	Timeline *currentTimeline;
	Timeline_Event *currentTimelineEvent;

	//Item *itemList;

	Collision_Detection *npcCollisionMap;

	NPC *next;

	//Path *movePath;

public:

	//NPC Constructor
	NPC(char *npcFirstName, char *npcLastName, int baseStr, int baseDef, int baseMaxHP, int npcLevel,
		int npcExperience, Time_Stamp *npcBirthDate, int npcGender, int npcAreaID, int npcModelID,
		int npcTextureID, int npcMaxNumberOfGossip, Demon *npcDemonInfo);

	~NPC();

	//Gossip functions
	int addGossip(Gossip *gossip, Time_Stamp *currentTime);
	int removeGossip(Gossip *gossip);
	float findJuiceyness(Gossip *gossip, Time_Stamp *currentTime);
	int compareGossip(Gossip *src, Gossip *dst = NULL);
	Gossip *getRandomGossip();
	Gossip *getGoodGossip(NPC *secondNPC);
	Gossip *getBadGossip(NPC *secondNPC);
	int updateAcquaintance(NPC *friendNPC, float likeAmount);
	float getLike(NPC *friendNPC);

	//Demon functions
	int setDemon(Demon *newDemon);
	Demon *getDemon();

	//State/General Info functions
	int setAreaID(int newAreaID);
	int getAreaID();
	int setHP(int newHP);
	int getHP();
	int getStr();
	int getMaxHP();
	int getDef();
	Time_Stamp *getBirthDate();
	char *getName();
	char *getLastName();
	char *getFirstName();
	int getGender();

	//Move npc functions
	vectorf *getPosition();
	vectorf *getDirection();
	int setPosition(vectorf *newPosition);
	int setDirection(vectorf *newDirection);
	int setMoveType(int newMoveType);
	int getMoveType();
	//Path *getMovePath();
	//int setMovePath(Path *newMovePath);
	int move();

	//Collision detection functions
	int setCollisionMap(Collision_Detection *newCollisionMap);
	Collision_Detection *getCollisionMap();
	
	//Model Info Functions
	int getModelID();
	int getTextureID();

	int getState();
	int setState(int newState);
	int getCombatState();
	int setCombatState(int newCombatState);

	//ADD TO TDD
	int setNext(NPC *newNext);
	NPC *getNext();

	//Timeline Functions
	int scheduleEvent(Event *newEvent, NPC *otherNPC, int priority, bool mustDo, Area *eventArea, int areaTypeID);
	Event *getEvent(Time_Stamp *currentTime);
	Timeline_Event *getTimelineEvent(Time_Stamp *currentTime);

	int checkSchedule(Time_Stamp *checkTime, int priority, int duration);
	int moveTimelineEvent(Timeline_Event *moveEvent, Time_Stamp *newTime);
	int rescheduleTimelineEvent(Timeline_Event *moveEvent);




};

#endif