class Audio_Engine
{

private:
	Sound_Effect_Engine *AESoundEffectEngine;
	Sound_Music_Engine *AESoundMusicEngine;
public:
	Audio_Engine();
	~Audio_Engine();

	int playSong(char *fileName, bool loop);
	int playEffect(char *fileName, bool loop);
	int stopSong();
	int stopEffect(int effectIndex);




};