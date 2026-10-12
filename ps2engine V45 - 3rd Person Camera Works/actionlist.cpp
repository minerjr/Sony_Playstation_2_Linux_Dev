#include "actionlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Action_List::Action_List()
{
	goodHead = NULL;
	badHead = NULL;
}

Action_List::~Action_List()
{

	//Vars used to walk through the link list and to delete items off the list
	Action *workAction, *deleteAction;

	workAction = goodHead;//Set the work variable to the head of the good list
	deleteAction = goodHead;//Set the delete variable to the head of the good list

	//Loop while workAction is not the end of the list
	while(workAction != NULL)
	{
		//Set work action to the next action on the list
		workAction = deleteAction->getNext();
		//Delete the deleteAction
		delete deleteAction;
		//Set delete action to the work action
		deleteAction = workAction;
	}

	//If there is still something in the deleteAction then delete it
	if (deleteAction)delete deleteAction;

	workAction = badHead;//Set the work variable to the head of the bad list
	deleteAction = badHead;//Set the delete variable to the head of the bad list

	//Loop while workAction is not the end of the list
	while(workAction != NULL)
	{
		//Set work action to the next action on the list
		workAction = deleteAction->getNext();
		//Delete the deleteAction
		delete deleteAction;
		//Set delete action to the work action
		deleteAction = workAction;
	}

	//If there is still something in the deleteAction then delete it
	if (deleteAction)delete deleteAction;

}

Action *Action_List::addAction(char *actionVerb, float actionLikeAmount, float actionLowerDuration, float actionUpperDuration, int actionAreaTypeID)
{
	Action *newAction;//New action to be added to the list

	Action *addActionList;//Worker pointer for stepping throught the list

	//If the like amount is greater then or equal to 0 then use the good list
	if(actionLikeAmount >= 0.0f)
	{
		addActionList = goodHead;
	}
	//Else use the bad list
	else
	{
		addActionList = badHead;
	}

	//Create the actual new action
	newAction = new Action(actionVerb, actionLikeAmount, actionLowerDuration, actionUpperDuration, actionAreaTypeID);
	
	//If the current list is empty then
	if (addActionList == NULL)
	{
		//Check to see if the like amount is greater then or equal to 0 then 
		if (actionLikeAmount >= 0.0f)
		{
			//Set the head of the good list to the new action
			goodHead = newAction;
			return newAction;
		}
		else
		{
			//Else set the bad head to the new action
			badHead = newAction;
			return newAction;
		}
	}
	
	//Other wise the current list is not empty then loop
	//until the next node on the link list is empty then
	while (addActionList->getNext() != NULL)
	{
		//Step to the next node
		addActionList = addActionList->getNext();
	}

	//Set the new action to the next node on the link list
	addActionList->setNext(newAction);

	return newAction;

}

Action *Action_List::getBadAction(int actionIndex)
{

	Action *returnAction;//Pointer to the action to be returned
	int counter = 0;//Counter variable to be matched up with actionIndex

	returnAction = badHead;//Set the return pointer to the head of the bad list

	while(returnAction != NULL)
	{
		if (counter == actionIndex)break;
		returnAction = returnAction->getNext();
		counter++;
	}

	//Not really neede because if the index is wrong then the last object will be null
	//if (counter != actionIndex)return NULL:

	return returnAction;

}


Action *Action_List::getGoodAction(int actionIndex)
{

	Action *returnAction;//Pointer to the action to be returned
	int counter = 0;//Counter variable to be matched up with actionIndex

	returnAction = goodHead;//Set the return pointer to the head of the good list

	while(returnAction != NULL)
	{
		if (counter == actionIndex)break;
		returnAction = returnAction->getNext();
		counter++;
	}

	//Not really neede because if the index is wrong then the last object will be null
	//if (counter != actionIndex)return NULL:

	return returnAction;

}


Action *Action_List::getBadAction(float likeValue)
{

	Action *returnAction = goodHead;//Pointer to the action to be returned
	Action *workAction = badHead;

	while(workAction != NULL)
	{
		//If the like value of the event is less then the like value of the event to return then break
		if ((rand() % 3) == 2 && workAction->getLikeAmount() >= likeValue && workAction->getLikeAmount() <= 0.0f)
		{
			returnAction = workAction;	
		}
		workAction = workAction->getNext();
	}

	//Not really neede because if the index is wrong then the last object will be null
	//if (counter != actionIndex)return NULL:

	return returnAction;

}

Action *Action_List::getGoodAction(float likeValue)
{

	Action *returnAction = badHead;//Pointer to the action to be returned
	Action *workAction = goodHead;

	while(workAction != NULL)
	{
		//If the like value of the event is less then the like value of the event to return then break
		if ((rand() % 3) == 2 && workAction->getLikeAmount() <= likeValue && workAction->getLikeAmount() >= 0.0f)
		{
			returnAction = workAction;	
		}
		workAction = workAction->getNext();
	}

	//Not really neede because if the index is wrong then the last object will be null
	//if (counter != actionIndex)return NULL:

	return returnAction;

}

int Action_List::loadActionList(char *fileName)
{

	FILE *input;//File pointer used for input

	//Vars used to hold read in data
	float readLikeAmount;
	float readLowerDuration;
	float readUpperDuration;
	int readAreaTypeID;
	char readName[256];

	//Var to hold read in data to be parsed
	char readData[256];

	//Try to open the file for reading and check it
	input = fopen(fileName, "r");
	//If there was an error return 0
	if (input == NULL)return 0;

	//Loop while not at the end of the file
	while(!feof(input))
	{
		//Try to read in a line of data, break if there was an error
		if(fgets(readData,256,input) == NULL)break;
		readData[strlen(readData) - 1] = '\0';
		//Copy the read in data into the name of the action
		sscanf(readData, "%s", readName);
		
		
		//Try to read in a line of data, break if there was an error
		if(fgets(readData,256,input) == NULL)break;
		readData[strlen(readData) - 1] = '\0';
		//Parse the data and store in the like amount variable
		sscanf(readData,"%f", &readLikeAmount);
		
		//Try to read in a line of data, break if there was an error
		if(fgets(readData,256,input) == NULL)break;
		readData[strlen(readData) - 1] = '\0';
		//Parse the data and store in the lower duration variable
		sscanf(readData,"%f", &readLowerDuration);
		
		//Try to read in a line of data, break if there was an error
		if(fgets(readData,256,input) == NULL)break;
		readData[strlen(readData) - 1] = '\0';
		//Parse the data and store in the upper duration variable
		sscanf(readData,"%f", &readUpperDuration);
	
		//Try to read in a line of data, break if there was an error
		if(fgets(readData,256,input) == NULL)break;
		readData[strlen(readData) - 1] = '\0';
		//Parse the data and store in the area type ID variable
		sscanf(readData,"%i", &readAreaTypeID);

		//Add the new action to the action list
		addAction(readName,readLikeAmount, readLowerDuration, readUpperDuration, readAreaTypeID);
	}

	//Close the input file
	fclose(input);

	return 1;

}
