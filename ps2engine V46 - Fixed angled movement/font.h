class Font_Engine
{

private:
	int textureID;
	int width;
	int height;
public:
	Font_Engine(int fTextureID, int fWidth, int fHeight);
	~Font_Engine();

	Dma_Packet *textOut(float x, float y, char *msg);





};