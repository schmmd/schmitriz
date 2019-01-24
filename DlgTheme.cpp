#include "DlgTheme.h"

#include "Resource.h"
#include "Main.h"

BOOL CALLBACK ThemeSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void ThemeSettingsDialog()
{
	DialogBox (Main.App.GetInstance(), MAKEINTRESOURCE(IDD_THEME), Main.App.GetWnd(), &ThemeSettingsProc);
}

void ExtractColor(ifstream& fin, COLORREF& rgb)
{
	char szBuffer[4];

	int r,g,b;

	fin.getline(szBuffer, 4);
	r = atoi(szBuffer);

	fin.ignore();

	fin.getline(szBuffer, 4);
	g = atoi(szBuffer);

	fin.ignore();

	fin.getline(szBuffer, 4);
	b = atoi(szBuffer);

	fin.ignore();

	rgb = RGB(r, g, b);
}

void ExtractColor(ifstream& fin, BYTE& r, BYTE& g, BYTE& b)
{
	char szBuffer[4];

	fin.getline(szBuffer, 4);
	r = atoi(szBuffer);

	fin.ignore();

	fin.getline(szBuffer, 4);
	g = atoi(szBuffer);

	fin.ignore();

	fin.getline(szBuffer, 4);
	b = atoi(szBuffer);

	fin.ignore();
}

BOOL CALLBACK ThemeSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ifstream fin;
	long sel;
	char szBuffer [128];
	char szBuffer2[128];
	int i;
	RECT rect, rect2;

	switch (message)
	{
		case WM_INITDIALOG:
			// Center dialog
			GetWindowRect(Main.App.GetWnd(), &rect);
			GetWindowRect(hWnd , &rect2);
			SetWindowPos (hWnd, HWND_TOP, ((rect.left + rect.right) / 2) - ((rect2.right - rect2.left) / 2), ((rect.top + rect.bottom) / 2) - ((rect2.bottom - rect2.top) / 2), 0, 0, SWP_NOSIZE);

			// Setup listbox
			fin.open("themes.idx");
			
			// Ignore comments
			while (fin.peek() == ';')
				fin.ignore(1280,'\n');

			while (!fin.eof())
			{
				fin.getline(szBuffer, 128);
				SendDlgItemMessage(hWnd, IDC_LIST , LB_ADDSTRING, 0, (LPARAM)&szBuffer);

				// Ignore lines
				for (i = 0; i < 6; i++)
					fin.ignore(1280,'\n');
			}
			fin.close();

			return 1;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_LIST:
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						// Extract tilset dirrectory from themes.idx
						sel = SendDlgItemMessage(hWnd, IDC_LIST, LB_GETCURSEL, 0, 0);
						if (sel != LB_ERR)
						{
							SendDlgItemMessage(hWnd, IDC_LIST, LB_GETTEXT, sel, (LPARAM)&szBuffer);

							fin.open("themes.idx");

							// Ignore lines
							while (fin.peek() == ';')
								fin.ignore(1280,'\n');

							while (!fin.eof())
							{
								fin.getline(szBuffer2, 128);
								if (strcmp(szBuffer, szBuffer2) == 0)
								{
									fin.getline(szBuffer, 128);
									if (szBuffer[0] != NULL)
									{
										sprintf(Main.Settings.szThemeDir, "%s", szBuffer);
										// Main.DirectX.ReleaseTheme();
										Main.DirectX.LoadTheme();

										fin.getline(Main.Settings.szGameBackCopyright, 128);
										fin.getline(Main.Settings.szHighBackCopyright, 128);

										ExtractColor(fin, Main.Settings.rgbGame);
										ExtractColor(fin, Main.Settings.rgbHighscores);
										ExtractColor(fin, Main.Settings.rGrid, Main.Settings.gGrid, Main.Settings.bGrid);

										fin.close();

										if (Main.Desktop.Game->IsVisible())
											Main.Desktop.Stats.SetAllForeColors(Main.Settings.rgbGame);
										else
											Main.Desktop.Stats.SetAllForeColors(Main.Settings.rgbHighscores);
									}

									break;
								}
							}
							fin.close();
						}

						SendMessage(hWnd, WM_DESTROY, 0, 0);
					}
					break;

				case IDCANCEL:
					SendMessage(hWnd, WM_DESTROY, 0, 0);
					return 1;
			}
		break;

		case WM_DESTROY:
			EndDialog(hWnd, 0);
			return 1;
	}

	return 0;
}