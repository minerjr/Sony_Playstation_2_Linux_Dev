Quest_NPC_Node
{
	NPC *questNPC;
	int questUse;
	Quest_NPC_Node *next;
};

Quest_Item_Node
{
	Item *questItem;
	int questUse;
	Quest_Item_Node *next;
};

class Quest
{
private:
	char *name;
	char *description;
	int triggerType;
	int triggerInfo;
	int completionType;
	int completionInfo;
	Quest_NPC_Node *questNPCList;
	Quest_Item_Node *questItemList;
	Quest *next;
	bool completed;
public:
	Quest(char *qName, char *qDescription, int qTriggerType, int qTriggerInfo, int qCompletionType, int qCompletionInfo);
	Quest(char *fileName);
	~Quest();

	bool load(char *fileName);

	addNPC(NPC *newNPC, int newQuestUse);
	addItem(Item *newItem, int newQuestUse);

	char *getName();
	char *getDescription();

	int update();

	bool getCompleted();
	int setCompleted(bool newCompleted);



};

