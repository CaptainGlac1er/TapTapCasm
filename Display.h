#ifndef DisplayImport
#define DisplayImport
#include "Units.h"

typedef struct Display
{
	char displayString[4];
	UInt8 state;
	UInt8* Lives;
	UInt8* Score;
	UInt8* PadState;
	void (*UpdateDisplay) (struct Display*, char string[4]);
	void (*Refresh) (struct Display*);
	void (*TurnOnRedLED) (void);
	void (*TurnOffRedLED) (void);
	void (*TurnOnGreenLED) (void);
	void (*TurnOffGreenLED) (void);
	void (*ClearLEDs) (void);
} Display;

void newDisplay(Display* newObject);

void UpdateDisplay (Display* self, char string[]);
void Refresh (Display* self);
void ClearDisplay(void);
#endif
