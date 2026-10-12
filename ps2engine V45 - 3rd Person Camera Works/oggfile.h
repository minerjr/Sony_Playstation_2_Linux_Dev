struct OGG_Data
{


};

class OGG_Loader
{

private:
	OGG_Data *loadData;	

public:

	OGG_Loader(char *fileName);
	~OGG_Loader();

	bool load(char *fileName);

	char *readData();

};