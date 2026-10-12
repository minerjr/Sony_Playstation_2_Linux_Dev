class Game_HUD
{
private:
	Player *thePlayer;
	Dma_Packet *hudPacket;
	int textureID;
	Font_Engine *fontEngine;

public:
	Game_HUD(Player *gThePlayer, int gTextureID, Font_Engine *gFontEngine);
	~Game_HUD();

	int getTextureID();
	int setTextureID(int newTextureID);

	Dma_Packet *getPacket();
	int update();



};