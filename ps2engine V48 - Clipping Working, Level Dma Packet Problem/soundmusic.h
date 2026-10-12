class Sound_Music_Engine
{

private:
	OGG_Loader *SEOggLoader;
public:
	
	Sound_Music_Engine();
	~Sound_Music_Engine();

	int play(char *fileName, bool loop);
	int stop();
	
};
