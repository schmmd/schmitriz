#ifndef _H_HIGHSCORE
#define _H_HIGHSCORE

#include <stdio.h>
#include <fstream.h>

#include "Object.h"
#include "Image.h"
#include "Static.h"
#include "Text.h"
#include "Button.h"

class CGUIHighscore : public CGUIObject
{
public:
	void Create();

	void Save();
	void Load();

	bool  ScoreFits(long lScore);
	void  NewScore(long lNewScore);
	short Find(long lScore);
	void  ShiftDown(short pos);
	void  Engrave(CGUIText* pObj);
	
	char* GetHighscore();

	virtual int wm_command(CGUIObject* guiObject, DWORD dwCommand, long lParam);

private:
	CGUIImage  Background;
	CGUIStatic Head;
	CGUIStatic EnterName;
	CGUIButton Newgame;
	CGUIButton Splash;
	CGUIText   Name [10];
	CGUIText   Score[10];

	bool bEngraved;
};

#define ID_HIGHSCORE_NAME   1
#define ID_HIGHSCORE_SCORE  2
#define ID_HIGHSCORE_NEW    3
#define ID_HIGHSCORE_SPLASH 4

#endif