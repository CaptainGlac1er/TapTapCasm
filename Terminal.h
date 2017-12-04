#ifndef TerminalImport
#define TerminalImport
#include "Units.h"

char GetChar (void);
UInt8 CheckChar (void);
void GetStringSB (char String[], int StringBufferCapacity);
void Init_UART0_IRQ (void);
void PutChar (char Character);
void PutNumHex (UInt32 input);
void PutNumUB (UInt8 input);
void PutNumU (UInt8 input);
void PutStringSB (char String[], int StringBufferCapacity);
void SetupUARTVariables(void);
void UART0_IRQHandler(void);


typedef struct Terminal{
	void (*PutChar) (char Character);
	char (*GetChar) (void);
	void (*GetStringSB) (char String[], int StringBufferCapacity);
	void (*PutStringSB) (char String[], int StringBufferCapacity);
	void (*PutNumHex) (UInt32 input);
	void (*PutNumUB) (UInt8 inputt);
	void (*PutNumU) (UInt8 input);
	UInt8 (*CheckChar) (void);
	//void (*PutChar) (struct Random*, UInt8 max);
	
	
} Terminal;
void newTerminal(Terminal* newObject);
#endif
