#ifndef SwitchImport
#define SwitchImport
#include "Units.h"
typedef struct Switch{
	UInt8 port;
	UInt8 (*GetStatus) (struct Switch* self);
} Switch;
void newSwitch(Switch* newObject, UInt8 portnumber);
void PORTC_PORTD_IRQHandler(void);
void EnableSwitchInterrupts (void (*trigger)(void));


#endif
