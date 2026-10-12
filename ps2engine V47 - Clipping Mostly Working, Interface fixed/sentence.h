class Sentence
{
private:
	char sentenceText;
	Sentence *next;
public:
	Sentence(char *newSentenceText);
	~Sentence();

	char *createSentence(Action *verb,NPC *firstNPC = NULL, NPC *secondNPC = NULL);
	int setNext(Sentence *newNext);
	Sentence *getNext();
};