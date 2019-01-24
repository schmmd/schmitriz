#ifndef _H_BUTTON_
#define _H_BUTTON_

#include "Static.h"

class CGUIButton : public CGUIStatic
{
public:
	CGUIButton();
	~CGUIButton();

	virtual void Init(DWORD dwNewID, char* szNewCaption, COLORREF rgbNewForeColor, COLORREF rgbNewOverColor, UINT uFormat = DT_CENTER, int iCaptionLength = -1);

	// Window Initialization
	void SetOverColor(COLORREF rgbNew);

	// Window Rendering
	virtual void RenderText(HDC hDC);

	// Window Messages
	virtual int wm_mouseover(POINT point);

protected:
	COLORREF rgbOver;

	bool bPressed, bOver;
};

#endif