class Sentence_List
{
private:
	Sentence *head;

public:
	Sentence_List(char *fileName);
	~Sentence_List();

	bool load(char *fileName);

	char *getRandomSentence(Action *verb, NPC *firstNPC = NULL, NPC *secondNPC = NULL);
};