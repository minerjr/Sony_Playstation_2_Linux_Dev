#include "area.h"
#include <string.h>
#include <stdlib.h>

Area::Area(char *aName, int aAreaID, int aAreaTypeID, char *aEntrancesFileName, NPC *aNPCList, char *aItemListFileName, char *aCollisionMapFileName, char *aLoadLevelName)
{
	name = new char[strlen(aName) + 1];
	strcpy(name,aName);
	areaID = aAreaID;
	areaTypeID = aAreaTypeID;

	collisionMap = NULL;

	npcs = aNPCList;

	loadEntrances(aEntrancesFileName);
	loadItemList(aItemListFileName);
	loadCollisionMap(aCollisionMapFileName);
	
	loadLevelFileName = new char[strlen(aLoadLevelName) + 1];
	strcpy(loadLevelFileName, aLoadLevelName);


}

Area::Area(char *fileName)
{
	loadArea(fileName);
}

Area::~Area()
{
	if (name)free(name);
	if (loadLevelFileName)free(loadLevelFileName);

	NPC *workNPC = npcs;
	NPC *deleteNPC = npcs;

	Collision_Detection *workCollision = collisionMap;
	Collision_Detection *deleteCollision = collisionMap;


	if (workNPC)
	{
		while(workNPC->getNext() != NULL)
		{
			workNPC = workNPC->getNext();
			delete deleteNPC;
			deleteNPC = workNPC;
		}

		if (deleteNPC)delete deleteNPC;
	}

	if (workCollision)
	{
		while (workCollision->getNext() != NULL)
		{
			workCollision = workCollision->getNext();
			delete deleteCollision;
			deleteCollision = workCollision;
		}

		if(deleteCollision)delete deleteCollision;
	}

	//Add code for deleting items and entrences
}

int Area::addNPC(NPC *newNPC)
{
	NPC *workNPC = npcs;

	if (workNPC == NULL)
	{
		npcs = newNPC;
	}
	else
	{
		while (workNPC->getNext() != NULL)
		{
			workNPC = workNPC->getNext();
		}

		workNPC->setNext(newNPC);
	}

	return 1;
}

int Area::forceNPCMeet()
{

	NPC *firstNPC = npcs;
	NPC *secondNPC = npcs;
	while (firstNPC != NULL)
	{
		while(secondNPC != NULL)
		{
			if (firstNPC != secondNPC)
			{
				gossipEngine->meet(firstNPC, secondNPC);
			}
			secondNPC = secondNPC->getNext();
		}
		secondNPC = npcs;
		firstNPC = firstNPC->getNext();
	}

	return 1;
}

int Area::setGossipEngine(Gossip_Engine *newGossipEngine)
{
	gossipEngine = newGossipEngine;
	return 1;
}

int Area::getAreaID()
{
	return areaID;
}

int Area::getAreaTypeID()
{
	return areaTypeID;
}

Collision_Detection *Area::getCollisionMap()
{
	return collisionMap;
}

char *Area::getLoadLevelFileName()
{
	return loadLevelFileName;
}

char *Area::getName()
{
	return name;
}

Area *Area::getNext()
{
	return next;
}

NPC *Area::getNPC(int npcIndex)
{
	NPC *workNPC = npcs;
	for (int x = 0; x < npcIndex; x++)
	{
		if (workNPC == NULL)return NULL;
		workNPC = workNPC->getNext();
		
	}
	return workNPC;
}

NPC *Area::getNPCList()
{
	return npcs;
}

bool Area::loadArea(char *fileName)
{
	
	return true;
}

bool Area::loadCollisionMap(char *fileName)
{
	return true;
}

bool Area::loadEntrances(char *fileName)
{
	return true;
}

bool Area::loadItemList(char *fileName)
{
	return true;
}

int Area::setCollisionMap(Collision_Detection *newCollisionMap)
{
	collisionMap = newCollisionMap;
	return 1;
}

int Area::setNext(Area *newArea)
{
	next = newArea;
	return 1;
}