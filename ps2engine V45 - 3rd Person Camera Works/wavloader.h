struct WAV_Data
{


};

class WAV_Loader
{

private:
	WAV_Data *loadData;	

public:

	WAV_Loader(char *fileName);
	~WAV_Loader();

	bool load(char *fileName);

	char *readData();

};