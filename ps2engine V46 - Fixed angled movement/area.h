#include "collision.h"
#include "npc.h"
//#include "gossipengine.h"

#ifndef AREA_H
#define AREA_H

class Gossip_Engine;

class Area
{
private:
	char *name;
	int areaID;
	int areaTypeID;

	//Entrance *entrances;

	NPC *npcs;

	Area *next;

	Gossip_Engine *gossipEngine;

	//Item *itemList;

	Collision_Detection *collisionMap;

	char *loadLevelFileName;
	

public:

	Area(char *aName, int aAreaID, int aAreaTypeID, char *aEntrancesFileName, NPC *aNPCList, char *aItemListFileName, char *aCollisionMapFileName, char *aLoadLevelName);
	Area(char *fileName);
	~Area();

	bool loadCollisionMap(char *fileName);
	bool loadEntrances(char *fileName);
	bool loadItemList(char *fileName);
	bool loadArea(char *fileName);

	int setCollisionMap(Collision_Detection *newCollisionMap);
	Collision_Detection *getCollisionMap();

	//ADD TO TDD
	int setGossipEngine(Gossip_Engine *newGossipEngine);
	char *getName();
	int getAreaID();
	int getAreaTypeID();
	NPC *getNPC(int npcIndex);
	NPC *getNPCList();
	//Item *getItemList();
	//Item *getItem(int itemIndex);
	char *getLoadLevelFileName();

	//int addItem(Item *newItem);
	int addNPC(NPC *newNPC);

	//int removeItem(Item *oldItem);
	int removeNPC(NPC *oldNPC);

	int forceNPCMeet();

	int setNext(Area *newArea);
	Area *getNext();

};

#endif