class Sound_Effect_Engine
{

private:
	WAV_Loader *SEWavLoader;
	OGG_Loader *SEOggLoader;
public:
	
	Sound_Effect_Engine();
	~Sound_Effect_Engine();

	int play(char *fileName, bool loop);
	int stop(int soundIndex);
	
	int removeEffect(int effectIndex);


};