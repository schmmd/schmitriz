#ifndef _H_DLGSIMPLE
#define _H_DLGSIMPLE

#include <windows.h>
#include <fstream.h>

class CSimpleDlg
{
public:
	CSimpleDlg() { bOK = false; }
	bool Run(HINSTANCE hInstance, HWND hwnd, int iResource);

private:
	static BOOL CALLBACK SimpleDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static bool bOK;
};

#endif