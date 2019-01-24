#ifndef _H_STATIC_
#define _H_STATIC_

#include "Object.h"

class CGUIStatic : public CGUIObject
{
public:
	CGUIStatic();
	~CGUIStatic();

	virtual void Init(DWORD dwNewID, char* szSetCaption, COLORREF rgbNewFore, UINT uNewFormat = DT_CENTER, int iCaptionLength = -1);

	// Window Positioning
	void AutoSize(HDC hDC);

	int GetTextWidth (HDC hDC);
	int GetTextHeight(HDC hDC);

	// Window Initialization
	void SetForeColor (COLORREF newrgb  );
	void SetBackColor (COLORREF newrgb  );
	void SetBackMode  (bool bTransparent);
	void SetTextFormat(UINT uNewFormat  );

	// Window Rendering
	virtual void RenderText(HDC hDC);

protected:
	int iBkMode;
	UINT uFormat;
	COLORREF rgbFore, rgbBack;
};

#endif