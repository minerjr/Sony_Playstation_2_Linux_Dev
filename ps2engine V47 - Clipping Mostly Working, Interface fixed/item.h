struct Item_Attribute
{
	char *name;
	int type;
	int value;

	Item_Attribute *next;
};

class Item
{

private:
	char *name;
	Item_Arttribute *attributes;

	int modelID;
	int textureID;


	bool equipable;
	bool isEquiped;
	bool usable;

	Item *next;

public:
	Item(char *fileName);
	~Item();

	bool load(char *fileName);

	int getModelID();
	int getTextureID();

	bool getEquipable();
	bool getUsuable();
	bool getIsEquiped();

	int setIsEquiped(bool newIsEquiped);

	Item_Attribte *getAttributes();

	int setNext(Item *newNext);
	Item *getNext();
};