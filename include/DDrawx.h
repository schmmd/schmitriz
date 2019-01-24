#ifndef _DDRAWX_
#define _DDRAWX_

#include <windows.h>
#include <ddraw.h>

#define IID_IDirectDrawX     IID_IDirectDraw4
#define LPDIRECTDRAWX        LPDIRECTDRAW4
#define LPDIRECTDRAWSURFACEX LPDIRECTDRAWSURFACE4

bool DirectDraw_ClearSurface(LPDIRECTDRAWSURFACEX lpdds, DWORD dwColor);
HRESULT DirectDraw_SimpleBlit(LPDIRECTDRAWSURFACEX lpddsdest, LPDIRECTDRAWSURFACEX lpddssource, int x, int y, int width, int height, DWORD dwExFlags);

DWORD   DirectDraw_CreateRGB  (LPDIRECTDRAWSURFACEX lpdds, BYTE red, BYTE green, BYTE blue);
HRESULT DirectDraw_SetColorKey(LPDIRECTDRAWSURFACEX lpdds, BYTE red, BYTE green, BYTE blue);


class CDirectDraw
{
private:
	DDPIXELFORMAT ddpixelformat;
public:
	LPDIRECTDRAWX		 lpdd;
	LPDIRECTDRAWSURFACEX lpddsprimary;
	LPDIRECTDRAWSURFACEX lpddsback;
	LPDIRECTDRAWCLIPPER  lpddclipper;
public:
	// Window specific
	bool SetWindowCooperation	(HWND);

	// General
	bool CreateDirectDraw		();
	bool CreatePrimarySurface	();
	bool GetPixelFormat			();

	bool CheckCoopLevel();

	HRESULT FlipBuffer(HWND);
	bool ClearBackSurface(DWORD);
	HRESULT SimpleBlit		 (LPDIRECTDRAWSURFACEX, int, int, int, int, DWORD dwExFlags = 0);
	HRESULT StretchBlit	 (LPDIRECTDRAWSURFACEX, int, int, int, int, int, int, DWORD dwExFlags = 0);

	DWORD CreateRGB(BYTE red, BYTE green, BYTE blue);

	// Clipper functions
	bool CreateClipper		();
	bool SetClipperWindow	(HWND);
	bool AttachClipper		(LPDIRECTDRAWSURFACEX);

	virtual bool Init(HWND) = 0;
	virtual bool Kill(    ) = 0;

	bool LoadJPEG(LPCTSTR Name, int Location, LPDIRECTDRAWSURFACEX* Surface);
};

HRESULT
DDCopyBitmap(LPDIRECTDRAWSURFACEX pdds, HBITMAP hbm, int x, int y, int dx, int dy);

HRESULT
DDCopyBitmapSegment(LPDIRECTDRAWSURFACEX pdds, HBITMAP hbm, int sx, int sy);

LPDIRECTDRAWSURFACEX 
DDLoadBitmap(LPDIRECTDRAWX pdd, LPCSTR szBitmap, int dx, int dy, DWORD dwColorLowValue = 0, DWORD dwColorHighValue = 0, DWORD memflags = 0);

LPDIRECTDRAWSURFACEX 
DDReLoadBitmap(LPDIRECTDRAWX pdd, LPDIRECTDRAWSURFACEX pdds, LPCSTR szBitmap, int dx, int dy);

#define FROM_FILE     0
#define FROM_RESOURCE 1

#define PF_555  1
#define PF_565  2

#endif