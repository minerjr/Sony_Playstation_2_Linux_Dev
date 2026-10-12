#include "acquaintance.h"


Acquaintance::Acquaintance(NPC *newPerson)
{
	person = newPerson;
	like = 0.0f;
	next = NULL;
}

Acquaintance::~Acquaintance()
{

}

float Acquaintance::getLike()
{
	return like;
}

Acquaintance *Acquaintance::getNext()
{
	return next;
}

NPC *Acquaintance::getNPC()
{
	return person;
}

int Acquaintance::setLike(float newLike)
{
	like = newLike;
	return 1;
}

int Acquaintance::setNext(Acquaintance *newAcquaintance)
{
	next = newAcquaintance;
	return 1;
}

