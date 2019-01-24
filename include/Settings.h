#ifndef _H_SETTINGS
#define _H_SETTINGS

#include <windows.h>
#include <fstream.h>

class CSettings
{
public:
	void Load();
	void Init();
	void Save();

	void InitKeys();

	bool bImageShapeTiles;
	bool bDisplayGrid;

	bool bShowHighscore[2];
	bool bShowScore[2];
	bool bShowLevel[2];
	bool bShowLines[2];
	bool bShowNextPiece[2];

	bool bPauseOnSwitch;

	bool bRepeatHorKeys[2];
	bool bRepeatRotKeys[2];

	short iInitHorRepeatDelay[2];
	short iHorRepeatDelay[2];
	short iInitRotRepeatDelay[2];
	short iRotRepeatDelay[2];

	int iKeyLeft[2];
	int iKeyRight[2];
	int iKeyRotate[2];
	int iKeyRevRotate[2];
	int iKeyFast[2];
	int iKeyDrop[2];

	BYTE rGrid, gGrid, bGrid;
	COLORREF rgbGame, rgbHighscores;

	DWORD dwFastDelay;
	DWORD dwScoring;

	char szThemeDir  [48];
	char szTilesetDir[48];
	char szGameBackCopyright[128];
	char szHighBackCopyright[128];
};

#endif