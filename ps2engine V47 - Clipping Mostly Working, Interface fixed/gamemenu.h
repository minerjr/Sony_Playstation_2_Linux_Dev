#ifndef GAMEMENU_H
#define GAMEMENU_H

#define MENU_S_NONE 0
#define MENU_S_MAINMENU 1
#define MENU_S_INGAMEMENU 2
#define MENU_S_DEMONABILITYMENU 3
#define MENU_S_STATSMENU 4
#define MENU_S_INVENTORYMENU 5
#define MENU_S_QUESTMENT 6
#define MENU_S_OPTIONSMENU 7
#define MENU_S_SAVELOADMENU 8
#define MENU_S_MAINMENU_LOAD 9

#define MENU_M_MAINMENU_MAX 3
#define MENU_M_INGAMEMENU_MAX 6
#define MENU_M_DEMONABILITYMENU_MAX 6


class Game_Menu
{
private:
	Font_Engine *fontEngine;
	Dma_Packet *menuPacket;
	Player *thePlayer;
	int textureID;
	int state;
	int cursorPosition;
public:
	Game_Menu(Font_Engine *gFontEngine, Player *pThePlayer, int pTextureID, Area *currentArea = NULL);
	~Game_Menu();

	int setState(int newState);
	int getState();

	int setTextureID(int newTextureID);
	int getTextureID();

	int getCursorPosition();
	int setCursorPosition(int newCursorPosition);

	Dma_Packet *getDma_Packet();

};


#endif