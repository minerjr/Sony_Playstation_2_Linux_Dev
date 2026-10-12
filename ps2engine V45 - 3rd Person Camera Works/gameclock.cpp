#include "gameclock.h"
#include "perftime.h"

Game_Clock_Engine::Game_Clock_Engine()
{
	init_perftime();
	poll_counters();
	lastupdate = read_counter0();
	gameTime = new Time_Stamp(0,0,0);
}

Game_Clock_Engine::~Game_Clock_Engine()
{
	close_perftime();
	delete gameTime;

}


Time_Stamp *Game_Clock_Engine::getGameClock()
{
	return gameTime;
}

int Game_Clock_Engine::setGameClock(Time_Stamp *newTime)
{
	if (newTime == NULL)return 0;
	*gameTime = *newTime;
	return 1;
}

int Game_Clock_Engine::setGameClockFrequency(int timeChange)
{
	timeChangeFrequency = timeChange;
	return 1;
}

int Game_Clock_Engine::updateGameClock()
{
	//Update the timers
	poll_counters();
	//Compare the difference between the current time and last update to see if the differce is greater then
	//the change frequency, if it is then...
	if (read_counter0() - lastupdate >= timeChangeFrequency)
	{

		//Set the last time to the current time
		lastupdate = read_counter0();
		//Update the gameTime hour value by 1
		gameTime->setHour(gameTime->getHour() + 1);

		//If there are more then 23 hours then 
		if (gameTime->getHour() > 23)
		{
			//Set the gameTime hour value to 0
			gameTime->setHour(0);
			//Increase the gameTime day value by 1
			gameTime->setDay(gameTime->getDay() + 1);

			//If the gameTime day value is greater then 363 then
			if (gameTime->getDay() > 363)
			{
				//Set the gameTime day value to 0
				gameTime->setDay(0);
				//And incrase the gameTime year by 1
				gameTime->setYear(gameTime->getYear() + 1);
			}
		}//Return 1 to show that the clock was updated
		return 1;
	}

	//Return 0 to show that the clock was not updated
	return 0;
}
