#include "timestamp.h"

Time_Stamp::Time_Stamp()
{
	day = 0;
	year = 0;
	hour = 0;
}

Time_Stamp::Time_Stamp(int newDay, int newYear, int newHour)
{
	day = newDay;
	year = newYear;
	hour = newHour;
}

Time_Stamp::~Time_Stamp()
{
	day = 0;
	year = 0;
	hour = 0;
}

int Time_Stamp::getDay()
{
	return day;
}

int Time_Stamp::getHour()
{
	return hour;
}

int Time_Stamp::getYear()
{
	return year;
}

int Time_Stamp::setDay(int newDay)
{
	day = newDay;
	return 1;
}

int Time_Stamp::setHour(int newHour)
{
	hour = newHour;
	return 1;
}

int Time_Stamp::setYear(int newYear)
{
	year = newYear;
	return 1;
}
