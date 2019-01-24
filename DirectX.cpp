#include "DirectX.h"

#include "Resource.h"
#include "Debug.h"

#include "Main.h"

bool CDirectX::Init (HWND hWnd)
{
	CreateDirectDraw		();

	SetWindowCooperation	(hWnd);

	CreatePrimarySurface	();

	CreateClipper			();

	SetClipperWindow		(hWnd);

	AttachClipper			(lpddsprimary);

    // create the backbuffer separately

	RECT rcBuffer;

	GetClientRect(hWnd, &rcBuffer);

	DDSURFACEDESC2 ddsd;

    memset( &ddsd, 0, sizeof(ddsd) );
    ddsd.dwSize = sizeof( ddsd );
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE; // DDSCAPS_3DDEVICE prevents mouse flicker on some computers
    ddsd.dwWidth  = rcBuffer.right;
    ddsd.dwHeight = rcBuffer.bottom;

    if(FAILED(lpdd->CreateSurface(&ddsd, &lpddsback, NULL)))
    {
        // error
        FatalError("Could not create the back buffer.");
    }

	// Must postceed CreateBackSurface
	GetPixelFormat();

	//if (NULL == (lpddsTitle = DDLoadBitmap(lpdd, "title.bmp", 0, 0, 0, 0, DDSD_CKSRCBLT)))
	//	FatalError("Could not load the title bitmap \"title.bmp\".");

	LoadImages();
	LoadTheme();
	LoadTileset();

	return 1;
}

bool CDirectX::Kill ()
{
	ReleaseTileset();
	ReleaseTheme();
	ReleaseImages();

	if (lpddclipper)
	{
		lpddclipper->Release();
		lpddclipper = NULL;
	}
	if (lpddsback)
	{
		lpddsback->Release();
		lpddsback = NULL;
	}
	if (lpddsprimary)
	{
		lpddsprimary->Release();
		lpddsprimary = NULL;
	}

	if (lpdd)
	{
		lpdd->Release();
		lpdd = NULL;
	}

	return 1;
}

void CDirectX::LoadImages()
{
	if (!LoadJPEG((char*)IDJ_TITLE, FROM_RESOURCE, &lpddsTitle))
		FatalError("Could not load the title jpeg from resource.");

	if (!LoadJPEG((char*)IDJ_GAMEOVER, FROM_RESOURCE, &lpddsGameover))
		FatalError("Could not load the gameover jpeg from resource.");

	if (!LoadJPEG((char*)IDJ_PAUSED, FROM_RESOURCE, &lpddsPaused))
		FatalError("Could not load the paused jpeg from resource.");
}

void CDirectX::ReleaseImages()
{
	if (lpddsTitle)
	{
		lpddsTitle->Release();
		lpddsTitle = NULL;
	}
	if (lpddsGameover)
	{
		lpddsGameover->Release();
		lpddsGameover = NULL;
	}
	if (lpddsPaused)
	{
		lpddsPaused->Release();
		lpddsPaused = NULL;
	}
}

void CDirectX::LoadTheme()
{
	char szErrorMessage[128], szFilename[48];
	sprintf(szErrorMessage, "Could not load theme from directory \"%s\".", Main.Settings.szThemeDir);

	sprintf(szFilename, "%s_game.jpg", Main.Settings.szThemeDir);
	if (!LoadJPEG(szFilename, FROM_FILE, &lpddsBackground))
		FatalError(szErrorMessage);
	//if (NULL == (lpddsBackground = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
	//	FatalError(szErrorMessage);

	sprintf(szFilename, "%s_high.jpg", Main.Settings.szThemeDir);
	if (!LoadJPEG(szFilename, FROM_FILE, &lpddsHighBack))
		FatalError(szErrorMessage);
	//if (NULL == (lpddsHighBack = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
	//	FatalError(szErrorMessage);
}

void CDirectX::ReleaseTheme()
{
	if (lpddsHighBack)
	{
		lpddsHighBack->Release();
		lpddsHighBack = NULL;
	}
	if (lpddsBackground)
	{
		lpddsBackground->Release();
		lpddsBackground = NULL;
	}
}

void CDirectX::LoadTileset()
{
	char szErrorMessage[128], szFilename[48];
	sprintf(szErrorMessage, "Could not create tileset surface in memory.");

	DDSURFACEDESC2          ddsd;

	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = 20;
    ddsd.dwHeight = 20;
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue  = 0;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = 0;
    if (lpdd->CreateSurface(&ddsd, &Main.DirectX.lpddsStraight, NULL) != DD_OK)
        FatalError(szErrorMessage);
    if (lpdd->CreateSurface(&ddsd, &Main.DirectX.lpddsRightL, NULL) != DD_OK)
        FatalError(szErrorMessage);
    if (lpdd->CreateSurface(&ddsd, &Main.DirectX.lpddsLeftL, NULL) != DD_OK)
        FatalError(szErrorMessage);
    if (lpdd->CreateSurface(&ddsd, &Main.DirectX.lpddsT, NULL) != DD_OK)
        FatalError(szErrorMessage);
    if (lpdd->CreateSurface(&ddsd, &Main.DirectX.lpddsS, NULL) != DD_OK)
        FatalError(szErrorMessage);
    if (lpdd->CreateSurface(&ddsd, &Main.DirectX.lpddsZ, NULL) != DD_OK)
        FatalError(szErrorMessage);
    if (lpdd->CreateSurface(&ddsd, &Main.DirectX.lpddsSquare, NULL) != DD_OK)
        FatalError(szErrorMessage);

    HBITMAP hbm;
	strcpy(szFilename, Main.Settings.szTilesetDir);
	sprintf(szErrorMessage, "Could not load image '%s'.", szFilename);
    hbm = (HBITMAP) LoadImage(NULL, szFilename, IMAGE_BITMAP, 140, 20, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        FatalError(szErrorMessage);

    DDCopyBitmap(Main.DirectX.lpddsStraight,	hbm,   0, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsLeftL,		hbm,  20, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsRightL,		hbm,  40, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsT,			hbm,  60, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsS,			hbm,  80, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsZ,			hbm, 100, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsSquare,		hbm, 120, 0, 20, 20);

    DeleteObject(hbm);
	
/*
	sprintf(szFilename, "%st1.bmp", Main.Settings.szTilesetDir);
	if (NULL == (lpddsStraight = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
		FatalError(szErrorMessage);

	sprintf(szFilename, "%st2.bmp", Main.Settings.szTilesetDir);
	if (NULL == (lpddsLeftL = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
		FatalError(szErrorMessage);

	sprintf(szFilename, "%st3.bmp", Main.Settings.szTilesetDir);
	if (NULL == (lpddsRightL = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
		FatalError(szErrorMessage);

	sprintf(szFilename, "%st4.bmp", Main.Settings.szTilesetDir);
	if (NULL == (lpddsT = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
		FatalError(szErrorMessage);

	sprintf(szFilename, "%st5.bmp", Main.Settings.szTilesetDir);
	if (NULL == (lpddsS = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
		FatalError(szErrorMessage);

	sprintf(szFilename, "%st6.bmp", Main.Settings.szTilesetDir);
	if (NULL == (lpddsZ = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
		FatalError(szErrorMessage);

	sprintf(szFilename, "%st7.bmp", Main.Settings.szTilesetDir);
	if (NULL == (lpddsSquare = DDLoadBitmap(lpdd, szFilename, 0, 0, 0, 0, DDSD_CKSRCBLT)))
		FatalError(szErrorMessage);
*/
	SetTilesetColorKeys();
}

void CDirectX::RestoreSurfaces()
{
	Main.DirectX.lpdd->RestoreAllSurfaces();

	char szErrorMessage[128], szFilename[48];

    HBITMAP hbm;
	strcpy(szFilename, Main.Settings.szTilesetDir);
	sprintf(szErrorMessage, "Could not reload image '%s'.", szFilename);
    hbm = (HBITMAP) LoadImage(NULL, szFilename, IMAGE_BITMAP, 140, 20, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        FatalError(szErrorMessage);

    DDCopyBitmap(Main.DirectX.lpddsStraight,	hbm,   0, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsLeftL,		hbm,  20, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsRightL,		hbm,  40, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsT,			hbm,  60, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsS,			hbm,  80, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsZ,			hbm, 100, 0, 20, 20);
	DDCopyBitmap(Main.DirectX.lpddsSquare,		hbm, 120, 0, 20, 20);

    DeleteObject(hbm);


	LoadTheme();
	LoadImages();

	SetTilesetColorKeys();
}

void CDirectX::SetTilesetColorKeys()
{
	DirectDraw_SetColorKey(lpddsStraight, 255, 0, 255);
	DirectDraw_SetColorKey(lpddsLeftL,	  255, 0, 255);
	DirectDraw_SetColorKey(lpddsRightL,	  255, 0, 255);
	DirectDraw_SetColorKey(lpddsT,		  255, 0, 255);
	DirectDraw_SetColorKey(lpddsS,		  255, 0, 255);
	DirectDraw_SetColorKey(lpddsZ,		  255, 0, 255);
	DirectDraw_SetColorKey(lpddsSquare,   255, 0, 255);
}

void CDirectX::ReleaseTileset()
{
	if (lpddsStraight)
	{
		lpddsStraight->Release();
		lpddsStraight = NULL;
	}
	if (lpddsLeftL)
	{
		lpddsLeftL->Release();
		lpddsLeftL = NULL;
	}
	if (lpddsRightL)
	{
		lpddsRightL->Release();
		lpddsRightL = NULL;
	}
	if (lpddsT)
	{
		lpddsT->Release();
		lpddsT = NULL;
	}
	if (lpddsS)
	{
		lpddsS->Release();
		lpddsS = NULL;
	}
	if (lpddsZ)
	{
		lpddsZ->Release();
		lpddsZ = NULL;
	}
	if (lpddsSquare)
	{
		lpddsSquare->Release();
		lpddsSquare = NULL;
	}
}
