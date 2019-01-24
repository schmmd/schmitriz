#ifndef _H_STATS
#define _H_STATS

#include "Object.h"
#include "Image.h"
#include "Static.h"

class CGUIPiecePreview : public CGUIImage
{
public:
	void Init(DWORD dwNewID, char* szNewCaption, LPDIRECTDRAWSURFACEX lpdds, int index, int iCaptionLength = -1);

	virtual HRESULT Render();

private:
	int index;
};

class CGUIStats : public CGUIObject
{
public:
	void Create();

	void SetAllForeColors(COLORREF);

	CGUIStatic High[2], Score[2], Level[2], Lines[2];
	CGUIStatic HighStatic[2], ScoreStatic[2], LevelStatic[2], LinesStatic[2], NextStatic[2];
	CGUIPiecePreview PiecePreview[2];

	void ShowIndex(int i, bool bshow);
};

#define ID_STATS_STATIC 1

#endif
