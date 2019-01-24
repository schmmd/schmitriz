#include "Highscore.h"

#include "Desktop.h"
#include "Main.h"
#include "resource.h"

#define MAXNAMESIZE  15
#define MAXSCORESIZE 5

void CGUIHighscore::Create()
{
	COLORREF rgbFore = RGB(0  , 200, 165);
	SIZE size;
	HDC hDC;
	int y, x1, x2, wid1, wid2, i;

	AddObject(&Background);
	Background.Init(0, "", Main.DirectX.lpddsHighBack);
	Background.SetClientRect(pParent->GetClientRect());
	Background.Show();

	if (SUCCEEDED(lpdds->GetDC(&hDC)))
	{
		y = 60;
		GetTextExtentPoint32(hDC, "0", 1, &size);

		wid1 = size.cx * MAXNAMESIZE;
		wid2 = size.cx * MAXSCORESIZE;
		x1 = (client.right - (wid1 + size.cx / 2 + wid2)) / 2;
		x2 = x1 + wid1 + size.cx / 2;

		AddObject(&EnterName);
		EnterName.Init(0, "Please enter you name...", RGB(0, 255, 255));
		EnterName.SetPos(0, 40);
		EnterName.SetSize(client.right, size.cy);

		AddObject(&Head);
		Head.Init(0, "Highscores", rgbFore);
		Head.SetPos (0, y);
		Head.SetSize(client.right, size.cy);
		Head.Show();

		y += size.cy;

		for (i = 0; i < 10; i++)
		{
			AddObject((Name + i));
			Name[i].Init(ID_HIGHSCORE_NAME, "", rgbFore, 1, DT_LEFT, MAXNAMESIZE);
			AddObject((Score + i));
			Score[i].Init(ID_HIGHSCORE_SCORE, "", rgbFore, 1, DT_RIGHT, MAXSCORESIZE);

			Name [i].SetPos (x1, y);
			Name [i].SetSize(wid1, size.cy);
			Score[i].SetPos (x2, y);
			Score[i].SetSize(wid2, size.cy);

			Name[i] .Show();
			Score[i].Show();

			y += size.cy;
		}

		y += size.cy;

		AddObject(&Newgame);
		Newgame.Init(ID_HIGHSCORE_NEW, "New Game", RGB(175, 25, 30), RGB(255, 25, 30));
		Newgame.SetPos((client.right - Newgame.GetTextWidth(hDC)) / 2, y);
		Newgame.AutoSize(hDC);
		Newgame.Show();

		y += size.cy * 2;

		AddObject(&Splash);
		Splash.Init(ID_HIGHSCORE_SPLASH, "Splash Screen", RGB(175, 25, 30), RGB(255, 25, 30));
		Splash.SetPos((client.right - Splash.GetTextWidth(hDC)) / 2, y);
		Splash.AutoSize(hDC);
		Splash.Show();

		lpdds->ReleaseDC(hDC);
	}
	else
		Debug("CGUIHighscore: A handle to the device context could not be estasblished.");

	Load();
}

int CGUIHighscore::wm_command(CGUIObject* guiObject, DWORD dwCommand, long lParam)
{
	if (dwCommand == OM_SHOW && lParam == 1)
	{
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_NEW,       MF_ENABLED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_PAUSE,     MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_END,       MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_HIGHSCORE, MF_GRAYED);

		bEngraved = true;
	}
	else if (dwCommand == OM_KEYUP)
	{
		if (lParam == VK_RETURN)
		{
			if (!bEngraved)
			{
				if ((guiObject->GetID() == ID_HIGHSCORE_NAME) && (!((CGUIText*)(guiObject))->IsLocked()))
				{
					Engrave((CGUIText*)guiObject);
					bEngraved = true;
					return 1;
				}
			}
			else if (guiObject->GetID() == 4)
			{
				// New game
				Hide();
				((CGUIDesktop*)(pParent))->Stats.Hide();
				// ((CGUIDesktop*)(pParent))->Stats.SetAllForeColors(Main.Settings.rgbHighscores);
				// ((CGUIDesktop*)(pParent))->Stats.ShowIndex(0, 0);
				// ((CGUIDesktop*)(pParent))->Stats.ShowIndex(1, 0);
				((CGUIDesktop*)(pParent))->NewGame.Show();

				/*
				for (int i = 0; i < 10; i++)
				{
					if (!Name[i].IsLocked())
					{
						Engrave(Name + i);
					}
				}
				*/
			}
		}
	}
	if (dwCommand == OM_MOUSEUP)
	{
		if (guiObject->GetID() == ID_HIGHSCORE_NEW)
		{
			// New game
			Hide();
			((CGUIDesktop*)(pParent))->Stats.Hide();
			// ((CGUIDesktop*)(pParent))->Stats.SetAllForeColors(Main.Settings.rgbHighscores);
			// ((CGUIDesktop*)(pParent))->Stats.ShowIndex(0, 0);
			// ((CGUIDesktop*)(pParent))->Stats.ShowIndex(1, 0);
			((CGUIDesktop*)(pParent))->NewGame.Show();

			for (int i = 0; i < 10; i++)
			{
				if (!Name[i].IsLocked())
				{
					Engrave(Name + i);
				}
			}
		}
		else if (guiObject->GetID() == ID_HIGHSCORE_SPLASH)
		{
			// Splash screen
			Hide();
			((CGUIDesktop*)(pParent))->Stats.Hide();
			((CGUIDesktop*)(pParent))->Title.Show();
		}
	}

	return CGUIObject::wm_command(guiObject, dwCommand, lParam);
}

void CGUIHighscore::Load()
{
	char szHighname [MAXNAMESIZE ];
	char szHighscore[MAXSCORESIZE];

	sprintf(szHighscore, "highscore.dat");
	ifstream fin(szHighscore, ios::nocreate);

	if (fin.fd() == -1)
	{
		sprintf(szHighname , "None");
		sprintf(szHighscore, "0"   );
		for (int i = 0; i < 10; i++)
		{
			Name [i].SetCaption(szHighname );
			Score[i].SetCaption(szHighscore);
		}

		Save();
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			fin.getline(szHighname , MAXNAMESIZE  + 1);
			if ((fin.peek()) == 10)
				fin.get(); // Trash line feed
			fin.getline(szHighscore, MAXSCORESIZE + 1);
			if (fin.peek() == 10)
				fin.get(); // Trash line feed

			Name [i].SetCaption(szHighname );
			Score[i].SetCaption(szHighscore);
		}
	}

	fin.close();
}

void CGUIHighscore::Save()
{
	char* szHighscore = "highscore.dat";

	ofstream fout(szHighscore);

	for (int i = 0; i < 10; i++)
	{
		fout << Name [i].GetCaption() << endl;
		fout << Score[i].GetCaption() << endl;
	}

	fout.close();
}

void CGUIHighscore::ShiftDown(short pos)
{
	if (pos < 10)
	{
		for (int i = 9; i > pos; --i)
		{
			// Make room for new highscore
			// Copy name
			sprintf(Name [i].GetCaption(), Name [i - 1].GetCaption());
			// Copy score
			sprintf(Score[i].GetCaption(), Score[i - 1].GetCaption());
		}
	}
}

void CGUIHighscore::Engrave(CGUIText* pObj)
{
	pObj->SetForeColor(RGB(0  , 200, 165));
	pObj->Lock();
	EnterName.Hide();

	Save();
}

bool CGUIHighscore::ScoreFits(long lScore)
{
	if (lScore > atoi(Score[9].GetCaption()))
		return true;
	else
		return false;
}

void CGUIHighscore::NewScore(long lNewScore)
{
	if (ScoreFits(lNewScore))
	{
		// Should be placed
		short pos = Find(lNewScore);
		if (pos != 10)
		{
			ShiftDown(pos);

			char szBuffer[32];
			sprintf(szBuffer, "%i", lNewScore);

			Score[pos].SetCaption(szBuffer);
			Name [pos].Unlock();
			Name [pos].SetForeColor(RGB(0, 255, 255));
			EnterName.Show();

			bEngraved = false;
		}
	}
}

short CGUIHighscore::Find(long lScore)
{
	long Pos = 9;

	while (Pos >= 0)
	{
		if (atoi(Score[Pos].GetCaption()) < lScore)
			--Pos;
		else
			return Pos + 1;
	}

	return 0;
}

char* CGUIHighscore::GetHighscore()
{
	return Score[0].GetCaption();
}
