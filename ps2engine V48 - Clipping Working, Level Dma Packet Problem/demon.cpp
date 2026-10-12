#include "demon.h"

Demon::Demon(int dType, int dLevel, int dStr, int dDef, int dHP, int dMaxHP, int dModelID, int dTextureID, bool dEnabled, bool dVisible)
{
	type = dType;
	level = dLevel;
	str = dStr;
	def = dDef;
	hp = dHP;
	maxHP = dMaxHP;
	modelID = dModelID;
	textureID = dTextureID;
	enabled = dEnabled;
	visible = dVisible;
	experience = 100;
}

Demon::~Demon()
{

}

int Demon::getDef()
{
	return def;
}

bool Demon::getEnabled()
{
	return enabled;
}

int Demon::getExperience()
{
	return experience;
}

int Demon::getHP()
{
	return hp;
}

int Demon::getLevel()
{
	return level;
}

int Demon::getMaxHP()
{
	return maxHP;
}

int Demon::getModelID()
{
	return modelID;
}

int Demon::getStr()
{
	return str;
}

int Demon::getTextureID()
{
	return textureID;
}

int Demon::getType()
{
	return type;
}

bool Demon::getVisible()
{
	return visible;
}

int Demon::setEnabled(bool newEnabled)
{
	enabled = newEnabled;
	return 1;
}

int Demon::setHP(int newHP)
{
	hp = newHP;
	return 1;
}

int Demon::setVisible(bool newVisible)
{
	visible = newVisible;
	return 1;
}