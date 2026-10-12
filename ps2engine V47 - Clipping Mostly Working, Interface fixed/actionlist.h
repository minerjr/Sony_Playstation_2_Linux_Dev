#include "action.h"

#ifndef ACTIONLIST_H
#define ACTIONLIST_H

class Action_List
{
private:
	Action *badHead;
	Action *goodHead;
public:
	
	Action_List();
	~Action_List();

	Action *addAction(char *actionVerb, float actionLikeAmount, float actionLowerDuration, float actionUpperDuration, int actionAreaTypeID);
	Action *getGoodAction(float likeValue = 0.01f);
	Action *getGoodAction(int actionIndex);
	Action *getBadAction(float likeValue = -0.01f);
	Action *getBadAction(int actionIndex);
	int loadActionList(char *fileName);

};

#endif