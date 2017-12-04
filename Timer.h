#ifndef TimerImport
#define TimerImport
#include "Units.h"

typedef struct Timer
{
	UInt32* TimerCount;
	UInt8* TimerRunning;
	void (*StartTimer) (struct Timer*);
	void (*StopTimer) (struct Timer*);
	void (*ResetTimer) (struct Timer*);
	void (*TriggerMethod) (void (*trigger)(void));
} Timer;

/*------------------------------------------------------------*/
/* NVIC                                                       */
/*------------------------------------------------------------*/
#define PIT_IRQ_NUMBER (22)
/*------------------------------------------------------------*/
/* NVIC_ICPR                                                  */
/* 31-00:CLRPEND=pending status for HW IRQ sources;           */
/*              read:   0 = not pending;  1 = pending         */
/*              write:  0 = no effect;                        */
/*                      1 = change status to not pending      */
/* 22:PIT IRQ pending status                                  */
/*------------------------------------------------------------*/
#define NVIC_ICPR_PIT_MASK (1 << PIT_IRQ_NUMBER)
/*------------------------------------------------------------*/
/* NVIC_IPR0-NVIC_IPR7                                        */
/* 2-bit priority:  0 = highest; 3 = lowest                   */
/*------------------------------------------------------------*/
#define PIT_IRQ_PRIORITY (0)
#define PIT_IPR_REGISTER (PIT_IRQ_NUMBER >> 2)
#define NVIC_IPR_PIT_MASK \
                      (3 << (((PIT_IRQ_NUMBER & 3) << 3) + 6))
/*------------------------------------------------------------*/
/* NVIC_ISER                                                  */
/* 31-00:SETENA=masks for HW IRQ sources;                     */
/*              read:   0 = masked;     1 = unmasked          */
/*              write:  0 = no effect;  1 = unmask            */
/* 22:PIT IRQ mask                                            */
/*------------------------------------------------------------*/
#define NVIC_ISER_PIT_MASK (1 << PIT_IRQ_NUMBER)
/*------------------------------------------------------------*/
/* PIT_LDVALn                                                 */
/* Clock ticks for 0.001 s = 1 ms at 24 MHz PIT clock rate    */
/* (0.01 s * 2,400,000 Hz) - 1                               */
/*------------------------------------------------------------*/
#define PIT_LDVAL_1ms  (23999u)
/*------------------------------------------------------------*/
/* PIT_LDVALn                                                 */
/* Clock ticks for 0.01 s = 10 ms at 24 MHz PIT clock rate    */
/* (0.01 s * 24,000,000 Hz) - 1                               */
/*------------------------------------------------------------*/
#define PIT_LDVAL_10ms  (239999u)
/*------------------------------------------------------------*/
/* PIT_MCR:  PIT module control register                      */
/* 1-->    0:FRZ=freeze (continue'/stop in debug mode)        */
/* 0-->    1:MDIS=module disable (PIT section)                */
/*                RTI timer not affected                      */
/*                must be enabled before any other PIT setup  */
/*------------------------------------------------------------*/
#define PIT_MCR_EN_FRZ  (PIT_MCR_FRZ_MASK)
/*------------------------------------------------------------*/
/* PIT_TCTRL:  timer control register                         */
/* 0-->   2:CHN=chain mode (enable)                           */
/* 1-->   1:TIE=timer interrupt enable                        */
/* 1-->   0:TEN=timer enable                                  */
/*------------------------------------------------------------*/
#define PIT_TCTRL_CH_IE  \
                       (PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK)

void newTimer (Timer* newObject);

void StartTimer (Timer* self);
void StopTimer (Timer* self);
void ResetTimer (Timer* self);
void TriggerMethod (void (*trigger)(void));

void PIT_IRQHandler (void);
#endif
