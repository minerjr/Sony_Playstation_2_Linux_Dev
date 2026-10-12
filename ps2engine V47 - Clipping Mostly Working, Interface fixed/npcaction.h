class NPC_Action_Engine
{

private:
	NPC *npcList;
	Area *currentArea;
	Player *thePlayer;
	
public:
	NPC_Action_Engine(Player *nThePlayer, NPC *nNPCList, Area *nCurrentArea);
	~NPC_Action_Engine();


	int getState(int npcIndex);
	int getCombatState(int npcIndex);
	int setCurrentArea(Area *newCurrentArea);
	
	int updateNPCList();

	int run();
	int runEngine();
	int runCombatEngine();
	

};