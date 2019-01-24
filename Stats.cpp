#include "Stats.h"

#include "Main.h"

void CGUIStats::SetAllForeColors(COLORREF rgb)
{
	// Send to children
	int pos = 0;
	CGUIObject** pObj;

	while ((pObj = Children.GetItem(pos)) != NULL)
	{
		if ((*pObj)->GetID() == ID_STATS_STATIC)
		{
			((CGUIStatic*)(*pObj))->SetForeColor(rgb);
		}

		pos++;
	}
}

void CGUIStats::Create()
{
	COLORREF rgbStaticFore = RGB(150, 150, 150);

	int y  = 10, x;
	int cy, cx;

	HDC hDC;
	lpdds->GetDC(&hDC);

	if (hDC)
	{
		for (int i = 0; i < 2; i++)
		{
			AddObject(&HighStatic[i]);
			AddObject(&High[i]);
			AddObject(&ScoreStatic[i]);
			AddObject(&Score[i]);
			AddObject(&LevelStatic[i]);
			AddObject(&Level[i]);
			AddObject(&LinesStatic[i]);
			AddObject(&Lines[i]);
			AddObject(&NextStatic[i]);

			HighStatic[i].Init(ID_STATS_STATIC, "High:", rgbStaticFore, DT_LEFT);
			HighStatic[i].SetPos (10, y);
			HighStatic[i].SetSize((cx = Width() - 10), (cy = LevelStatic[i].GetTextHeight(hDC)));

			x = HighStatic[i].GetTextWidth(hDC) + 18;
			High[i].Init(ID_STATS_STATIC, "0", rgbStaticFore, DT_LEFT, 10);
			High[i].SetPos(x, y);
			High[i].SetSize(Width() - x, cy);

			y += (int)(cy * 1.5);

			ScoreStatic[i].Init(ID_STATS_STATIC, "Score:", rgbStaticFore, DT_LEFT);
			ScoreStatic[i].SetPos (10, y);
			ScoreStatic[i].SetSize(cx, cy);

			x = ScoreStatic[i].GetTextWidth(hDC) + 18;
			Score[i].Init(ID_STATS_STATIC, "0", rgbStaticFore, DT_LEFT, 10);
			Score[i].SetPos(x, y);
			Score[i].SetSize(Width() - x, cy);

			y += (int)(cy * 1.5);

			LevelStatic[i].Init(ID_STATS_STATIC, "Level:", rgbStaticFore, DT_LEFT);
			LevelStatic[i].SetPos (10, y);
			LevelStatic[i].SetSize(cx, cy);

			x = LevelStatic[i].GetTextWidth(hDC) + 18;
			Level[i].Init(ID_STATS_STATIC, "0", rgbStaticFore, DT_LEFT, 10);
			Level[i].SetPos(x, y);
			Level[i].SetSize(Width() - x, cy);

			y += (int)(cy * 1.5);

			LinesStatic[i].Init(ID_STATS_STATIC, "Lines:", rgbStaticFore, DT_LEFT);
			LinesStatic[i].SetPos (10, y);
			LinesStatic[i].SetSize(cx, cy);

			x = LinesStatic[i].GetTextWidth(hDC) + 18;
			Lines[i].Init(ID_STATS_STATIC, "0", rgbStaticFore, DT_LEFT, 10);
			Lines[i].SetPos(x, y);
			Lines[i].SetSize(Width() - x, cy);

			y += (int)(cy * 1.5);

			NextStatic[i].Init(ID_STATS_STATIC, "Next Piece:", rgbStaticFore, DT_LEFT);
			NextStatic[i].SetPos (10, y);
			NextStatic[i].SetSize(cx, cy);

			y += cy;

			AddObject(&PiecePreview[i]);
			PiecePreview[i].Init(0, "Preview", NULL, i);
			PiecePreview[i].SetPos(10, y);

			y = (client.bottom - client.top) / 2 + 10;
		}
	}
	else
		Debug("CGUIStats: A handle to the device context could not be estasblished.");

	ShowIndex(0, 0);
	ShowIndex(1, 0);

	lpdds->ReleaseDC(hDC);
}

void CGUIStats::ShowIndex(int i, bool show)
{
	High[i].Show(show);
	Score[i].Show(show);
	Level[i].Show(show);
	Lines[i].Show(show);
	HighStatic[i].Show(show);
	ScoreStatic[i].Show(show);
	LevelStatic[i].Show(show);
	LinesStatic[i].Show(show);
	NextStatic[i].Show(show);
	PiecePreview[i].Show(show);
}

void CGUIPiecePreview::Init(DWORD dwNewID, char* szNewCaption, LPDIRECTDRAWSURFACEX lpdds, int newindex, int iCaptionLength)
{
	CGUIImage::Init(dwNewID, szNewCaption, lpdds, iCaptionLength);

	index = newindex;
}

HRESULT CGUIPiecePreview::Render()
{
	return Main.Desktop.Game->Controller[index].NextShape.DrawPix(client.left, client.top);
}