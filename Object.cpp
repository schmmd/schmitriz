#include "Object.h"

CGUIObject::CGUIObject()
{
	pParent = NULL;

	szCaption = NULL;
	bVisible  = false;

	dwID = 0;

	memset(&client, 0, sizeof(RECT));
}

CGUIObject::~CGUIObject()
{
	int pos = 0;
	bVisible = false;

	/* If children are dynamically allocated
	CGUIObject** obj;
	while ((obj = Children.GetItem(pos)) != NULL)
	{
		delete (*obj);
		*obj = NULL;

		pos++;
	}
	*/

	if (szCaption)
		delete szCaption;
}

void CGUIObject::RenderText(HDC hDC)
{
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if ((*pObj)->IsVisible())
			(*pObj)->RenderText(hDC);

		pos++;
	}
}

HRESULT CGUIObject::Render()
{
	int pos = 0;
	HRESULT hr;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if ((*pObj)->IsVisible())
		{
			if ((hr = (*pObj)->Render()) != DD_OK)
				return hr;
		}

		pos++;
	}

	return DD_OK;
}

void CGUIObject::Init(DWORD dwNewID, char* szNewCaption, int iCaptionLength)
{
	// Base class

	// Independent
	SetWindow (pParent->hWnd);
	SetSurface(pParent->lpdds);

	// Dependent
	SetID(dwNewID);
	InitCaption(szNewCaption, iCaptionLength);
}

void CGUIObject::AddObject(CGUIObject* pObj)
{
	pObj->SetParent(this);
	Children.AddItem(pObj);
}

void CGUIObject::SetID(DWORD dwNewID)
{
	dwID = dwNewID;
}

DWORD CGUIObject::GetID() const
{
	return dwID;
}

void CGUIObject::SetParent(CGUIObject *guiObj)
{
	pParent = guiObj;
}

void CGUIObject::SetWindow(HWND hNewWnd)
{
	hWnd = hNewWnd;
}

void CGUIObject::SetSurface(LPDIRECTDRAWSURFACEX lpddsNew)
{
	lpdds = lpddsNew;
}

void CGUIObject::InitCaption(const char * const szNewCaption, int len)
{
	if (szNewCaption)
	{
		int temp;
		if (szCaption)
			delete szCaption;

		temp = strlen(szNewCaption);
		if (len < temp)
			len = temp;

		szCaption = new char[len + 1];
		strcpy(szCaption, szNewCaption);
		iCaptionLen = (int)len;
	}
}

void CGUIObject::SetCaption(const char * const szNewCaption)
{
	if (szNewCaption)
	{
		int len = strlen(szNewCaption);

		if (len > iCaptionLen)
		{
			iCaptionLen = len + 1;

			if (szCaption)
				delete szCaption;

			szCaption = new char[iCaptionLen];
		}

		strcpy(szCaption, szNewCaption);
	}
}

char* CGUIObject::GetCaption() const
{
	return szCaption;
}

void CGUIObject::Show(bool bShow)
{
	wm_show(bShow);
}

void CGUIObject::Hide()
{
	wm_show(false);
}

bool CGUIObject::IsVisible() const
{
	return bVisible;
}

void CGUIObject::SetClientRect(const RECT& rect)
{
	memcpy(&client, &rect, sizeof(RECT));
}

const RECT& CGUIObject::GetClientRect() const
{
	return client;
}

void CGUIObject::SetPos (int x, int y)
{
	client.left = x + pParent->Left();
	client.top  = y + pParent->Top();
}

void CGUIObject::SetSize(int w, int h)
{
	client.right  = client.left   + w;
	client.bottom = client.top    + h;
}

long CGUIObject::Width() const
{
	return client.right - client.left;
}

long CGUIObject::Height() const
{
	return client.bottom - client.top;
}

long CGUIObject::Left() const
{
	return client.left;
}

long CGUIObject::Top() const
{
	return client.top;
}

long CGUIObject::Right() const
{
	return client.right;
}

long CGUIObject::Bottom() const
{
	return client.bottom;
}

int CGUIObject::wm_mouseover(POINT point)
{
	// Send to children
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if (((*pObj)->Left() <= point.x) && ((*pObj)->Top() <= point.y) && ((*pObj)->Right() >= point.x) && ((*pObj)->Bottom() >= point.y))
		{
			if ((*pObj)->IsVisible())
			{
				// Send to object (for standard processing)
				(*pObj)->wm_mouseover(point);
				// Send to this (for custom processing)
				wm_command(*pObj, OM_MOUSEOVER, (long)&point);
			}
		}
		pos++;
	}

	return 0;
}

int CGUIObject::wm_mousedown(POINT point)
{
	// Send to children
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if (((*pObj)->Left() <= point.x) && ((*pObj)->Top() <= point.y) && ((*pObj)->Right() >= point.x) && ((*pObj)->Bottom() >= point.y))
		{
			if ((*pObj)->IsVisible())
			{
				// Send to object (for standard processing)
				(*pObj)->wm_mousedown(point);
				// Send to this (for custom processing)
				wm_command(*pObj, OM_MOUSEDOWN, (long)&point);
			}
		}
		pos++;
	}

	return 0;
}

int CGUIObject::wm_mouseup(POINT point)
{
	// Send to children
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if (((*pObj)->Left() <= point.x) && ((*pObj)->Top() <= point.y) && ((*pObj)->Right() >= point.x) && ((*pObj)->Bottom() >= point.y))
		{
			if ((*pObj)->IsVisible())
			{
				// Send to object (for standard processing)
				(*pObj)->wm_mouseup(point);
				// Send to this (for custom processing)
				if (!wm_command(*pObj, OM_MOUSEUP, (long)&point))
					return 0;
			}
		}
		pos++;
	}

	return 0;
}

int CGUIObject::wm_keyup(BYTE byte, BYTE* keyboard)
{
	// Send to children
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if ((*pObj)->IsVisible())
		{
			// Send to object (for standard processing)
			(*pObj)->wm_keyup(byte, keyboard);
			// Send to this (for custom processing)
			wm_command(*pObj, OM_KEYUP, (long)byte);
		}

		pos++;
	}

	return 0;
}

int CGUIObject::wm_keydown(BYTE byte, BYTE* keyboard)
{
	// Send to children
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if ((*pObj)->IsVisible())
		{
			// Send to object (for standard processing)
			(*pObj)->wm_keydown(byte, keyboard);
			// Send to this (for custom processing)
			wm_command(*pObj, OM_KEYDOWN, (long)byte);
		}

		pos++;
	}

	return 0;
}

int CGUIObject::wm_show(bool bShow)
{
	if (pParent)
	{
		bVisible = bShow;
		wm_command(this, OM_SHOW, bShow);

		return bShow;
	}
	else
		return -1;
}
