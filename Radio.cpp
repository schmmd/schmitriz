#include "Radio.h"

CGUIRadio::CGUIRadio()
{
	bSelected = false;
}

CGUIRadio::~CGUIRadio()
{

}

void CGUIRadio::Init(DWORD dwNewID, char* szNewCaption, COLORREF rgbNewForeColor, COLORREF rgbNewOverColor, COLORREF rgbNewSelColor, UINT uNewFormat, int iNewCaptionLength)
{
	// Base class
	CGUIButton::Init(dwNewID, szNewCaption, rgbNewForeColor, rgbNewOverColor, uNewFormat, iNewCaptionLength);

	// Independent
	SetSelColor(rgbNewSelColor);

	// Dependent
}

void CGUIRadio::SetSelColor(COLORREF rgbNew)
{
	memcpy(&rgbSel, &rgbNew, sizeof(COLORREF));
}

void CGUIRadio::RenderText(HDC hDC)
{
	SetBkColor  (hDC, rgbBack);
	SetBkMode   (hDC, iBkMode);

	if (bSelected)
		SetTextColor(hDC, rgbSel);
	else if (bOver)
		SetTextColor(hDC, rgbOver);
	else
		SetTextColor(hDC, rgbFore);

	DrawText(hDC, szCaption, strlen(szCaption), &client, uFormat);

	bOver = false;
}

void CGUIRadio::DeselectGroup()
{
	CGUIObject* pPar = pParent;

	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = pPar->Children.GetItem(pos)) != NULL)
	{
		if ((*pObj)->GetID() == GetID())
		{
			((CGUIRadio*)(*pObj))->Deselect();
		}
		pos++;
	}

	return;
}

void CGUIRadio::Deselect()
{
	bSelected = false;
}

void CGUIRadio::Select()
{
	bSelected = true;
}

bool CGUIRadio::Selected()
{
	return bSelected;
}

CGUIRadio* CGUIRadio::GetSelected()
{
	// Send to children
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = pParent->Children.GetItem(pos)) != NULL)
	{
		if ((*pObj)->GetID() == GetID())
		{
			if (((CGUIRadio*)*pObj)->Selected())
				return (CGUIRadio*)*pObj;
		}

		pos++;
	}

	return NULL;
}

int CGUIRadio::wm_mousedown(POINT point)
{
	// Select radio
	DeselectGroup();
	Select();

	return 0;
}