#include "timestamp.h"
#include "structs.h"

#ifndef GAMECLOCK_H
#define GAMECLOCK_H

class Game_Clock_Engine
{

	Time_Stamp *gameTime;
	int timeChangeFrequency;
	uint64 lastupdate;

public:


	Game_Clock_Engine();
	~Game_Clock_Engine();

	int setGameClock(Time_Stamp *newTime);
	int setGameClockFrequency(int timeChange);
	int updateGameClock();
	Time_Stamp *getGameClock();
};

#endif