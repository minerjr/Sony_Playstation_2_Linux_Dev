class Physics_Engine
{

private:
	float gravity;
	float friction;
public:

	Physics_Engine(float pGravity, float pFricition);
	~Physics_Engine();

	float getGravity();
	float getFricition();

	int applyGravity(NPC *npc);
	int applyFricition(NPC *npc);
	int applyGravity(Player *player);
	int applyFriction(Player *player);
	int applyGravity(Effect_Object *effect);

};
