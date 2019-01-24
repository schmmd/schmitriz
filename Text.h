#ifndef _H_TEXT_
#define _H_TEXT_

#include <windows.h>
#include <stdio.h>

#include "Static.h"

class CGUIText : public CGUIStatic
{
public:
	virtual void Init(DWORD dwNewID, char* szNewCaption, COLORREF rgbNewForeColor, bool bNewLock = 0, UINT uNewFormat = DT_CENTER, int iCaptionLength = -1);

	void Lock();
	void Unlock();
	bool IsLocked() const;

	virtual int wm_keydown(BYTE, BYTE*);

private:
	bool bLocked;
};

#endif