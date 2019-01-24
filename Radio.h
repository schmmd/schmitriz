#ifndef _H_RADIO_
#define _H_RADIO_

#include "Button.h"

class CGUIRadio : public CGUIButton
{
public:
	CGUIRadio();
	~CGUIRadio();

	void Init(DWORD dwNewID, char* szNewCaption, COLORREF rgbNewForeColor, COLORREF rgbNewOverColor, COLORREF rgbNewSelColor, UINT uNewFormat = DT_CENTER, int iCaptionLength = -1);

	void SetSelColor(COLORREF rgbNew);

	virtual void RenderText(HDC hDC);

	void Select();
	void Deselect();
	bool Selected();

	CGUIRadio* GetSelected();

	virtual int wm_mousedown(POINT);

private:
	void DeselectGroup();

	COLORREF rgbSel;

	bool bSelected;
};

#endif