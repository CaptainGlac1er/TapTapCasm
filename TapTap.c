#include "GameEngine.h"
#include "Units.h"
#include "Terminal.h"
#include "Random.h"
#include "Display.h"
#include "Timer.h"
#include "Switch.h"

void MainClock(void);
void MainSwitchTrigger(void);

struct Terminal terminal;
struct GameEngine gameEngine;
struct Random	randomGen;
struct Display display;
struct Timer timer;
struct Switch sw1;
struct Switch sw3;

int main (void) {
	newRandom(&randomGen);
	newTerminal(&terminal);
	newDisplay(&display);
	newTimer(&timer);
	
		
	newSwitch(&sw1,3);
	
	newSwitch(&sw3,12);
	
	EnableSwitchInterrupts(&MainSwitchTrigger);
	
	newGameEngine(&gameEngine, &terminal, &randomGen, &display, &timer);
	timer.TriggerMethod(&MainClock);
	gameEngine.RunGame(&gameEngine);
  while (TRUE);
}
void MainClock(void){
	gameEngine.Clock(&gameEngine);
}
void MainSwitchTrigger(void){
	gameEngine.SwitchPress(&gameEngine);
	//terminal.PutChar((gameEngine.CurrentPress + '0'));
	//terminal.PutChar((gameEngine.CurrentPadState + '0'));
}
