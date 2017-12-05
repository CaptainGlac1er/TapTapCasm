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
	UInt32 ReactionDelay = 2500;
	while(TRUE){
		if(self->terminal->CheckChar() > 0){
			UInt8 input = self->terminal->GetChar();
			switch(self->GameState){
				case 0:
					switch(input){
						case 'w':
							self->CurrentPress |= 1;
							break;
						default:
							break;
					}
					break;
				case 2:
					switch(input){
						case 'a':
							self->CurrentPress |= 1;
							break;
						case 'd':
							self->CurrentPress |= 2;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
			//input = self->randomGen->GetRand(self->randomGen,10);
			//self->terminal->PutNumUB(input);
		}
		if(self->currentmill > gametick){
			//self->terminal->PutNumHex(*self->timer->TimerCount);
      //char* test = "\r\nTick\r\n";
      //self->terminal->PutStringSB(test,20);
			//self->terminal->PutNumUB(self->Score);
			switch(self->GameState){
				case 0:	//ready
					if(self->terminal->DontUpdate == 0){
						self->terminal->DontUpdate = 1;
						self->terminal->NeedUpdate = 1;
					}
					if((self->CurrentPress & 1) == 1){
						NewGame(self);
						self->GameState = 1;
						self->terminal->DontUpdate = 0;
					}
					break;
				case 1: //setupRound
					self->CurrentPadState = self->randomGen->GetRandSalt(self->randomGen,4, self->currentmill);
					self->terminal->NeedUpdate = 1;
					if(self->CurrentPress != 0){
						self->GameState = 4;
						TimeOut = self->currentmill + 1000;
					}else{
						self->GameState = 2;
						TimeOut = self->currentmill + ReactionDelay;
						if(ReactionDelay > 500){
							ReactionDelay -= 10;
						}
					}
					continue;
				case 2:	//display
					if(self->currentmill < TimeOut){
						if(self->CurrentPadState == self->CurrentPress){
							if(self->CurrentPadState > 0){
								self->Score++;
								self->terminal->NeedUpdate = 1;
								self->GameState = 3;
								self->CurrentPress = 0;
							}else{
								self->GameState = 1;								
							}
							self->CurrentPadState = 0;
							TimeOut = self->currentmill + 1000;
							continue;
						}else if((self->CurrentPress | self->CurrentPadState) != self->CurrentPadState){
							self->GameState = 4;
							self->terminal->NeedUpdate = 1;
							self->CurrentPress = 0;
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
								self->terminal->NeedUpdate = 1;
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
			if(self->terminal->NeedUpdate == 1){
				char* string;	
				self->terminal->PutChar('\r');
				string = "                                    ";
				self->terminal->PutStringSB(string, 40);	
				if(self->GameState == 0){
					self->terminal->PutChar('\r');
					string = "Press w or left trigger on hardware";
					self->terminal->PutStringSB(string, 40);
					
				}
				if(self->GameState == 2 || self->GameState == 3 || self->GameState == 4){
					self->terminal->PutChar('\r');
					string = "Score: ";
					self->terminal->PutStringSB(string, 8);
					self->terminal->PutNumUB(self->Score);
					self->terminal->PutChar(' ');
					string = "Lives: ";
					self->terminal->PutStringSB(string, 8);
					self->terminal->PutNumUB(self->Lives);
					self->terminal->PutChar(' ');				
				}
				if(self->GameState == 2){
					switch(self->CurrentPadState){
						case 0:
							string = "__";
						break;
						case 1:
							string = "0_";
						break;
						case 2:
							string = "_0";
						break;
						case 3:
							string = "00";
						break;
					}
					self->terminal->PutStringSB(string, 3);
					self->terminal->PutChar(' ');
				}else if(self->GameState == 3 || self->GameState == 4){
					string = "  ";
					self->terminal->PutStringSB(string, 3);
				}
				self->terminal->NeedUpdate = 0;
			}
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
