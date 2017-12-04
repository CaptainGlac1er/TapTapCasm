#include "Terminal.h"
void newTerminal(Terminal* newObject)
{
	if(TRUE){
		__ASM ("CPSID I");
		Init_UART0_IRQ ();
		SetupUARTVariables();
		__ASM ("CPSIE I");
	}
	newObject->GetChar = GetChar;
	newObject->PutChar = PutChar;
	newObject->GetStringSB = GetStringSB;
	newObject->PutNumHex = PutNumHex;
	newObject->PutNumUB = PutNumUB;
	newObject->PutNumU = PutNumU;
	newObject->CheckChar = CheckChar;
  newObject->PutStringSB = PutStringSB;
	
}
