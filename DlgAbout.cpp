#include "DlgTheme.h"

#include "Resource.h"
#include "Main.h"

BOOL CALLBACK AboutSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void AboutDialog()
{
	DialogBox (Main.App.GetInstance(), MAKEINTRESOURCE(IDD_ABOUT), Main.App.GetWnd(), &AboutSettingsProc);
}

BOOL CALLBACK AboutSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect, rect2;

	switch (message)
	{
		case WM_INITDIALOG:
			// Center dialog
			GetWindowRect(Main.App.GetWnd(), &rect);
			GetWindowRect(hWnd , &rect2);
			SetWindowPos (hWnd, HWND_TOP, ((rect.left + rect.right) / 2) - ((rect2.right - rect2.left) / 2), ((rect.top + rect.bottom) / 2) - ((rect2.bottom - rect2.top) / 2), 0, 0, SWP_NOSIZE);

			SetDlgItemText(hWnd, IDC_GAME, Main.Settings.szGameBackCopyright);
			SetDlgItemText(hWnd, IDC_HIGH, Main.Settings.szHighBackCopyright);

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
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