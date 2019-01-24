#ifndef _H_MAIN
#define _H_MAIN

#include <windows.h>
#include <assert.h>

#include "DlgAbout.h"
#include "DlgKeys.h"
#include "DlgTheme.h"
#include "DlgTileset.h"

#include "Application.h"
#include "DirectX.h"
#include "Desktop.h"
#include "Settings.h"
#include "Debug.h"

class CMain
{
public:
	void Init(HINSTANCE);
	void Loop();
	void Quit();
	void Kill();

	void HandleDXError(HRESULT, char* szDefault);
	void FatalError   (char* szErrorMessage);

	CApplication App;
	CDirectX     DirectX;
	CGUIDesktop  Desktop;

	CSettings    Settings;

	bool bExit, bInit, bActive;
};

extern CMain Main;

#endif