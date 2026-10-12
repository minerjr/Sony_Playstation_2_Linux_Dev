class Player_Action_Engine
{

private:
	Player *thePlayer;
	Area *currentArea;
	Input_Data *currentInputData;
	NPC *npcList;
	
public:
	Player_Action_Engine(Player *pThePlayer, NPC *pNPCList, Area *pCurrentArea);
	~Player_Action_Engine();


	int getState();
	int getCombatState();
	int setCurrentArea(Area *newCurrentArea);
	int updateInputDate(Input_Data *newCurrentInputData);
	int updateNPCList();

	int run();
	int runEngine();
	int runCombatEngine();
	

};