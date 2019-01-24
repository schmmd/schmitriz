#include "Image.h"
#include "Debug.h"

#include "DDrawX.h"

void CGUIImage::Init(DWORD dwNewID, char* szNewCaption, LPDIRECTDRAWSURFACEX lpdds, int iCaptionLength)
{
	// Base class
	CGUIObject::Init(dwNewID, szNewCaption);

	// Independent

	// Dependent
	AttachSource(lpdds);
}

void CGUIImage::AttachSource(LPDIRECTDRAWSURFACEX lpddsNew)
{
	lpddssource = lpddsNew;
}

HRESULT CGUIImage::Render()
{
	if (lpdds)
	{
		HRESULT hr = DirectDraw_SimpleBlit(lpdds, lpddssource, client.left, client.top, client.right - client.left, client.bottom - client.top, 0);
		if (hr != DD_OK)
		{
			Debug("Image box '%s' failed simpleblit().", szCaption);
			return hr;
		}
	}

	return DD_OK;
}