#ifndef _H_NEWGAME
#define _H_NEWGAME

#include <stdio.h>

#include "Object.h"
#include "Image.h"
#include "Static.h"
#include "Button.h"
#include "Radio.h"

class CGUINewGame : public CGUIObject
{
public:
	void Create();

	virtual int wm_command(CGUIObject* guiObject, DWORD dwCommand, long lParam);

private:
	CGUIImage  Background;
	CGUIStatic Players, Level, Height;
	CGUIButton Start, KeySettings;
	CGUIRadio  LevelRadio [10];
	CGUIRadio  HeightRadio[5];
	CGUIRadio  PlayersRadio [2];
};

#define ID_NEWGAME_PLAYERS 1
#define ID_NEWGAME_START   2
#define ID_NEWGAME_LEVEL   3
#define ID_NEWGAME_HEIGHT  4
#define ID_NEWGAME_KEYS    5

#endif