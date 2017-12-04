#include "Timer.h"

UInt32 TimerCount = 0;
UInt8 TimerRunning = 0;
void (*Trigger)(void);
void InitTimer(void);

void newTimer (Timer* newObject){
	newObject->ResetTimer = ResetTimer;
	newObject->StartTimer = StartTimer;
	newObject->StopTimer = StopTimer;
	newObject->TriggerMethod = TriggerMethod;
	
  newObject->TimerCount = &TimerCount;
	newObject->TimerRunning = &TimerRunning;
	
	*newObject->TimerCount = (uint32_t) 0u; /* No IRQ periods measured */
	*newObject->TimerRunning = 0;
	
	InitTimer();
	
}
void InitTimer(void){
  __ASM ("CPSID I");  /* Mask all KL46 IRQs */
	  /* Enable PIT module clock */
  SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
  /* Disable PIT Timer 0 */
  PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
  /* Set PIT interrupt priority to 0 (highest) */
  NVIC->IP[PIT_IPR_REGISTER] &= NVIC_IPR_PIT_MASK;
  /* Clear any pending PIT interrupts */
  NVIC->ICPR[0] = NVIC_ICPR_PIT_MASK;
  /* Unmask UART0 interrupts */
  NVIC->ISER[0] = NVIC_ISER_PIT_MASK;
  /* Enable PIT timer module */
  /* and set to stop in debug mode */
  PIT->MCR = PIT_MCR_EN_FRZ;
  /* Set PIT Timer 0 period for 0.01 s */
  PIT->CHANNEL[0].LDVAL = PIT_LDVAL_1ms;
  /* Enable PIT Timer 0 and interrupt */
  PIT->CHANNEL[0].TCTRL = PIT_TCTRL_CH_IE;
  __ASM ("CPSIE I");  /* Unmask all KL46 IRQs */
}

void StartTimer (Timer* self){
	__asm("CPSID   I");
	*self->TimerRunning = 1;
  __asm("CPSIE   I");
}
void StopTimer (Timer* self){
	__asm("CPSID   I");
	*self->TimerRunning = 0;
  __asm("CPSIE   I");
}
void ResetTimer (Timer* self){
	__asm("CPSID   I");
	*self->TimerCount =  0;
  __asm("CPSIE   I");
}

void TriggerMethod (void (*trigger)(void)){
	Trigger = trigger;
}

void PIT_IRQHandler (void){
	__asm("CPSID   I");  /* mask interrupts */
  if (TimerRunning == 1) {
    TimerCount++;
  }
  /* clear PIT timer 0 interrupt flag */
  PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
  __asm("CPSIE   I");  /* unmask interrupts */
  if (TimerRunning == 1) {
		Trigger();
  }
}
