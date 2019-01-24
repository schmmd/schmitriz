#include "Application.h"

#include "Debug.h"

void CApplication::Init()
{
	hInstance = NULL;
	hWnd      = NULL;
	hAccel    = NULL;
}

void CApplication::Kill()
{

}

bool CApplication::InitWindow(HINSTANCE hInst, WNDPROC wndproc, int iClientWidth, int iClientHeight, char* szMenu, char* szAccel, char* szIcon)
{
	WNDCLASSEX wcl;
	RECT       temp;
	HWND       hLocalWnd;

	// Setup window style
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	// Initialize window class structure
	wcl.cbSize = sizeof(WNDCLASSEX);

	wcl.hInstance     = hInst;
	wcl.lpszClassName = szAppName;
	wcl.lpfnWndProc   = wndproc;
	wcl.lpszMenuName  = szMenu;

	wcl.hIcon   = LoadIcon(hInst, szIcon);
	wcl.hIconSm = NULL;
	wcl.hCursor = LoadCursor(NULL, IDC_ARROW      );

	wcl.style = 0;
	wcl.cbClsExtra = 0;
	wcl.cbWndExtra = 0;

	wcl.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

	// Register window class
	if (!RegisterClassEx(&wcl))
		FatalError("Could not register class.");

	// Calculate client size
	client.left   = 0;
	client.top    = 0;
	client.right  = iClientWidth;
	client.bottom = iClientHeight;

	//CopyRect(&game, &client);

	//client.right += 100;

	// Calculate window size
	//memcpy(&window,  &client, sizeof(RECT));
	memcpy(&temp, &client, sizeof(RECT));
	AdjustWindowRect(&temp, dwStyle, !(wcl.lpszMenuName == NULL));
	temp.right  -= temp.left;
	temp.bottom -= temp.top;
	temp.left = temp.top = 0;

	if (!(hLocalWnd = CreateWindowEx(NULL,			// Extended style
								szAppName,			// Class name
								szAppName,			// Caption
								dwStyle,			// Style
								CW_USEDEFAULT,		// X position
								CW_USEDEFAULT,		// Y position
								temp.right,			// Width
								temp.bottom,		// Height
								NULL,				// Parent
								NULL,				// Menu
								hInstance,			// Instance
								NULL)))				// Extra
		FatalError("Could not create the window.");

	if ((int)szAccel)
		// Load accelerators
		hAccel = LoadAccelerators(hInst, szAccel);

	// Retrieve assigned window rect
	GetWindowRect(hLocalWnd, &window);
	GetClientRect(hLocalWnd, &client);

	// Save global variables
	hInstance = hInst;
	hWnd      = hLocalWnd;

	return true;
}

bool CApplication::HandleMessage(UINT& message)
{
	MSG msg;
	bool bIsMessage;

	message = 0;

	// If there is a message, retrieve and remove
	if (bIsMessage = (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE))
	{
		message = msg.message;
		if (msg.message == WM_QUIT)
			return bIsMessage;

		if (!TranslateAccelerator(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return bIsMessage;
}

void CApplication::ShowWindow()
{
	// Show Window
	::ShowWindow(hWnd, true);
}

void CApplication::UpdateWindow()
{
	// Show Window
	::UpdateWindow(hWnd);
}

void CApplication::SetName(char* szNewName)
{
	if (szAppName)
		delete szAppName;

	szAppName = new char[strlen(szNewName) + 1];
	if (szAppName != NULL)
	{
		strcpy(szAppName, szNewName);
	}
}

char* CApplication::GetName()
{
	return szAppName;
}

HWND CApplication::GetWnd()
{
	return hWnd;
}

HMENU CApplication::GetMenu()
{
	return ::GetMenu(hWnd);
}

HINSTANCE CApplication::GetInstance()
{
	return hInstance;
}