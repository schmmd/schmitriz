#ifndef _H_DESKTOP_
#define _H_DESKTOP_

#include "Object.h"
#include "Image.h"

#include "Title.h"
#include "Game.h"
#include "Stats.h"
#include "NewGame.h"
#include "Highscore.h"

class CGUIDesktop : public CGUIObject
{
public:
	CGUIDesktop();
	~CGUIDesktop();

	void CreateMessages();

	void Create();

	HRESULT Render();

	// Children
	CGUITitle     Title;
	CGUIGame*     Game;
	CGUIGame1     Game1;
	CGUIGame2	  Game2;
	CGUIStats     Stats;
	CGUINewGame   NewGame;
	CGUIHighscore Highscore;

private:
	POINT mouse;
	SHORT mousestate;

	BYTE* keyboard;
	BYTE* oldkeyboard;
};

#define TITLE		0
#define SETTINGS	1
#define PLAYING		2
#define PAUSED		3
#define HIGHSCORE	4
#define ENTERNAME	5

// Macros
#ifndef DOWN
	#define DOWN(vk_code) ((vk_code & 0x80) ? 1 : 0)
#endif
#ifndef UP
	#define UP(vk_code)   ((vk_code & 0x80) ? 0 : 1)
#endif

#endif