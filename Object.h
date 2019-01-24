#ifndef _H_OBJECT_
#define _H_OBJECT_

#include <windows.h>
#include <ddraw.h>
#include "list.h"

#include "DDrawX.h"

class CGUIObject
{
public:
	CGUIObject();
	~CGUIObject();

	// Window Initialization
	virtual void Init(DWORD dwNewID, char* szSetCaption, int iCaptionLength = -1);
	void AddObject(CGUIObject* pObj);

	void  SetID     (DWORD dwID);
	DWORD GetID     (          ) const;
	void  SetSurface(LPDIRECTDRAWSURFACEX lpdds);
	void  SetParent (CGUIObject *guiObj);
	void  SetWindow (HWND hWnd         );
	void  InitCaption(const char * const szNewCaption, int len = -1);
	void  SetCaption (const char * const szNewCaption);
	char* GetCaption () const;

	// Window Positioning
	void  SetClientRect(const RECT& rect  );
	void  SetPos (int x, int y);
	void  SetSize(int w, int h);

	const RECT& GetClientRect() const;

	long Width()  const;
	long Height() const;

	long Left()   const;
	long Top()    const;
	long Right()  const;
	long Bottom() const;

	// Window Management
	void Show(bool bShow = 1);
	void Hide();
	bool IsVisible() const;

	// Window Rendering
	virtual void    RenderText(HDC hDC);
	virtual HRESULT Render    (       );

	// Window Messages
	virtual int wm_command  (CGUIObject *guiObj, DWORD dwCommand, long lParam) { return 0; }
	virtual int wm_paint    (	  ) { return 0; }
	virtual int wm_keyup    (BYTE, BYTE*);
	virtual int wm_keydown  (BYTE, BYTE*);
	virtual int wm_mouseover(POINT);
	virtual int wm_mousedown(POINT);
	virtual int wm_mouseup  (POINT);
	virtual int wm_show     (bool);

	CGUIObject* pParent;
	CLinkedList<CGUIObject*> Children;
protected:
	DWORD dwID;
	RECT client;

	char* szCaption;
	int   iCaptionLen;

	LPDIRECTDRAWSURFACEX lpdds;

	HWND hWnd;

	bool bVisible;
};

#define OM_MOUSEOVER 1
#define OM_MOUSEDOWN 2
#define OM_MOUSEUP   3
#define OM_KEYUP     4
#define OM_KEYDOWN   5
#define OM_SHOW      6

#endif