#ifndef RandomImport
#define RandomImport
#include "Units.h"

typedef struct Random
{
	UInt32 prime;
	UInt32 previousNumber;
	UInt8 (*GetRand) (struct Random*, UInt8 max);
	UInt8 (*GetRandSalt) (struct Random*, UInt8 max, UInt8 salt);
} Random;

void newRandom(Random* newObject);

UInt8 GetRand (Random* self, UInt8 max);
UInt8 GetRandSalt (Random* self, UInt8 max, UInt8 salt);

#endif
