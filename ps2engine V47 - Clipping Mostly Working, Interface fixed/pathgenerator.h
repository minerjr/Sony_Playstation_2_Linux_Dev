class Path_Generator
{
private:

public:
	Path_Generator();
	~Path_Generator();

	Path *buildPath(NPC *buildNPC, Area *finalArea, vectorf finalPosition);

};
