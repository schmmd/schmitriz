#include "Desktop.h"

#include "Main.h"

CGUIDesktop::CGUIDesktop()
{
	keyboard    = new BYTE[256];
	oldkeyboard = new BYTE[256];

	GetKeyboardState(keyboard);
	GetKeyboardState(oldkeyboard);
}

CGUIDesktop::~CGUIDesktop()
{
	delete keyboard;
	delete oldkeyboard;
}

void CGUIDesktop::Create()
{
	AddObject(&Title);
	AddObject(&Stats);
	AddObject(&Highscore);
	AddObject(&Game1);
	AddObject(&Game2);
	AddObject(&NewGame);

	Title.Init(0, "Title");
	Title.SetClientRect(client);
	Title.Create();

	Game = &Game2;

	Game1.Init(0, "Game 1");
	Game1.SetPos    (0, 0);
	Game1.SetSize   (GAME_WIDTH, GAME_HEIGHT);
	Game1.Create();

	Game2.Init(0, "Game 2");
	Game2.SetPos    (0, 0);
	Game2.SetSize   (GAME_WIDTH, GAME_HEIGHT);
	Game2.Create();

	NewGame.Init(0, "NewGame");
	NewGame.SetPos    (0, 0);
	NewGame.SetSize   (GAME_WIDTH, GAME_HEIGHT);
	NewGame.Create();

	Highscore.Init(0, "Highscore");
	Highscore.SetPos(0, 0);
	Highscore.SetSize(GAME_WIDTH, GAME_HEIGHT);
	Highscore.Create();

	Stats.Init(0, "Stats");
	Stats.SetPos (Game1.Right(), 0);
	Stats.SetSize(client.right - (client.right - Game1.Right()), client.bottom);
	Stats.Create();

	Stats.High[0].SetCaption(Highscore.GetHighscore());
	Stats.High[1].SetCaption(Highscore.GetHighscore());

	Title.Show();
}

void CGUIDesktop::CreateMessages()
{
	// Swap keyboards with new
	BYTE* temp;
	temp = keyboard;
	keyboard = oldkeyboard;
	oldkeyboard = temp;

	// Grab state
	GetKeyboardState(keyboard);

	for (BYTE i = 8; ; i++)
	{
		if (UP(keyboard[i]) && DOWN(oldkeyboard[i]))
		{
			wm_keyup(i, oldkeyboard);
		}
		else if (DOWN(keyboard[i]) && UP(oldkeyboard[i]))
		{
			wm_keydown(i, keyboard);
		}

		if (i == 255)
			break;
	}

	/* Handle Mouse */
	POINT old;

	memcpy(&old, &mouse, sizeof(POINT));
	GetCursorPos(&mouse);
	ScreenToClient(hWnd, &mouse);

	if (mouse.x > client.left && mouse.y > client.top && mouse.x < client.right && mouse.y < client.bottom)
	{
		// is over
		wm_mouseover(mouse);

		bool mousestate = DOWN(keyboard   [VK_LBUTTON]);
		bool oldstate   = DOWN(oldkeyboard[VK_LBUTTON]);
		if (mousestate == 1 && oldstate == 0)
			// was pressed down
			wm_mousedown(mouse);
		else if (mousestate == 0 && oldstate == 1)
			// was released up
			wm_mouseup(mouse);
	}
}

HRESULT CGUIDesktop::Render()
{
	// Render graphics
	HRESULT hr = CGUIObject::Render();
	if (hr != DD_OK)
		return hr;

	// Render text
	HDC hDC;
	Main.DirectX.lpddsback->GetDC(&hDC);		// Get DC
	RenderText(hDC);							// Render the text
	Main.DirectX.lpddsback->ReleaseDC(hDC);		// Release DC

	return DD_OK;
}