#ifndef _H_APPLICATION_
#define _H_APPLICATION_

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include "Debug.h"

class CApplication
{
	private:
		HINSTANCE hInstance;
		HWND      hWnd;

		HACCEL hAccel;

		char* szAppName;

	public:
		RECT client;
		RECT window;

	public:
		void Init();
		void Kill();

		bool InitWindow(HINSTANCE hInst, WNDPROC wndclass, int iClientWidth, int iClientHeight, char* szMenu, char* szAccel, char* szIcon);
		void ShowWindow();
		void UpdateWindow();

		bool HandleMessage(UINT&);

		void  SetName(char* szNewName);
		char* GetName();

		HWND  GetWnd();
		HMENU GetMenu();
		HINSTANCE GetInstance();
};

#endif