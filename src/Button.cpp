#include "Button.h"

CGUIButton::CGUIButton()
{
	bOver    = 0;
}

CGUIButton::~CGUIButton()
{
}

void CGUIButton::Init(DWORD dwNewID, char* szNewCaption, COLORREF rgbNewForeColor, COLORREF rgbNewOverColor, UINT uNewFormat, int iNewCaptionLength)
{
	// Base class
	CGUIStatic::Init(dwNewID, szNewCaption, rgbNewForeColor, uNewFormat, iNewCaptionLength);

	// Independent

	// Dependent
	SetOverColor(rgbNewOverColor);
}

void CGUIButton::SetOverColor(COLORREF rgbNew)
{
	memcpy(&rgbOver, &rgbNew, sizeof(COLORREF));
}

void CGUIButton::RenderText(HDC hDC)
{
	SetBkColor  (hDC, rgbBack);
	SetBkMode   (hDC, iBkMode);

	if (bOver)
		SetTextColor(hDC, rgbOver);
	else
		SetTextColor(hDC, rgbFore);

	DrawText(hDC, szCaption, strlen(szCaption), &client, uFormat);

	bOver = false;
}

int CGUIButton::wm_mouseover(POINT point)
{
	bOver = true;
	return 0;
}
