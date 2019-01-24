#ifndef _H_DIRECTX_
#define _H_DIRECTX_

#include <windows.h>
#include <stdio.h>
#include <ddraw.h>
#include "ddrawx.h"

class CDirectX : public CDirectDraw
{
public:
	virtual bool Init(HWND);
	virtual bool Kill();

	void LoadTileset();
	void ReleaseTileset();

	void LoadTheme();
	void ReleaseTheme();

	void LoadImages();
	void ReleaseImages();

	void RestoreSurfaces();
	void SetTilesetColorKeys();

	LPDIRECTDRAWSURFACEX lpddsSquare, lpddsStraight, lpddsLeftL,
						 lpddsRightL, lpddsT,        lpddsS,
						 lpddsZ;
	LPDIRECTDRAWSURFACEX lpddsBackground, lpddsHighBack, lpddsTitle, lpddsGameover, lpddsPaused;
};

#endif