#include "GameEngine.h"
void GameSwitchTrigger(void);
void SwitchPress(GameEngine* self);
void newGameEngine(GameEngine* newObject, Terminal* terminal, Random* randomGen, Display* display, Timer* timer){
	newObject->Lives = 2;
	newObject->Score = 0;
	newObject->RunGame = RunGame;
	newObject->terminal = terminal;
	newObject->randomGen = randomGen;
	newObject->display = display;
	newObject->Clock = Clock;
	newObject->timer = timer;
	newObject->SwitchPress = SwitchPress;
	newObject->currentmill = 0;
	newObject->CurrentPress = 0;
	newObject->GameState = 0;
	//setup display
	
	display->Score = &newObject->Score;
	display->Lives = &newObject->Lives;
	display->PadState = &newObject->CurrentPadState;
}
void NewGame(GameEngine* self){
	self->display->TurnOffGreenLED();
	self->display->TurnOffRedLED();
	self->Lives = 2;
	self->Score = 0;
	self->CurrentPress = 0;
	//self->GameState = 0;
}
void RunGame(GameEngine* self){
	self->timer->StartTimer(self->timer);
	UInt16 toggle = 0;
	UInt8 Random8 = 0;
	UInt32 gametick = 0, displaytick = 0;
	UInt8 InGame = 0;
	UInt32 TimeOut = 0;
	while(TRUE){
		if(self->terminal->CheckChar() > 0){
			char input = self->terminal->GetChar();
			input = self->randomGen->GetRand(self->randomGen,10);
			self->terminal->PutNumUB(input);
		}
		if(self->currentmill > gametick){
			//self->terminal->PutNumHex(*self->timer->TimerCount);
      char* test = "\r\nTick\r\n";
      self->terminal->PutStringSB(test,20);
			self->terminal->PutNumUB(self->Score);
			switch(self->GameState){
				case 0:	//ready
					if((self->CurrentPress & 1) == 1){
						NewGame(self);
						self->GameState = 1;
					}
					break;
				case 1: //setupRound
					self->CurrentPadState = self->randomGen->GetRandSalt(self->randomGen,4, self->currentmill);
					self->GameState = 2;
					TimeOut = self->currentmill + 5000;
					continue;
					break;
				case 2:	//display
					if(self->currentmill < TimeOut){
						if(self->CurrentPadState == self->CurrentPress){
							if(self->CurrentPadState > 0){
								self->Score++;
								self->GameState = 3;
							}else{
								self->GameState = 1;								
							}
							self->CurrentPadState = 0;
							TimeOut = self->currentmill + 1000;
							continue;
						}else if((self->CurrentPress | self->CurrentPadState) != self->CurrentPadState){
							self->GameState = 4;
							self->CurrentPadState = 0;
							TimeOut = self->currentmill + 1000;
							continue;
						}
					}else{
						self->GameState = 4;
						self->CurrentPadState = 0;
						TimeOut = self->currentmill + 1000;
						continue;
					}
					break;
				case 3: //success
					self->CurrentPress = 0;
					if(TimeOut > self->currentmill){
						self->display->TurnOnGreenLED();
					}else{
						self->display->TurnOffGreenLED();
						self->GameState = 1;
					}
					break;
				case 4: //failure
					self->CurrentPress = 0;
					if(TimeOut > self->currentmill){
						self->display->TurnOnRedLED();
					}else if(self->Lives > 0){
						self->Lives--;
						self->display->TurnOffRedLED();
						self->GameState = 1;
					}else{
						self->display->TurnOffRedLED();
						self->GameState = 5;
					}
					break;
				case 5: //done
					break;
			}
			gametick = self->currentmill + 50;
		}
		if(self->currentmill > displaytick){
			self->display->Refresh(self->display);	
			displaytick = self->currentmill + 100;
		}
	}
	
}
void Wait(GameEngine* self, UInt32 ms){
	ms += self->currentmill;
	while(self->currentmill < ms);
}
void Clock(GameEngine* self){
	self->currentmill++;
}
void SwitchPress(GameEngine* self){
	switch(PORTC->ISFR){
		case (1 << 3):
					PORTC->PCR[3] |= PORT_PCR_ISF_MASK;
					self->CurrentPress |= 2;
			break;
		case (1 << 12):
					PORTC->PCR[12] |= PORT_PCR_ISF_MASK;
					self->CurrentPress |= 1;
			
			break;
		default:
					PORTC->ISFR |= 0xFFFFFFFF;
			break;
	}
}
