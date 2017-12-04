#include "Random.h"
void newRandom(Random* newObject)
{
	newObject->previousNumber = 1;
	newObject->GetRand = GetRand;
	newObject->GetRandSalt = GetRandSalt;
	newObject->prime = 100109;
}


UInt8 GetRand (Random* self, UInt8 max){
	self->previousNumber = (54526 * self->previousNumber) % self->prime;
	return self->previousNumber % max;
}
UInt8 GetRandSalt (Random* self, UInt8 max, UInt8 salt){
	self->previousNumber = (54526 * self->previousNumber + salt) % self->prime;
	return self->previousNumber % max;
}

