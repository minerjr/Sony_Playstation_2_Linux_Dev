#define PLAYER_STANDING 0
#define PLAYER_WALKING 1
#define PLAYER_RUNNING 2
#define PLAYER_FLYING 3
#define PLAYER_CLIMBING 4
#define PLAYER_JUMPING 5



#define PLAYER_NOTHING 0
#define PLAYER_ATTACKING 1
#define PLAYER_MOVING 2
#define PLAYER_TALKING 3
#define PLAYER_TRANSFORMING 4
#define PLAYER_DYING 5
#define PLAYER_USINGITEM 6
#define PLAYER_ENTERGAMEMENU 7
#define PLAYER_ENTERMAINMENU 8
#define PLAYER_ENTERDEMONABILITYMENU 9
#define PLAYER_BERSERK 10
#define PLAYER_ABSORBSOUL 11
#define PLAYER_CHECKLEVEL 12
#define PLAYER_INFRAREDVISION 13
#define PLAYER_INVISIBILITY 14
#define PLAYER_CAUGHT 15
#define PLAYER_BLACKOUT 16
#define PLAYER_FREAKOUT 17
#define PLAYER_TRANSPROT 18

#define PLAYER_COMBAT_NOTHING 0
#define PLAYER_COMBAT_PUNCH 1
#define PLAYER_COMBAT_KICK 2
#define PLAYER_COMBAT_SWORDSWING 3
#define PLAYER_COMBAT_CLAWSWING 4
#define PLAYER_COMBAT_FIREBALLSHOOT 5
#define PLAYER_COMBAT_SHINSEISWING 6
#define PLAYER_COMBAT_WALK 7


#define PLAYER_DEMONFLYING_ENABLED 1
#define PLAYER_DEMONFLYING_DISABLED 0

#define PLAYER_DEMON_ENABLED 1
#define PLAYER_DEMON_DISABLED 0

#define PLAYER_DEMONABILITY_CLAWS 1
#define PLAYER_DEMONABILITY_WINGS 2
#define PLAYER_DEMONABILITY_FIREBALL 4
#define PLAYER_DEMONABILITY_INFRARED 8
#define PLAYER_DEMONABILITY_INVISIBLE 16
#define PLAYER_DEMONABILITY_SHINSEI 32


#define PLAYER_SHINSEI_SHARD1 1
#define PLAYER_SHINSEI_SHARD2 2
#define PLAYER_SHINSEI_SHARD3 4
#define PLAYER_SHINSEI_SHARD4 8
#define PLAYER_SHINSEI_SHARD5 16
#define PLAYER_SHINSEI_SHARD6 32
#define PLAYER_SHINSEI_SHARD7 64
#define PLAYER_SHINSEI_SHARD8 128



class Player
{

private:

	int hp;
	int maxhp;
	int str;
	int sp;
	int maxsp;
	int def;
	int totalCharacterExpereince;
	int nextCharacterExperience;
	int level;

	char *firstName;
	char *lastName;

	bool demonEnabled;
	bool demonFlying;
	int demonSkills;


	int totalClawExperience;
	int nextClawExperience;
	int clawLevel;

	int totalWingExperience;
	int nextWingExperience;
	int wingLevel;

	int totalFireballExperience;
	int nextFireballExperience;
	int fireballLevel;

	int totalInfraredExperience;
	int nextInfraredExperience;
	int infraredLevel;

	int totalInvisibleExperience;
	int nextInvisibleExperience;
	int invisibleLevel;

	int shinseiShards;

	vectorf position;
	vectorf direction;
	int moveType;
	int state;
	int combatState;

	Time_Stamp *birthDate;

	int humanModelID;
	int humanTextureID;

	int demonModelID;
	int demonTextureID;


	Quest *questList;
	Item *itemList;

	Collision_Dectection *collisionMap;

	Area *currentArea;


public:
	Player(char *pFirstName, char *pLastName, int pMaxHP, int pMaxSP, int pStr, int pDef, vectorf position, vectorf direction);
	~Player();

	int getHP();
	int getMaxHP();
	int getSP();
	int getMaxSP();
	int getStr();
	int getDef();

	int getTotalCharacterExperience();
	int getNextCharacterExperience();
	int getLevel();

	char *getFirstName();
	char *getLastName();
	char *getName();

	bool getDemonEnabled();
	int getDemonSkills();
	bool getDemonFlying();

	int getTotalClawExperience();
	int getNextClawExperience();
	int getClawLevel();

	int getTotalWingExperience();
	int getNextWingExperience();
	int getWingLevel();

	int getTotalFireballExperience();
	int getNextFireballExperience();
	int getFireballLevel();

	int getTotalInfraredExperience();
	int getNextInfraredExperience();
	int getInfraredLevel();

	int getTotalInvisibleExperience();
	int getNextInvisibleExperience();
	int getInvisibleLevel();

	int getShinseiShards();

	vectorf getPosition();
	vectorf getDirection();
	int getMoveType();
	int getState();
	int setState(int newState);
	int getCombatState();
	int setCombatState(int newCombatState);
	
	Time_Stamp *getBirthDate();

	int getHumanModelID();
	int getHumanTextureID();

	int getDemonModelID();
	int getDemonTextureID();


	Area *getCurrentArea();
	int setCurrentArea(Area *newCurrentArea);

	Quest *getQuestList;
	Item *getItemList;

	Collision_Dectection *getCollisionMap;


	int setHP(int newHP);
	int setMaxHP(int newMaxHP);
	int setSP(int newSP);
	int setMaxSP(int newMaxSP);
	int setStr(int newStr);
	int setDef(int nreDef);

	

	int setFirstName(char *newFirstName);
	int setLastName(char *newLastName);
	
	int setDemonEnabled(bool newDemonEnabled);
	int setDemonSkills(int newDemonSkills);
	int setDemonFlying(bool newDemonFlying);

	int setShinseiShards(int newShinseiShards);

	int setPosition(vectorf newPosition);
	int setDirection(vectorf newDirection);
	int setMoveType(int newMoveType);
	
	int setBirthDate(Time_Stamp *newBirthDate);

	int setHumanModelID(int newHumanModelID);
	int setHumanTextureID(int newHumanTextureID);

	int setDemonModelID(int newDemonModelID);
	int setDemonTextureID(int newDemonTextureID);


	int addQuest(Quest *newQuest);
	int updateQuest(Quest *oldQuest, Quest *newQuestInfo);
	Quest *getQuest(int questIndex);

	int addItem(Item *newItem);
	Item *getItem(int itemIndex);
	int equipItem(Item *equipItem);
	int removeItem(Item *removeItem);

	int setCollisionMap(Collision_Dectection *newCollisionMap);

	int move();
	int levelup();

	int updateCharacterExperience(int newCharacterExperience);
	int updateClawExperience(int newClawExperience);
	int updateWingExperience(int newWingExperience);
	int updateInfraredExperience(int newInfraredExperience);
	int updateInvisibleExperience(int newInvisibleExperience);
	int updateFireballExperience(int newFireballExperience);


};