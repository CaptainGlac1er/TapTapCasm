#include "Display.h"
#include "Units.h"
#include <MKL46Z4.h>
#include "Definitions.h"
void TurnOnRedLED(void);
void TurnOffRedLED(void);

void TurnOnGreenLED (void);
void TurnOffGreenLED (void);
void UpdateScore(Display* self);
void UpdateLives(Display* self);
void ClearLEDs(void);
void UpdatePads(Display* self);

void newDisplay(Display* newObject)
{
	newObject->UpdateDisplay = UpdateDisplay;
	newObject->Refresh = Refresh;
	newObject->TurnOffRedLED = TurnOffRedLED;
	newObject->TurnOnRedLED = TurnOnRedLED;
	newObject->TurnOnGreenLED = TurnOnGreenLED;
	newObject->TurnOffGreenLED = TurnOffGreenLED;
	newObject->ClearLEDs = ClearLEDs;
	
	SIM->SOPT1 &= ~(SIM_SOPT1_OSC32KSEL_MASK);
	SIM->SCGC5 |= (SIM_SCGC5_SLCD_MASK | 
								SIM_SCGC5_PORTB_MASK | 
								SIM_SCGC5_PORTC_MASK |  
								SIM_SCGC5_PORTD_MASK |  
								SIM_SCGC5_PORTE_MASK);
	
	UInt32 mask = PORT_PCR_ISF_MASK;
	PORTB->PCR[7] = mask;
	PORTB->PCR[8] = mask;
	PORTB->PCR[10] = mask;
	PORTB->PCR[11] = mask;
	PORTB->PCR[21] = mask;
	PORTB->PCR[22] = mask;
	PORTB->PCR[23] = mask;
	
	PORTC->PCR[17] = mask;
	PORTC->PCR[18] = mask;
	
	PORTD->PCR[0] = mask;
	
	PORTE->PCR[4] = mask;
	PORTE->PCR[5] = mask;
	
	LCD->GCR &= ~(LCD_GCR_LCDEN_MASK);
	
	LCD->GCR = (LCD_GCR_CPSEL_MASK | 
				 LCD_GCR_LADJ_MASK |
				 LCD_GCR_PADSAFE_MASK |
				 LCD_GCR_FFR_MASK |
				 LCD_GCR_SOURCE_MASK |
				 //LCD_GCR_LCLK_MASK |
				 (4 << LCD_GCR_LCLK_SHIFT) |
				 //LCD_GCR_DUTY_MASK);
				 (3 << LCD_GCR_DUTY_SHIFT));
				 
	LCD->AR = 0;
	
	LCD->FDCR = 0;
	
	LCD->PEN[0] = 0x000E0D80u;
	LCD->PEN[1] = 0x00300160u;
	
	LCD->BPEN[0] = 0x000C0000u;
	LCD->BPEN[1] = 0x00100100u;
	UInt8 config[64] = {	0x00, 
							0x00,
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							(LCD_WF_D_MASK | LCD_WF_H_MASK),
							(LCD_WF_C_MASK | LCD_WF_G_MASK),
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							(LCD_WF_A_MASK | LCD_WF_E_MASK),
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							(LCD_WF_B_MASK | LCD_WF_F_MASK),
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00,						
							0x00					
		};
	for(UInt8 i = 0; i<64; i++){
		LCD->WF8B[i] = config[i];
	}
	LCD->GCR = (LCD->GCR | LCD_GCR_LCDEN_MASK) & ~(LCD_GCR_PADSAFE_MASK);
	newObject->state = 0;
	
	newObject->UpdateDisplay(newObject, "    ");
	
	PORTD->PCR[5] = PORT_PCR_MUX(1);
	FPTD->PDDR |= 1 << 5;
	FPTD->PDOR |= 1 << 5;
	PORTE->PCR[29] = PORT_PCR_MUX(1);
	FPTE->PDDR |= 1 << 29;
	FPTE->PDOR |= 1 << 29;
	
}

void UpdateDisplay (Display* self, char string[4]){
	for(UInt8 i = 0; i < 4; i++){
		self->displayString[i] = string[i];
	}
	self->Refresh(self);
}
void TurnOffGreenLED(void){
	FPTD->PDOR |= 1 << 5;
}
void TurnOnGreenLED(void){
	FPTD->PDOR &=  ~(1 << 5);
}
void TurnOffRedLED(void){
	FPTE->PDOR |= 1 << 29;
}
void TurnOnRedLED(void){
	FPTE->PDOR &=  ~(1 << 29);
}
void ClearLEDs(void){
	TurnOffRedLED();
	TurnOffGreenLED();
}
void Refresh (Display* self){
	ClearDisplay();
	
	
	self->state = (self->state + 1) % 5;
	if(self->Score != NULL && self->Lives != NULL){
		if(FALSE){
			*self->Score = *self->Score + 1;
			*self->Score= *self->Score % 100;
		}
		UpdateLives(self);
		UpdateScore(self);
		UpdatePads(self);
	}
	
}
void UpdateLives(Display* self){
	if(*self->Lives == 0){
		LCD->WF8B[LCD_PIN_10] &= ~LCD_SEG_A;
		LCD->WF8B[LCD_PIN_12] &= ~LCD_SEG_A;		
	}
	if(*self->Lives == 2){
		LCD->WF8B[LCD_PIN_10] |= LCD_SEG_A;
	}else{
		LCD->WF8B[LCD_PIN_10] &= ~LCD_SEG_A;
	}
	if(*self->Lives >= 1){
		LCD->WF8B[LCD_PIN_12] |= LCD_SEG_A;
	}else{
		LCD->WF8B[LCD_PIN_12] &= ~LCD_SEG_A;
	}
}
void UpdatePads(Display* self){
	switch(*self->PadState){
		case 0:
			break;
		case 1:
			LCD->WF8B[LCD_PIN_9] |= LCD_SEG_D | LCD_SEG_E | LCD_SEG_G;
			LCD->WF8B[LCD_PIN_10] |= LCD_SEG_C;
			break;
		case 2:
			LCD->WF8B[LCD_PIN_11] |= LCD_SEG_D | LCD_SEG_E | LCD_SEG_G;
			LCD->WF8B[LCD_PIN_12] |= LCD_SEG_C;
			break;
		case 3:
			LCD->WF8B[LCD_PIN_9] |= LCD_SEG_D | LCD_SEG_E | LCD_SEG_G;
			LCD->WF8B[LCD_PIN_10] |= LCD_SEG_C;
			LCD->WF8B[LCD_PIN_11] |= LCD_SEG_D | LCD_SEG_E | LCD_SEG_G;
			LCD->WF8B[LCD_PIN_12] |= LCD_SEG_C;
			break;
	}
}

void UpdateScore(Display* self){
	UInt8 second = *self->Score % 10;
	UInt8 first = (*self->Score / 10) % 10;
//  A
//F   B
//  G
//E   C
//  D
	
	if(first == 0 || first == 2 || first == 3 || first == 5 || first == 7 || first == 8 || first == 9){
		LCD->WF8B[LCD_PIN_6] |= LCD_SEG_A;
	}
	if(first == 0 || first == 1 || first == 2 || first == 3 || first == 4 || first == 7 || first == 8 || first == 9){
		LCD->WF8B[LCD_PIN_6] |= LCD_SEG_B;
	}
	if(first == 0 || first == 1 || first == 3 || first == 4 || first == 5 || first == 6 || first == 7 || first == 8 || first == 9){
		LCD->WF8B[LCD_PIN_6] |= LCD_SEG_C;
	}
	if(first == 0 || first == 2 || first == 3 || first == 5 || first == 6 || first == 8){
		LCD->WF8B[LCD_PIN_5] |= LCD_SEG_D;
	}
	if(first == 0 || first == 2 || first == 6 || first == 8){
		LCD->WF8B[LCD_PIN_5] |= LCD_SEG_E;
	}
	if(first == 0 || first == 4 || first == 5 || first == 6 || first == 8 || first == 9){
		LCD->WF8B[LCD_PIN_5] |= LCD_SEG_F;
	}
	if(first == 2 || first == 3 || first == 4 || first == 5 || first == 6 || first == 8 || first == 9){
		LCD->WF8B[LCD_PIN_5] |= LCD_SEG_G;
	}
	if(second == 0 || second == 2 || second == 3 || second == 5 || second == 7 || second == 8 || second == 9){
		LCD->WF8B[LCD_PIN_8] |= LCD_SEG_A;
	}
	if(second == 0 || second == 1 || second == 2 || second == 3 || second == 4 || second == 7 || second == 8 || second == 9){
		LCD->WF8B[LCD_PIN_8] |= LCD_SEG_B;
	}
	if(second == 0 || second == 1 || second == 3 || second == 4 || second == 5 || second == 6 || second == 7 || second == 8 || second == 9){
		LCD->WF8B[LCD_PIN_8] |= LCD_SEG_C;
	}
	if(second == 0 || second == 2 || second == 3 || second == 5 || second == 6 || second == 8){
		LCD->WF8B[LCD_PIN_7] |= LCD_SEG_D;
	}
	if(second == 0 || second == 2 || second == 6 || second == 8){
		LCD->WF8B[LCD_PIN_7] |= LCD_SEG_E;
	}
	if(second == 0 || second == 4 || second == 5 || second == 6 || second == 8 || second == 9){
		LCD->WF8B[LCD_PIN_7] |= LCD_SEG_F;
	}
	if(second == 2 || second == 3 || second == 4 || second == 5 || second == 6 || second == 8 || second == 9){
		LCD->WF8B[LCD_PIN_7] |= LCD_SEG_G;
	}
}
void ClearDisplay(void){
	LCD->WF8B[LCD_PIN_5] = 0;
	LCD->WF8B[LCD_PIN_6] = 0;
	LCD->WF8B[LCD_PIN_7] = 0;
	LCD->WF8B[LCD_PIN_8] = 0;
	LCD->WF8B[LCD_PIN_9] = 0;
	LCD->WF8B[LCD_PIN_10] = 0;
	LCD->WF8B[LCD_PIN_11] = 0;
	LCD->WF8B[LCD_PIN_12] = 0;
}
//  D
//C   E
//  G
//B   F
//  A
