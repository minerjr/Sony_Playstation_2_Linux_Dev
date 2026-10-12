class Talk_Window
{
private:
	Dma_Packet *talkPacket;
	Font_Engine *fontEngine;
	int textureID;
	char currentMsg[512];
public:
	Talk_Window(Font_Engine *tFontEngine, int tTextureID);
	~Talk_Window();

	int setMsg(char *newMsg);
	int clearMsg();
	char *getMsg();

	int buildMsg();

	int setTextureID(int newTextureID);
	int getTextureID();
	Dma_Packet *getPacket();




};