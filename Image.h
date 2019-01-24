#ifndef _H_IMAGE_
#define _H_IMAGE_

#include "Object.h"
#include "DDrawX.h"

class CGUIImage : public CGUIObject
{
public:
	void Init(DWORD dwNewID, char* szNewCaption, LPDIRECTDRAWSURFACEX lpdds, int iCaptionLength = -1);

	void AttachSource(LPDIRECTDRAWSURFACEX lpddsNew);

	virtual HRESULT Render();

private:
	LPDIRECTDRAWSURFACEX lpddssource;
};

#endif