#include "Switch.h"

void (*SwitchTrigger) (void);
void TriggerMethod (void (*trigger)(void));
UInt8 GetStatus(Switch* self);

void newSwitch(Switch* newObject, UInt8 portnumber){
	PORTC->PCR[portnumber] =  PORT_PCR_MUX(1) | PORT_PCR_IRQC(10) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;
	FPTC->PDDR &= ~(1 << portnumber);
	newObject->port = portnumber;
	newObject->GetStatus = GetStatus;
}

void PORTC_PORTD_IRQHandler(void){
	if(SwitchTrigger != NULL){
		SwitchTrigger();
	}
	
}
UInt8 GetStatus(Switch* self){
	return (PORTC->ISFR & (1 << self->port)) > 0;
}

void EnableSwitchInterrupts (void (*trigger)(void)){
	
	__ASM ("CPSID I");
	NVIC->ICPR[0] |= (UInt32)(1 << 31);
	NVIC->ISER[0] |= (UInt32)(1 << 31);
	__ASM ("CPSIE I");
	SwitchTrigger = trigger;
}


