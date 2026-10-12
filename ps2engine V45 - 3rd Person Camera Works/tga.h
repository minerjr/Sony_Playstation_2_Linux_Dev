#ifndef TGA_H
#define TGA_H

class TGA_Loader
{

public:
	TGA_Loader(char *fileName);
	~TGA_Loader();
	
	int loadTGA(char *fileName);
};

#endif