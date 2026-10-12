#include "area.h"
#include "npc.h"
#include "gameclock.h"
#include <stdlib.h>
#include <stdio.h>

int main()
{

	Area *testArea;//The test area for the npc's to be stored in
	Gossip_Engine *testGossipEngine;//The test gossip engine
	Game_Clock_Engine *testGameClock;//The test game clock

	//Setup the game clock
	testGameClock = new Game_Clock_Engine();
	//Set the game clock change frequency to 10 seconds
	testGameClock->setGameClockFrequency(10000);

	//Setup the gossip engine
	testGossipEngine = new Gossip_Engine(testGameClock);
	//Get the gossip engine to load the action list from actions.txt
	testGossipEngine->loadActionList("actions.txt");

	//Setup the test area
	testArea = new Area("Area 1", 0, 0, "", NULL, "", "", "TestFile.dat");
	//Pass the test area's gossip engine to the one allready created
	testArea->setGossipEngine(testGossipEngine);

	//Add some npc's to the test area
	testArea->addNPC(new NPC("David", "Froy", 10, 10, 10, 1, 10, testGameClock->getGameClock(), MALE, 0, 0, 0, 5, NULL));
	testArea->addNPC(new NPC("Will", "Froy", 10, 10, 10, 1, 10, testGameClock->getGameClock(), MALE, 0, 0, 0, 5, NULL));
	testArea->addNPC(new NPC("Jeff", "Hoeksma", 10, 10, 10, 1, 10, testGameClock->getGameClock(), MALE, 0, 0, 0, 5, NULL));
	testArea->addNPC(new NPC("Cathy", "Lawson", 10, 10, 10, 1, 10, testGameClock->getGameClock(), FEMALE, 0, 0, 0, 5, NULL));


	//Force the npc's to meet 1000 times
	for (int x = 0; x < 1000; x++)
	{
		//Force each npc in the test area to meet each other
		testArea->forceNPCMeet();
		//Update the game clock
		testGameClock->updateGameClock();
	}

	//Open notepad and display the output text file
	//system("notepad.exe output.txt");



	//Delete all the alloaced test objects

	printf("Deleting AI Engine\n");
	delete testGossipEngine;
	delete testGameClock;
	delete testArea;
	printf("Done\n");

	return 0;
}
