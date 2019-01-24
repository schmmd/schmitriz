#include "Main.h"
#include "Resource.h"
#include "dlgSimple.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

CMain Main;

HWND GetRunningWindow (char * szClass, char * szTitle)
{
    HWND hwnd = ::FindWindow (szClass, szTitle);
    if (::IsWindow (hwnd))
    {
        HWND hwndPopup = ::GetLastActivePopup (hwnd);
        if (::IsWindow (hwndPopup))
            hwnd = hwndPopup;
    }
    else 
        hwnd = 0;

    return hwnd;
}

void CMain::Init(HINSTANCE hInstance)
{
    HWND hwndOther = GetRunningWindow("Schmitriz", "Schmitriz");
    if (hwndOther != 0)
    {
        ::SetForegroundWindow (hwndOther);
        if (::IsIconic (hwndOther))
            ::ShowWindow (hwndOther, SW_RESTORE);
        exit(1);
    }

	Settings.Load();

	// Initialize Window
	App.SetName("Schmitriz");
	if (!App.InitWindow(hInstance, WindowProc, 300, 400, (char*)IDR_MENU, (char*)IDR_ACCEL, (char*)IDI_ICON1))
		FatalError("Could not create the window.");

	if (!DirectX.Init(App.GetWnd()))
		FatalError("Could not create the interface to DirectX.");

	App.ShowWindow();
	App.UpdateWindow();

	// Initialize Desktop Object
	Desktop.SetParent (NULL);
	Desktop.SetWindow (App.GetWnd());
	Desktop.SetCaption("Desktop");
	Desktop.SetClientRect(App.client);
	Desktop.SetSurface(DirectX.lpddsback);
	Desktop.Create();
	Desktop.Show();

	// Update fields and menus with settings
	Settings.Init();

	bInit = true;
	bExit = false;
	bActive = true;

	srand(GetTickCount());
}

void CMain::Quit()
{
	bExit = true;
	PostQuitMessage(0);
}

void CMain::Kill()
{
	DirectX.Kill();
	App.Kill();
	Settings.Save();
}

void CMain::Loop()
{
	UINT message;

	DWORD dwStartTime;

	// Real-time main message loop
	dwStartTime = 0;
	while (true)
	{
		if (App.HandleMessage(message))
		{
			if (message == WM_QUIT)
				return;
		}
		else if (bActive)
		{
			if (bInit && !bExit)
			{
				// Handle objects
				Desktop.CreateMessages();

				if (GetTickCount() - dwStartTime > 15 && bActive)
				{
					// Clear back
					// DirectX.ClearBackSurface(0);

					if (DirectX.CheckCoopLevel())
					{
						// Render
						HandleDXError(
							Desktop.Render(), 
							"Could not render the desktop.");

						// Flip to screen
						HandleDXError(
							DirectX.FlipBuffer(App.GetWnd()), 
							"Could not flip the buffer.");

						// Force cursor shown
						ShowCursor(TRUE);
					}

					// Update start time
					dwStartTime = GetTickCount();
				}
			}
		}
		else
		{
			// Free up resources (processor) when minimized
			WaitMessage();
		}
	}
}

void CMain::HandleDXError(HRESULT hr, char* szDefault)
{
	switch(hr)
	{
	case DD_OK:
		break;
	case DDERR_SURFACELOST:
		DirectX.RestoreSurfaces();
		break;
	default:
		//FatalError(szDefault);
		break;
	}
}

void CMain::FatalError(char* szErrorMessage)
{
	MessageBox(App.GetWnd(), szErrorMessage, App.GetName(), MB_ICONERROR);
	Kill(); // Free resources
	exit(-1);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CSimpleDlg dlgInstructions;
	static bool bPaused = 0;
	PAINTSTRUCT paint;
	int i;

	switch(msg)
	{
		case WM_ACTIVATEAPP:
			if ((BOOL)wParam == FALSE && Main.Settings.bPauseOnSwitch == true && Main.Desktop.Game->IsVisible())
			{
				bPaused = true;
				CheckMenuItem(Main.App.GetMenu(), IDM_FILE_PAUSE, bPaused ? MF_CHECKED : MF_UNCHECKED);
				Main.Desktop.Game->Pause(bPaused);
			}
			break;
		case WM_INITMENU:
			for (i = 0; i < Main.Desktop.Game->NumPlayers; i++)
			{
				Main.Desktop.Game->Controller[i].Gravity.Disable();
			}
			break;
		case WM_COMMAND:
			for (i = 0; i < Main.Desktop.Game->NumPlayers; i++)
			{
				Main.Desktop.Game->Controller[i].Gravity.Disable();
			}
			switch(LOWORD(wParam))
			{
				case IDM_FILE_NEW:
					if (Main.Desktop.Game->IsVisible() && Main.Desktop.Game->NumPlayers == 1 && Main.Desktop.Highscore.ScoreFits(Main.Desktop.Game->Controller[0].lCurScore))
					{
						Main.Desktop.Game->CloseGame();
						break;
						/*
						i = MessageBox(hWnd, "Would you like to continue? If you continue, your score will NOT be added to the highscore list. Please press 'Yes' to start a new game or 'No' to add your score to the highscore list.", Main.App.GetName(), MB_YESNO | MB_ICONSTOP);
						if (i == IDNO)
						{
							Main.Desktop.Game->CloseGame();
							break;
						}
						*/
					}
					for (i = 0; i < Main.Desktop.Game->NumPlayers; i++)
					{
						Main.Desktop.Game->Controller[i].NextShape.Kill();
					}

					Main.Desktop.Stats.SetAllForeColors(Main.Settings.rgbHighscores);
					Main.Desktop.Stats.Show();
					Main.Desktop.Title.Hide();
					Main.Desktop.Game->Hide();
					Main.Desktop.Highscore.Hide();
					Main.Desktop.NewGame.Show();
					break;

				case IDM_FILE_PAUSE:
					bPaused = !bPaused;
					CheckMenuItem(Main.App.GetMenu(), IDM_FILE_PAUSE, bPaused ? MF_CHECKED : MF_UNCHECKED);
					Main.Desktop.Game->Pause(bPaused);
					break;

				case IDM_FILE_END:
					if (Main.Desktop.Game->IsVisible())
						Main.Desktop.Game->End();
					break;

				case IDM_FILE_HIGHSCORE:
					if (Main.Desktop.Game->IsGameover() && Main.Desktop.Game->IsVisible())
					{
						Main.Desktop.Game->CloseGame();
						break;
					}
					Main.Desktop.Stats.SetAllForeColors(Main.Settings.rgbHighscores);
					Main.Desktop.Stats.Show();
					Main.Desktop.Title.Hide();
					Main.Desktop.Game->Hide();
					Main.Desktop.NewGame.Hide();
					Main.Desktop.Highscore.Show();
					break;
				case IDM_FILE_EXIT:
					DestroyWindow(Main.App.GetWnd());
					break;

				case IDM_OPTIONS_KEYS:
					KeySettingsDialog();
					break;

				case IDM_OPTIONS_TILESET:
					TilesetSettingsDialog();
					break;

				case IDM_OPTIONS_THEME:
					ThemeSettingsDialog();
					break;

				case IDM_OPTIONS_SHOWGRID:
					Main.Settings.bDisplayGrid = !Main.Settings.bDisplayGrid;
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWGRID, Main.Settings.bDisplayGrid ? MF_CHECKED : MF_UNCHECKED);
					break;

				case IDM_OPTIONS_SHOWHIGHSCORE1:
					Main.Settings.bShowHighscore[0] = !Main.Settings.bShowHighscore[0];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWHIGHSCORE1, Main.Settings.bShowHighscore[0] ? MF_CHECKED : MF_UNCHECKED);
					Main.Desktop.Stats.High[0].Show(Main.Settings.bShowHighscore[0]);
					Main.Desktop.Stats.HighStatic[0].Show(Main.Settings.bShowHighscore[0]);
					break;

				case IDM_OPTIONS_SHOWSCORE1:
					Main.Settings.bShowScore[0] = !Main.Settings.bShowScore[0];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWSCORE1, Main.Settings.bShowScore[0] ? MF_CHECKED : MF_UNCHECKED);
					Main.Desktop.Stats.Score[0].Show(Main.Settings.bShowScore[0]);
					Main.Desktop.Stats.ScoreStatic[0].Show(Main.Settings.bShowScore[0]);
					break;

				case IDM_OPTIONS_SHOWLEVEL1:
					Main.Settings.bShowLevel[0] = !Main.Settings.bShowLevel[0];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLEVEL1, Main.Settings.bShowLevel[0] ? MF_CHECKED : MF_UNCHECKED);
					Main.Desktop.Stats.Level[0].Show(Main.Settings.bShowLevel[0]);
					Main.Desktop.Stats.LevelStatic[0].Show(Main.Settings.bShowLevel[0]);
					break;

				case IDM_OPTIONS_SHOWLINES1:
					Main.Settings.bShowLines[0] = !Main.Settings.bShowLines[0];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLINES1, Main.Settings.bShowLines[0] ? MF_CHECKED : MF_UNCHECKED);
					Main.Desktop.Stats.Lines[0].Show(Main.Settings.bShowLines[0]);
					Main.Desktop.Stats.LinesStatic[0].Show(Main.Settings.bShowLines[0]);
					break;

				case IDM_OPTIONS_SHOWNEXTPIECE1:
					Main.Settings.bShowNextPiece[0] = !Main.Settings.bShowNextPiece[0];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWNEXTPIECE1, Main.Settings.bShowNextPiece[0] ? MF_CHECKED : MF_UNCHECKED);
					Main.Desktop.Stats.PiecePreview[0].Show(Main.Settings.bShowNextPiece[0]);
					Main.Desktop.Stats.NextStatic[0].Show(Main.Settings.bShowNextPiece[0]);
					break;

				case IDM_OPTIONS_SHOWHIGHSCORE2:
					Main.Settings.bShowHighscore[1] = !Main.Settings.bShowHighscore[1];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWHIGHSCORE2, Main.Settings.bShowHighscore[1] ? MF_CHECKED : MF_UNCHECKED);
					if (Main.Desktop.Game->NumPlayers != 1)
					{
						Main.Desktop.Stats.High[1].Show(Main.Settings.bShowHighscore[1]);
						Main.Desktop.Stats.HighStatic[1].Show(Main.Settings.bShowHighscore[1]);
					}
					break;

				case IDM_OPTIONS_SHOWSCORE2:
					Main.Settings.bShowScore[1] = !Main.Settings.bShowScore[1];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWSCORE2, Main.Settings.bShowScore[1] ? MF_CHECKED : MF_UNCHECKED);
					if (Main.Desktop.Game->NumPlayers != 1)
					{
						Main.Desktop.Stats.Score[1].Show(Main.Settings.bShowScore[1]);
						Main.Desktop.Stats.ScoreStatic[1].Show(Main.Settings.bShowScore[1]);
					}
					break;

				case IDM_OPTIONS_SHOWLEVEL2:
					Main.Settings.bShowLevel[1] = !Main.Settings.bShowLevel[1];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLEVEL2, Main.Settings.bShowLevel[1] ? MF_CHECKED : MF_UNCHECKED);
					if (Main.Desktop.Game->NumPlayers != 1)
					{
						Main.Desktop.Stats.Level[1].Show(Main.Settings.bShowLevel[1]);
						Main.Desktop.Stats.LevelStatic[1].Show(Main.Settings.bShowLevel[1]);
					}
					break;

				case IDM_OPTIONS_SHOWLINES2:
					Main.Settings.bShowLines[1] = !Main.Settings.bShowLines[1];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWLINES2, Main.Settings.bShowLines[1] ? MF_CHECKED : MF_UNCHECKED);
					if (Main.Desktop.Game->NumPlayers != 1)
					{
						Main.Desktop.Stats.Lines[1].Show(Main.Settings.bShowLines[1]);
						Main.Desktop.Stats.LinesStatic[1].Show(Main.Settings.bShowLines[1]);
					}
					break;

				case IDM_OPTIONS_SHOWNEXTPIECE2:
					Main.Settings.bShowNextPiece[1] = !Main.Settings.bShowNextPiece[1];
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_SHOWNEXTPIECE2, Main.Settings.bShowNextPiece[1] ? MF_CHECKED : MF_UNCHECKED);
					if (Main.Desktop.Game->NumPlayers != 1)
					{
						Main.Desktop.Stats.PiecePreview[1].Show(Main.Settings.bShowNextPiece[1]);
						Main.Desktop.Stats.NextStatic[1].Show(Main.Settings.bShowNextPiece[1]);
					}
					break;

				case IDM_OPTIONS_PAUSEON:
					Main.Settings.bPauseOnSwitch = !Main.Settings.bPauseOnSwitch;
					CheckMenuItem(Main.App.GetMenu(), IDM_OPTIONS_PAUSEON, Main.Settings.bPauseOnSwitch ? MF_CHECKED : MF_UNCHECKED);
					break;

				case IDM_HELP_INSTRUCTIONS:
					dlgInstructions.Run(Main.App.GetInstance(), Main.App.GetWnd(), IDD_INSTRUCTIONS);
					break;

				case IDM_HELP_SCHMITZTECH:
					ShellExecute(Main.App.GetWnd(), "Open", "http://www.schmitztech.com/", "", "", SW_SHOWNORMAL);
					break;

				case IDM_HELP_ABOUT:
					AboutDialog();
					break;
			}
			break;

		case WM_ERASEBKGND:
		{
			return true;
		}
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED || wParam == SIZE_MAXHIDE)
				Main.bActive = false;
			else
				Main.bActive = true;
		}
		case WM_PAINT:
		{
			if (Main.bInit && !Main.bExit && Main.bActive)
				if (Main.DirectX.FlipBuffer(Main.App.GetWnd()) == DDERR_SURFACELOST)
					Main.DirectX.RestoreSurfaces();

			BeginPaint(hWnd, &paint);
			EndPaint  (hWnd, &paint);

			break;
		}

		case WM_DESTROY:
		{
			Main.Quit();

			return 0;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)                      
{
	Main.Init(hInst);
	Main.Loop(     );
	Main.Kill(     );

	return 0;
}