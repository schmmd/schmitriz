#include "DlgKeys.h"

#include "Main.h"
#include "resource.h"

#include <assert.h>

#ifndef KEYDOWN
	#define KEYDOWN(vk_code) ((GetKeyState(vk_code) & 0x8000) ? 1 : 0)
#endif
#ifndef KEYUP
	#define KEYUP(vk_code)   ((GetKeyState(vk_code) & 0x8000) ? 0 : 1)
#endif

BOOL CALLBACK KeySettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

bool VirtualToText(short iVirtual, char* szText);
int RunModalWindow(HWND hwndDialog, HWND hwndParent);

bool bDone;

void KeySettingsDialog()
{
	bDone = false;

	HWND hParent = Main.App.GetWnd();
	HWND hDialog = CreateDialog(Main.App.GetInstance(), MAKEINTRESOURCE(IDD_KEYS), hParent, &KeySettingsProc);

	RunModalWindow(hDialog, hParent);
	// DialogBox (Main.App.GetInstance(), MAKEINTRESOURCE(IDD_KEYS), Main.App.GetWnd(), &KeySettingsProc);
}

int RunModalWindow(HWND hwndDialog, HWND hwndParent)
{
	if(hwndParent != NULL)
		EnableWindow(hwndParent,FALSE);

	ShowWindow(hwndDialog, TRUE);

	MSG msg;

	while(!bDone)
	{
		WaitMessage();

		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				bDone = true;
				PostMessage(NULL, WM_QUIT, 0, 0);
				break;
			}
			else if (msg.message == WM_DESTROY)
			{
				bDone = true;
				break;
			}

			if(!IsDialogMessage(hwndDialog, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_KEYDOWN)
			{
				KeySettingsProc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			}
		}
	}

	if(hwndParent != NULL)
		EnableWindow(hwndParent,TRUE);

	DestroyWindow(hwndDialog);

	return 1;
}

struct keycodes
{
	int iKeyLeft[2];
	int iKeyRight[2];
	int iKeyRotate[2];
	int iKeyRevRotate[2];
	int iKeyFast[2];
	int iKeyDrop[2];
};

BOOL CALLBACK KeySettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static char szBuffer[48];
	static char szPrevEntry[48];
	static keycodes KeyCodes;
	static HWND hEdit;
	static int  iEdit;
	static bool bResetEditOnUpdate = 0;

	HWND hCtrlWnd;
	CController& ctr1 = Main.Desktop.Game2.Controller[0];
	CController& ctr2 = Main.Desktop.Game2.Controller[1];
	short vk;

	switch (message)
	{
	case WM_KEYUP:
		hCtrlWnd = 0;
		break;
		case WM_INITDIALOG:
			// Center dialog
			//GetWindowRect(g_hMainWnd, &rect );
			//GetWindowRect(hWnd , &rect2);
			//SetWindowPos (hWnd, HWND_TOP, ((rect.left + rect.right) / 2) - ((rect2.right - rect2.left) / 2), ((rect.top + rect.bottom) / 2) - ((rect2.bottom - rect2.top) / 2), 0, 0, SWP_NOSIZE);

			memcpy(KeyCodes.iKeyLeft,      Main.Settings.iKeyLeft,      sizeof(int) * 2);
			memcpy(KeyCodes.iKeyRight,     Main.Settings.iKeyRight,     sizeof(int) * 2);
			memcpy(KeyCodes.iKeyRotate,    Main.Settings.iKeyRotate,    sizeof(int) * 2);
			memcpy(KeyCodes.iKeyRevRotate, Main.Settings.iKeyRevRotate, sizeof(int) * 2);
			memcpy(KeyCodes.iKeyFast,      Main.Settings.iKeyFast,      sizeof(int) * 2);
			memcpy(KeyCodes.iKeyDrop,      Main.Settings.iKeyDrop,      sizeof(int) * 2);

			SendDlgItemMessage(hWnd, IDC_INITHORDELAY , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_HORDELAY     , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_INITROTDELAY , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_ROTDELAY     , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_FASTSPEED    , EM_LIMITTEXT, 5, 0);

			SendDlgItemMessage(hWnd, IDC_REPEATHOR, BM_SETCHECK, Main.Settings.bRepeatHorKeys[0], 0);
			SendDlgItemMessage(hWnd, IDC_REPEATROT, BM_SETCHECK, Main.Settings.bRepeatRotKeys[0], 0);

			sprintf(szBuffer, "%d", Main.Settings.iInitHorRepeatDelay[0]);
			SendDlgItemMessage(hWnd, IDC_INITHORDELAY , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.iHorRepeatDelay[0]);
			SendDlgItemMessage(hWnd, IDC_HORDELAY     , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.iInitRotRepeatDelay[0]);
			SendDlgItemMessage(hWnd, IDC_INITROTDELAY , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.iRotRepeatDelay[0]);
			SendDlgItemMessage(hWnd, IDC_ROTDELAY     , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.dwFastDelay);
			SendDlgItemMessage(hWnd, IDC_FASTSPEED    , WM_SETTEXT, 0, (LPARAM)&szBuffer);

			VirtualToText(Main.Settings.iKeyLeft[0], szBuffer);
			SendDlgItemMessage(hWnd, IDC_LEFT , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyRight[0], szBuffer);
			SendDlgItemMessage(hWnd, IDC_RIGHT , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyRotate[0], szBuffer);
			SendDlgItemMessage(hWnd, IDC_ROTATE , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyRevRotate[0], szBuffer);
			SendDlgItemMessage(hWnd, IDC_REVROTATE , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyDrop[0], szBuffer);
			SendDlgItemMessage(hWnd, IDC_DROP , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyFast[0], szBuffer);
			SendDlgItemMessage(hWnd, IDC_FAST , WM_SETTEXT, 0, (LPARAM)&szBuffer);

			SendDlgItemMessage(hWnd, IDC_INITHORDELAY2 , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_HORDELAY2     , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_INITROTDELAY2 , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_ROTDELAY2     , EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hWnd, IDC_FASTSPEED2    , EM_LIMITTEXT, 5, 0);

			SendDlgItemMessage(hWnd, IDC_REPEATHOR2, BM_SETCHECK, Main.Settings.bRepeatHorKeys[1], 0);
			SendDlgItemMessage(hWnd, IDC_REPEATROT2, BM_SETCHECK, Main.Settings.bRepeatRotKeys[1], 0);

			sprintf(szBuffer, "%d", Main.Settings.iInitHorRepeatDelay[1]);
			SendDlgItemMessage(hWnd, IDC_INITHORDELAY2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.iHorRepeatDelay[1]);
			SendDlgItemMessage(hWnd, IDC_HORDELAY2     , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.iInitRotRepeatDelay[1]);
			SendDlgItemMessage(hWnd, IDC_INITROTDELAY2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.iRotRepeatDelay[1]);
			SendDlgItemMessage(hWnd, IDC_ROTDELAY2     , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			sprintf(szBuffer, "%d", Main.Settings.dwFastDelay);
			SendDlgItemMessage(hWnd, IDC_FASTSPEED2    , WM_SETTEXT, 0, (LPARAM)&szBuffer);

			VirtualToText(Main.Settings.iKeyLeft[1], szBuffer);
			SendDlgItemMessage(hWnd, IDC_LEFT2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyRight[1], szBuffer);
			SendDlgItemMessage(hWnd, IDC_RIGHT2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyRotate[1], szBuffer);
			SendDlgItemMessage(hWnd, IDC_ROTATE2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyRevRotate[1], szBuffer);
			SendDlgItemMessage(hWnd, IDC_REVROTATE2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyDrop[1], szBuffer);
			SendDlgItemMessage(hWnd, IDC_DROP2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);
			VirtualToText(Main.Settings.iKeyFast[1], szBuffer);
			SendDlgItemMessage(hWnd, IDC_FAST2 , WM_SETTEXT, 0, (LPARAM)&szBuffer);

			return 1;

		case WM_KEYDOWN:
			vk = (int)wParam;

			GetWindowText(hEdit, szPrevEntry, 48);
			if (VirtualToText(vk, szBuffer))
			{
				switch(iEdit)
				{
				case IDC_LEFT:
					KeyCodes.iKeyLeft[0] = vk;
					break;
				case IDC_RIGHT:
					KeyCodes.iKeyRight[0] = vk;
					break;
				case IDC_ROTATE:
					KeyCodes.iKeyRotate[0] = vk;
					break;
				case IDC_REVROTATE:
					KeyCodes.iKeyRevRotate[0] = vk;
					break;
				case IDC_DROP:
					KeyCodes.iKeyDrop[0] = vk;
					break;
				case IDC_FAST:
					KeyCodes.iKeyFast[0] = vk;
					break;

				case IDC_LEFT2:
					KeyCodes.iKeyLeft[1] = vk;
					break;
				case IDC_RIGHT2:
					KeyCodes.iKeyRight[1] = vk;
					break;
				case IDC_ROTATE2:
					KeyCodes.iKeyRotate[1] = vk;
					break;
				case IDC_REVROTATE2:
					KeyCodes.iKeyRevRotate[1] = vk;
					break;
				case IDC_DROP2:
					KeyCodes.iKeyDrop[1] = vk;
					break;
				case IDC_FAST2:
					KeyCodes.iKeyFast[1] = vk;
					break;

				default:
					break;
				}
			}
			else
			{
				strcpy(szBuffer, szPrevEntry);
			}

			SetWindowText(hEdit, szBuffer);
			bResetEditOnUpdate = true;

			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_LEFT:  case IDC_RIGHT:  case IDC_ROTATE:  case IDC_REVROTATE:  case IDC_DROP:  case IDC_FAST:
			case IDC_LEFT2: case IDC_RIGHT2: case IDC_ROTATE2: case IDC_REVROTATE2: case IDC_DROP2: case IDC_FAST2:
				hEdit = (HWND)lParam;
				iEdit = (int )LOWORD(wParam);
					if (HIWORD(wParam) == EN_UPDATE)
					{
						if (bResetEditOnUpdate)
						{
							bResetEditOnUpdate = false;
							SetWindowText(hEdit, szBuffer);
							break;
						}
					}
					else if (HIWORD(wParam) == EN_SETFOCUS || HIWORD(wParam) == EN_CHANGE)
					{
						hCtrlWnd = (HWND)lParam;
						SendMessage(hCtrlWnd, EM_SETSEL, 0, 16);
					}
					return 1;
				case IDOK:
					Main.Settings.bRepeatHorKeys[0] = (SendDlgItemMessage(hWnd, IDC_REPEATHOR, BM_GETCHECK, 0, 0) == BST_CHECKED);
					Main.Settings.bRepeatRotKeys[0] = (SendDlgItemMessage(hWnd, IDC_REPEATROT, BM_GETCHECK, 0, 0) == BST_CHECKED);

					SendDlgItemMessage(hWnd, IDC_INITHORDELAY, WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iInitHorRepeatDelay[0] = atoi(szBuffer);
					SendDlgItemMessage(hWnd, IDC_HORDELAY,     WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iHorRepeatDelay[0] = atoi(szBuffer);
					SendDlgItemMessage(hWnd, IDC_INITROTDELAY, WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iInitRotRepeatDelay[0] = atoi(szBuffer);
					SendDlgItemMessage(hWnd, IDC_ROTDELAY,     WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iRotRepeatDelay[0] = atoi(szBuffer);

					SendDlgItemMessage(hWnd, IDC_INITHORDELAY2, WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iInitHorRepeatDelay[1] = atoi(szBuffer);
					SendDlgItemMessage(hWnd, IDC_HORDELAY2,     WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iHorRepeatDelay[1] = atoi(szBuffer);
					SendDlgItemMessage(hWnd, IDC_INITROTDELAY2, WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iInitRotRepeatDelay[1] = atoi(szBuffer);
					SendDlgItemMessage(hWnd, IDC_ROTDELAY2,     WM_GETTEXT, 48, (LPARAM)&szBuffer);
					Main.Settings.iRotRepeatDelay[1] = atoi(szBuffer);

					memcpy(Main.Settings.iKeyLeft,      KeyCodes.iKeyLeft,      sizeof(int) * 2);
					memcpy(Main.Settings.iKeyRight,     KeyCodes.iKeyRight,     sizeof(int) * 2);
					memcpy(Main.Settings.iKeyRotate,    KeyCodes.iKeyRotate,    sizeof(int) * 2);
					memcpy(Main.Settings.iKeyRevRotate, KeyCodes.iKeyRevRotate, sizeof(int) * 2);
					memcpy(Main.Settings.iKeyFast,      KeyCodes.iKeyFast,      sizeof(int) * 2);
					memcpy(Main.Settings.iKeyDrop,      KeyCodes.iKeyDrop,      sizeof(int) * 2);

					Main.Settings.InitKeys();

					SendMessage(hWnd, WM_DESTROY, 0, 0);
					return 1;

				case IDCANCEL:
					SendMessage(hWnd, WM_DESTROY, 0, 0);
					return 1;
			}
		break;

		case WM_DESTROY:
			EndDialog(hWnd, 0);
			bDone = true;
			return 1;
	}

	return 0;
}

bool VirtualToText(short iVirtual, char* szText)
{
	assert(iVirtual != 0);

	// Extract shift
	bool bShift = (iVirtual & 256) == 256; // or >> 8
	// Clear shift
	iVirtual &= 0xFEFF;
	switch(iVirtual)
	{
		case VK_LBUTTON:
			sprintf(szText, "Left Button");
			return true;
		case VK_RBUTTON:
			sprintf(szText, "Right Button");
			return true;
		case VK_MBUTTON:
			sprintf(szText, "Mid Button");
			return true;
		case VK_BACK:
			sprintf(szText, "Backspace");
			return true;
		case VK_TAB:
			sprintf(szText, "Tab");
			return true;
		case VK_RETURN:
			sprintf(szText, "Return");
			return true;
		case VK_SHIFT:
			sprintf(szText, "Shift");
			return true;
		case VK_CONTROL:
			sprintf(szText, "Control");
			return true;
		case VK_MENU:
			sprintf(szText, "Alt");
			return true;
		case VK_SPACE:
			sprintf(szText, "Space");
			return true;
		case VK_LEFT:
			sprintf(szText, "Left");
			return true;
		case VK_RIGHT:
			sprintf(szText, "Right");
			return true;
		case VK_UP:
			sprintf(szText, "Up");
			return true;
		case VK_DOWN:
			sprintf(szText, "Down");
			return true;
		case VK_ADD:
			sprintf(szText, "Add");
			return true;
		case VK_SUBTRACT:
			sprintf(szText, "Subtract");
			return true;
		case VK_MULTIPLY:
			sprintf(szText, "Multiply");
			return true;
		case VK_DIVIDE:
			sprintf(szText, "Divide");
			return true;
	}

	if (iVirtual >= 48 && iVirtual < 58)
	{
		// Number
		sprintf(szText, "Num %d", iVirtual - 48);
		return true;
	}
	if (iVirtual >= 96 && iVirtual <= 105)
	{
		// Number
		sprintf(szText, "Num %d", iVirtual - 96);
		return true;
	}
	if (iVirtual > 64 && iVirtual <= 90)
	{
		// Letter
		sprintf(szText, "Key %c", (char)iVirtual);
		return true;
	}
	if (iVirtual > 38 && iVirtual < 48)
	{
		// Letter
		sprintf(szText, "Key %c", (char)iVirtual);
		return true;
	}

	return false;
}