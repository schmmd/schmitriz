#include "NewGame.h"

#include "Debug.h"

#include "Main.h"
#include "Desktop.h"
#include "Radio.h"
#include "resource.h"

int CGUINewGame::wm_command(CGUIObject* guiObject, DWORD dwCommand, long lParam)
{
	if (dwCommand == OM_SHOW && lParam == 1)
	{
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_NEW,       MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_PAUSE,     MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_END,       MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_HIGHSCORE, MF_ENABLED);
	}
	else if (dwCommand == OM_MOUSEUP && guiObject->GetID() == ID_NEWGAME_KEYS)
	{
		KeySettingsDialog();
	}
	else if ((guiObject->GetID() == ID_NEWGAME_START) && (dwCommand == OM_MOUSEUP || (dwCommand == OM_KEYUP && lParam == VK_RETURN)))
	{
		CGUIRadio* pLevel  = LevelRadio -> GetSelected();
		CGUIRadio* pHeight = HeightRadio-> GetSelected();
		CGUIRadio* pGame   = PlayersRadio->GetSelected();

		if (pLevel && pHeight && pGame)
		{
			if (strcmp(pGame->GetCaption(), "1") == 0)
				((CGUIDesktop*)pParent)->Game = &(((CGUIDesktop*)pParent)->Game1);
			else
				((CGUIDesktop*)pParent)->Game = &(((CGUIDesktop*)pParent)->Game2);

			((CGUIDesktop*)pParent)->Game->New(atoi(pLevel->GetCaption()), atoi(pHeight->GetCaption()));
			((CGUIDesktop*)pParent)->Stats.Show();
			((CGUIDesktop*)pParent)->Stats.SetAllForeColors(Main.Settings.rgbGame);
			Hide();
		}
		else
		{
			Debug("CGUINewGame: No radio selected");
			FatalError("No radio was selected.");
		}

		// Return false ends the propogation of WM_MOUSEDOWN and WM_KEYDOWN
		return false;
	}

	return CGUIObject::wm_command(guiObject, dwCommand, lParam);
}

void CGUINewGame::Create()
{
	COLORREF rgbStaticFore = RGB(0, 225, 175);
	COLORREF rgbButtonFore = RGB(175, 25, 30);
	COLORREF rgbButtonOver = RGB(255, 25, 30);
	COLORREF rgbRadioFore  = RGB(0  , 150, 100);
	COLORREF rgbRadioOver  = RGB(0, 255, 200);

	AddObject(&Background);
	Background.Init(0, "", Main.DirectX.lpddsHighBack);
	Background.SetClientRect(pParent->GetClientRect());
	Background.Show();

	int i;
	int y  = 84;
	int cy;
	int x;
	int cx;

	HDC hDC;

	if (SUCCEEDED(lpdds->GetDC(&hDC)))
	{
		char szBuffer[16];

		AddObject(&Players);
		Players.Init(0, "Number of Players:", rgbStaticFore);
		Players.SetPos(00, y);
		Players.SetSize(client.right, (cy = Players.GetTextHeight(hDC)));
		Players.Show();

		y += cy;

		cy = Players.GetTextHeight(hDC);

		strcpy(szBuffer, "1");
		AddObject(&PlayersRadio[0]);
		PlayersRadio[0].Init(ID_NEWGAME_PLAYERS, szBuffer, rgbRadioFore, rgbRadioOver, rgbRadioOver);
		PlayersRadio[0].Show();

		strcpy(szBuffer, "2");
		AddObject(&PlayersRadio[1]);
		PlayersRadio[1].Init(ID_NEWGAME_PLAYERS, szBuffer, rgbRadioFore, rgbRadioOver, rgbRadioOver);
		PlayersRadio[1].Show();

		cx = PlayersRadio[0].GetTextWidth(hDC) * 2; 
		x  = (client.right - (cx + PlayersRadio[1].GetTextWidth(hDC))) / 2;
		PlayersRadio[0].SetPos(x, y);
		PlayersRadio[0].AutoSize(hDC);
		x += cx;
		PlayersRadio[1].SetPos(x, y);
		PlayersRadio[1].AutoSize(hDC);

		PlayersRadio[0].Select();

		y += cy * 2;

		AddObject(&Level);
		Level.Init(0, "Starting Level:", rgbStaticFore);
		Level.SetPos(00, y);
		Level.SetSize(client.right, cy);
		Level.Show();

		y += cy;

		for (i = 0; i < 10; i++)
		{
			char szBuffer[2];
			sprintf(szBuffer, "%i", i);

			AddObject(&LevelRadio[i]);
			LevelRadio[i].Init(ID_NEWGAME_LEVEL, szBuffer, rgbRadioFore, rgbRadioOver, rgbRadioOver);
			LevelRadio[i].Show();
		}

		cx = LevelRadio[0].GetTextWidth(hDC);
		x  = (client.right - cx * 19) / 2;
		for (i = 0; i < 10; i++)
		{
			LevelRadio[i].SetPos(x, y);
			LevelRadio[i].AutoSize(hDC);
			x += cx * 2;
		}
		LevelRadio[0].Select();

		y += cy * 2;

		AddObject(&Height);
		Height.Init(0, "Starting Height:", rgbStaticFore);
		Height.SetPos(00, y);
		Height.SetSize(client.right, cy);
		Height.Show();

		y += cy;

		for (i = 0; i < 5; i++)
		{
			char szBuffer[2];
			sprintf(szBuffer, "%.2i", (i == 0) ? (0) : (1 + i * 3));

			AddObject(&HeightRadio[i]);
			HeightRadio[i].Init(ID_NEWGAME_HEIGHT, szBuffer, rgbRadioFore, rgbRadioOver, rgbRadioOver);
			HeightRadio[i].Show();
		}

		x  = (client.right - cx * 14) / 2;
		for (i = 0; i < 5; i++)
		{
			HeightRadio[i].SetPos(x, y);
			HeightRadio[i].AutoSize(hDC);
			x += cx * 3;
		}
		HeightRadio[0].Select();


		y += cy * 2;

		AddObject(&KeySettings);
		KeySettings.Init(ID_NEWGAME_KEYS, "Key Settings...", rgbButtonFore, rgbButtonOver);
		KeySettings.SetPos((client.right - KeySettings.GetTextWidth(hDC)) / 2, y);
		KeySettings.AutoSize(hDC);
		KeySettings.Show();

		y += cy + cy/2;

		AddObject(&Start);
		Start.Init(ID_NEWGAME_START, "Start Game", rgbButtonFore, rgbButtonOver);
		Start.SetPos((client.right - Start.GetTextWidth(hDC)) / 2, y);
		Start.AutoSize(hDC);
		Start.Show();

		lpdds->ReleaseDC(hDC);
	}
	else
		Debug("CGUINewGame: A handle to the device context could not be estasblished.");
}
