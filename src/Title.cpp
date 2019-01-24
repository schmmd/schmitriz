#include "Title.h"

#include "Main.h"
#include "resource.h"

void CGUITitle::Create()
{
	AddObject(&Background);
	Background.Init(0, "", Main.DirectX.lpddsTitle);
	Background.SetClientRect(client);
	Background.Show();

	dwStartTime = GetTickCount();
} 

int CGUITitle::wm_command(CGUIObject* guiObject, DWORD dwCommand, long lParam)
{
	if (dwCommand == OM_SHOW && lParam == 1)
	{
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_NEW,       MF_ENABLED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_PAUSE,     MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_END,       MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_HIGHSCORE, MF_ENABLED);
	}
	else if ((dwCommand == OM_MOUSEUP || dwCommand == OM_KEYUP) && bPress && GetTickCount() - dwStartTime > 250)
	{
		CGUIDesktop* desktop = (CGUIDesktop*)pParent;

		desktop->Stats.ShowIndex(0, 0); // Hide player 1
		desktop->Stats.ShowIndex(1, 0); // Hide player 2

		Hide();
		desktop->NewGame.Show();
		desktop->Stats.SetAllForeColors(Main.Settings.rgbHighscores);
		desktop->Stats.Show();

		bPress = false;

		return 1;
	}
	else if (dwCommand == OM_MOUSEDOWN || dwCommand == OM_KEYDOWN)
	{
		bPress = true;
	}

	return CGUIObject::wm_command(guiObject, dwCommand, lParam);
}