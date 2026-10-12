#define DEMON_T_CLAWED 1
#define DEMON_T_WINGED 2
#define DEMON_T_FIREBALL 4
#define DEMON_T_INFRARED 8
#define DEMON_T_INVISIBLE 16


class Demon
{

private:
	int type;
	int level;
	int str;
	int def;
	int hp;
	int maxHP;
	int experience;

	int modelID;
	int textureID;

	bool enabled;
	bool visible;

public:

	Demon(int dType, int dLevel, int dStr, int dDef, int dHP, int dMaxHP, int dModelID, int dTextureID, bool dEnabled, bool dVisible);
	~Demon();

	int getType();
	int getLevel();
	int getStr();
	int getDef();
	int getHP();
	int getMaxHP();
	int getExperience();

	int getModelID();
	int getTextureID();

	bool getEnabled();
	bool getVisible();

	
	int setHP(int newHP);
	int setEnabled(bool newEnabled);
	int setVisible(bool newVisible);




};