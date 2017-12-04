#ifndef GameEngineImport
#define GameEngineImport
#include "Units.h"
#include "Terminal.h"
#include "Random.h"
#include "Display.h"
#include "Timer.h"

//#include <MKL46Z4.h>

typedef struct GameEngine
{
	Terminal* terminal;
	Random* randomGen;
	Display* display;
	Timer* timer;
	UInt8 Lives;
	UInt8 Score;
	UInt8 CurrentPadState;
	UInt32 currentmill;
	UInt8 GameState;
	UInt8 CurrentPress;
	void (*RunGame) (struct GameEngine*);
	void (*Clock) (struct GameEngine*);
	void (*SwitchPress) (struct GameEngine*);
} GameEngine;
void newGameEngine(GameEngine* newObject, Terminal* terminal, Random* random, Display* display, Timer* timer);
void Clock(GameEngine* self);
void RunGame(GameEngine* self);
#endif
