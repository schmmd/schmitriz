#include "DlgTileset.h"

#include "Resource.h"
#include "Main.h"

BOOL CALLBACK TilesetSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void TilesetSettingsDialog()
{
	DialogBox (Main.App.GetInstance(), MAKEINTRESOURCE(IDD_TILESET), Main.App.GetWnd(), &TilesetSettingsProc);
}

void ReloadTileset()
{
	Main.DirectX.ReleaseTileset();
	Main.DirectX.LoadTileset();
}

void LoadTilesetFromTitle(char* szBuffer)
{
	ifstream fin;

	char szBuffer2[48];

	fin.open("tilesets.idx");
	while (!fin.eof())
	{
		fin.getline(szBuffer2, 48);
		if (strcmp(szBuffer, szBuffer2) == 0)
		{
			fin.getline(szBuffer, 48);
			if (szBuffer[0] != NULL)
			{
				fin.close();
				sprintf(Main.Settings.szTilesetDir, "%s", szBuffer);
				ReloadTileset();
			}

			break;
		}
	}
	fin.close();
}

BOOL CALLBACK TilesetSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ifstream fin;
	long sel;
	char szBuffer[48];
	RECT rect, rect2;

	switch (message)
	{
		case WM_INITDIALOG:
			// Center dialog
			GetWindowRect(Main.App.GetWnd(), &rect);
			GetWindowRect(hWnd , &rect2);
			SetWindowPos (hWnd, HWND_TOP, ((rect.left + rect.right) / 2) - ((rect2.right - rect2.left) / 2), ((rect.top + rect.bottom) / 2) - ((rect2.bottom - rect2.top) / 2), 0, 0, SWP_NOSIZE);

			// Setup listbox
			fin.open("tilesets.idx");
			while (!fin.eof())
			{
				fin.getline(szBuffer, 48);
				SendDlgItemMessage(hWnd, IDC_LIST , LB_ADDSTRING, 0, (LPARAM)&szBuffer);
				fin.getline(szBuffer, 48);
			}
			fin.close();

			return 1;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_LIST:
					if (HIWORD(wParam) == LBN_SELCHANGE)
					{
						// Extract tilset dirrectory from tilesets.idx
						sel = SendDlgItemMessage(hWnd, IDC_LIST, LB_GETCURSEL, 0, 0);
						if (sel != LB_ERR)
						{
							SendDlgItemMessage(hWnd, IDC_LIST, LB_GETTEXT, sel, (LPARAM)&szBuffer);
							LoadTilesetFromTitle(szBuffer);
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