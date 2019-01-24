#include "dlgSimple.h"

#include "Resource.h"
#include "Main.h"

bool CSimpleDlg::bOK = false;

BOOL CALLBACK AboutSettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool CSimpleDlg::Run(HINSTANCE hInstance, HWND hwnd, int iResource)
{
	DialogBox (hInstance, (char*)iResource, hwnd, &SimpleDlgProc);
	return bOK;
}

BOOL CALLBACK CSimpleDlg::SimpleDlgProc
   (HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
	RECT rect, rect2;

    switch (message)
    {
    case WM_INITDIALOG:
		// Center dialog
		GetWindowRect(GetParent(hwnd), &rect);
		GetWindowRect(hwnd , &rect2);
		SetWindowPos (hwnd, HWND_TOP, ((rect.left + rect.right) / 2) - ((rect2.right - rect2.left) / 2), ((rect.top + rect.bottom) / 2) - ((rect2.bottom - rect2.top) / 2), 0, 0, SWP_NOSIZE);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
			bOK = true;
		default:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}