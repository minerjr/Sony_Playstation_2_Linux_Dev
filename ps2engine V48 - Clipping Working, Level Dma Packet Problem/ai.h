class AI_Engine
{

private:
	Player_Action_Engine *AIEPlayerActionEngine;
	NPC_Action_Engine *AIENPCActionEngine;
	Gossip_Engine *AIEGossipEngine;
	Collision_Engine *AIECollision_Engine;
	Game_Clock_Engine *AIEGameClockEngine;
	Path_Generator *AIEPathGenerator;
	Physics_Engine *AIEPhysicsEngine;
public:
	AI_Engine(Player *aThePlayer, Area *currentArea);
	~AI_Engine();

	int run();
	int setArea(Area *newArea);

};