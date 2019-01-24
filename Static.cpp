#include "Static.h"

CGUIStatic::CGUIStatic()
{
}

CGUIStatic::~CGUIStatic()
{

}

void CGUIStatic::Init(DWORD dwNewID, char* szNewCaption, COLORREF rgbNewForeColor, UINT uNewFormat, int iNewCaptionLength)
{
	// Base class
	CGUIObject::Init(dwNewID, szNewCaption, iNewCaptionLength);

	// Independent
	SetBackColor (RGB(0, 0, 0));
	SetBackMode  (true        );

	// Dependent
	SetForeColor (rgbNewForeColor);
	SetTextFormat(uNewFormat     );
}

void CGUIStatic::RenderText(HDC hDC)
{
	SetTextColor(hDC, rgbFore);
	SetBkColor  (hDC, rgbBack);
	SetBkMode   (hDC, iBkMode);

	DrawText(hDC, szCaption, strlen(szCaption), &client, uFormat);
}

void CGUIStatic::AutoSize(HDC hDC)
{
	SIZE size;

	char* szBuffer = new char[iCaptionLen + 1];

	if (szBuffer)
	{
		memset(szBuffer,'A', sizeof(char) * iCaptionLen);
		memcpy(szBuffer, szCaption, sizeof(char) * strlen(szCaption));
		szBuffer[iCaptionLen] = NULL;

		GetTextExtentPoint32(hDC, szBuffer, strlen(szBuffer), &size);
		client.bottom = client.top  + size.cy;
		client.right  = client.left + size.cx;
	}

	delete szBuffer;
}

int CGUIStatic::GetTextWidth(HDC hDC)
{
	SIZE size;

	char* szBuffer = new char[iCaptionLen + 1];

	if (szBuffer)
	{
		memcpy(szBuffer, szCaption, sizeof(char) * strlen(szCaption));
		szBuffer[strlen(szCaption)] = NULL;

		GetTextExtentPoint32(hDC, szBuffer, strlen(szBuffer), &size);
		return size.cx;
	}

	delete szBuffer;

	return -1;
}

int CGUIStatic::GetTextHeight(HDC hDC)
{
	SIZE size;

	char szBuffer[2] = "A";

	if (szBuffer)
	{
		GetTextExtentPoint32(hDC, szBuffer, strlen(szBuffer), &size);
		return size.cy;
	}

	return -1;
}

void CGUIStatic::SetForeColor(COLORREF newrgb)
{
	memcpy(&rgbFore, &newrgb, sizeof(COLORREF));
}

void CGUIStatic::SetBackColor(COLORREF newrgb)
{
	memcpy(&rgbBack, &newrgb, sizeof(COLORREF));
}

void CGUIStatic::SetBackMode (bool bTransparent)
{
	iBkMode = (bTransparent ? TRANSPARENT : OPAQUE);
}

void CGUIStatic::SetTextFormat(UINT uNewFormat)
{
	uFormat = uNewFormat;
}