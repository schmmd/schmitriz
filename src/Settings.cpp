#include "Settings.h"

#include "Resource.h"
#include "Main.h"

void CSettings::Load()
{
	ifstream fin("settings.dat", ios::in | ios::binary | ios::nocreate);
	fin.setmode(filebuf::binary);

	if (fin.fd() == -1)
	{
		bImageShapeTiles = 1;
		bPauseOnSwitch   = 1;
		bShowHighscore[0]	= bShowHighscore[1]   = 1;
		bShowScore[0]		= bShowScore[1]       = 1;
		bShowLevel[0]		= bShowLevel[1]       = 1;
		bShowLines[0]		= bShowLines[1]       = 1;
		bShowNextPiece[0]	= bShowNextPiece[1]   = 0;
		bDisplayGrid		= 0;
		bRepeatHorKeys[0]	   = bRepeatHorKeys[1] = 1;
		bRepeatRotKeys[0]	   = bRepeatRotKeys[1] = 0;
		iInitHorRepeatDelay[0] = iInitHorRepeatDelay[1] = 500;
		iHorRepeatDelay[0]     = iHorRepeatDelay[1]     = 100;
		iInitRotRepeatDelay[0] = iInitRotRepeatDelay[1] = 400;
		iRotRepeatDelay[0]     = iRotRepeatDelay[1]     = 250;
		iKeyRotate[0]    = 48 + 5;
		iKeyRevRotate[0] = 48 + 8;
		iKeyLeft[0]      = 48 + 4;
		iKeyRight[0]     = 48 + 6;
		iKeyDrop[0]      = 48 + 0;
		iKeyFast[0]      = VK_ADD;
		iKeyRotate[1]    = 'K';
		iKeyRevRotate[1] = 'I';
		iKeyLeft[1]      = 'J';
		iKeyRight[1]     = 'L';
		iKeyDrop[1]      = VK_SPACE;
		iKeyFast[1]      = 'A';
		dwFastDelay      = 25;
		dwScoring          = 1;
		rgbHighscores	   = RGB(200, 200, 200);
		rgbGame			   = RGB(075, 075, 075);
		rGrid			   = 150;
		gGrid			   = 150;
		bGrid			   = 150;
		sprintf(szGameBackCopyright, "Game picture by Mikhail Popenov of Fedoskino and from Sunbirds.com");
		sprintf(szHighBackCopyright, "Highscore picture from Nasa.gov");
		sprintf(szThemeDir  , "themes\\rusbw");
		sprintf(szTilesetDir, "tilesets\\wacky.bmp");

		Save();
	}
	else
	{
		fin.read((char*)this, sizeof(CSettings));
	}

	fin.close();
}

void CSettings::Init()
{
	int i;

	InitKeys();

	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_PAUSEON,			bPauseOnSwitch		? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWGRID,			bDisplayGrid		? MF_CHECKED : MF_UNCHECKED);

	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWHIGHSCORE1,	bShowHighscore[0]	? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWSCORE1,		bShowScore[0]		? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLEVEL1,		bShowLevel[0]		? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLINES1,		bShowLines[0]		? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWNEXTPIECE1,	bShowNextPiece[0]	? MF_CHECKED : MF_UNCHECKED);

	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWHIGHSCORE2,	bShowHighscore[1]	? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWSCORE2,		bShowScore[1]		? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLEVEL2,		bShowLevel[1]		? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLINES2,		bShowLines[1]		? MF_CHECKED : MF_UNCHECKED);
	CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWNEXTPIECE2,	bShowNextPiece[1]	? MF_CHECKED : MF_UNCHECKED);

	for (i = 0; i < 2; i++)
	{
		Main.Desktop.Stats.High[i].Show(bShowHighscore[i]);
		Main.Desktop.Stats.HighStatic[i].Show(bShowHighscore[i]);
		Main.Desktop.Stats.Score[i].Show(bShowScore[i]);
		Main.Desktop.Stats.ScoreStatic[i].Show(bShowScore[i]);
		Main.Desktop.Stats.Level[i].Show(bShowLevel[i]);
		Main.Desktop.Stats.LevelStatic[i].Show(bShowLevel[i]);
		Main.Desktop.Stats.Lines[i].Show(bShowLines[i]);
		Main.Desktop.Stats.LinesStatic[i].Show(bShowLines[i]);
		Main.Desktop.Stats.PiecePreview[i].Show(bShowNextPiece[i]);
		Main.Desktop.Stats.NextStatic[i].Show(bShowNextPiece[i]);
	}
}

void CSettings::InitKeys()
{
	for (int i = 0; i < Main.Desktop.Game->NumPlayers; i++)
	{
		Main.Desktop.Game->Controller[i].KeyRotate   .SetKey(iKeyRotate[i]   );
		Main.Desktop.Game->Controller[i].KeyRevRotate.SetKey(iKeyRevRotate[i]);
		Main.Desktop.Game->Controller[i].KeyLeft     .SetKey(iKeyLeft[i]     );
		Main.Desktop.Game->Controller[i].KeyRight    .SetKey(iKeyRight[i]    );
		Main.Desktop.Game->Controller[i].KeyDrop     .SetKey(iKeyDrop[i]     );
		Main.Desktop.Game->Controller[i].KeyFast     .SetKey(iKeyFast[i]     );

		Main.Desktop.Game->Controller[i].KeyLeft     .SetRepeat(Main.Settings.iInitHorRepeatDelay[i], Main.Settings.iHorRepeatDelay[i]);
		Main.Desktop.Game->Controller[i].KeyRight    .SetRepeat(Main.Settings.iInitHorRepeatDelay[i], Main.Settings.iHorRepeatDelay[i]);
		Main.Desktop.Game->Controller[i].KeyRotate   .SetRepeat(Main.Settings.iInitRotRepeatDelay[i], Main.Settings.iRotRepeatDelay[i]);
		Main.Desktop.Game->Controller[i].KeyRevRotate.SetRepeat(Main.Settings.iInitRotRepeatDelay[i], Main.Settings.iRotRepeatDelay[i]);
	}}

void CSettings::Save()
{
	ofstream fout("settings.dat", ios::out | ios::binary);
	fout.setmode(filebuf::binary);

	fout.write((char*)this, sizeof(CSettings));

	fout.close();
}