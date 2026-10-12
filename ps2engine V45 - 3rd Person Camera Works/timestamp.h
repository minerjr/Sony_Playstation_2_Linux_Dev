#ifndef TIMESTAMP_H
#define TIMESTAMP_H

class Time_Stamp
{
private:

	int day;
	int year;
	int hour;

public:
	Time_Stamp();
	Time_Stamp(int newDay, int newYear, int newHour);
	~Time_Stamp();

	int getDay();
	int getHour();
	int getYear();

	int setDay(int newDay);
	int setHour(int newHour);
	int setYear(int newYear);

};

#endif