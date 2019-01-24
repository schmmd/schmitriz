#include "pieces.h"

#include "Main.h"

char acShapeTop   [iShapes][iRotations];
char acShapeLeft  [iShapes][iRotations];
char acShapeWidth [iShapes][iRotations];
char acShapeHeight[iShapes][iRotations];

HRESULT Piece_DrawSquareByPixel(long lShape, int x, int y)
{
	if (lShape != -1)
	{
		if (Main.Settings.bImageShapeTiles)
		{
			LPDIRECTDRAWSURFACEX lpdds;
			switch(lShape)
			{
				case 0:
					lpdds = Main.DirectX.lpddsStraight;
					break;
				case 1:
					lpdds = Main.DirectX.lpddsLeftL;
					break;
				case 2:
					lpdds = Main.DirectX.lpddsRightL;
					break;
				case 3:
					lpdds = Main.DirectX.lpddsT;
					break;
				case 4:
					lpdds = Main.DirectX.lpddsS;
					break;
				case 5:
					lpdds = Main.DirectX.lpddsZ;
					break;
				case 6:
					lpdds = Main.DirectX.lpddsSquare;
					break;
			}
			CBoard& board = Main.Desktop.Game->board;
			if (lpdds && y >= 0 && x >= 0) // && y + board.nTileH <= board.nTileH * board.nTileY && x + board.nTileW <= board.nX + board.nTileW * board.nTileX)
			{
				CBoard& board = Main.Desktop.Game->board;
				return Main.DirectX.StretchBlit(lpdds, x, y, 20, 20, board.nTileW, board.nTileH, DDBLT_KEYSRC);
			}
		}
	}

	return DD_OK;
}

HRESULT Piece_DrawSquareByGrid(long lShape, int x, int y)
{
	CBoard& board = Main.Desktop.Game->board;
	return Piece_DrawSquareByPixel(lShape, x * board.nTileW, y * board.nTileH);
}

piece::piece()
{
	type = -1;
}

HRESULT piece::DrawPix(int left, int top)
{
	HRESULT hr;
	CBoard& board = Main.Desktop.Game->board;
	if (type != -1)
	{
		for (int iPix = 0, i = 0; i < 4; iPix += board.nTileW, i++)
		{
			for (int nPix = 0, n = 0; n < 4; nPix += board.nTileH, n++)
			{
				if (y + nPix < board.nTileH * board.nTileY)
				{
					if (GetShape(i, n) != 0)
					{
						hr = Piece_DrawSquareByPixel(type, left + iPix, top + nPix);
						if (hr != DD_OK)
							return hr;
					}
				}
			}
		}
	}

	return DD_OK;
}

HRESULT piece::Draw(int xoffset)
{
	CBoard& board = Main.Desktop.Game->board;
	HRESULT hr = DrawPix(x * board.nTileW + xoffset, y * board.nTileH);
	if (hr != DD_OK)
		Debug("Could not draw piece at %i, %i.", x, y);
	return DD_OK;
}

void piece::New(char iRot)
{
	type = rand() % iShapes;
	rotation = iRot;
}

void piece::Kill()
{
	type = -1;
}

short piece::Right() const
{
	return Width() + Left();
}

short piece::Bottom() const
{
	return Height() + Top();
}

short piece::Width() const
{
	return acShapeWidth[type][rotation];
}

short piece::Height() const
{
	return acShapeHeight[type][rotation];
}

short piece::Left() const
{
	return acShapeLeft[type][rotation];
}

short piece::Top() const
{
	return acShapeTop[type][rotation];
}

int piece::GetX() const
{
	return x;
}

int piece::GetY() const
{
	return y;
}

char piece::GetType() const
{
	return type;
}

char piece::GetRotation() const
{
	return rotation;
}

void piece::MoveUp()
{
	y--;
}

void piece::Fall()
{
	y++;
}

void piece::MoveRight()
{
	x++;
}

void piece::MoveLeft ()
{
	x--;
}

void piece::operator =(const piece& piece)
{
	type = piece.type;
	rotation = piece.rotation;
	x = piece.x;
	y = piece.y;
}

void piece::Set(const piece* piece)
{
	type = piece->type;
	rotation = piece->rotation;
	x = piece->x;
	y = piece->y;
}

inline char piece::GetShape(short x, short y) const
{
	return cShapes[type][rotation][y][x];
}

inline char GetShape(short shape, short rotation, short x, short y)
{
	return cShapes[shape][rotation][y][x];
}


void InitPieces()
{
	int shape, rot, y, x;
	int top, bottom, left, right;
	bool bFilled;
	for (shape = 0; shape < iShapes; shape++)
	{
		for (rot = 0; rot < iRotations; rot++)
		{
			top = -1;
			bottom = -1;
			for (y = 0; y < 4; y++)
			{
				bFilled = 0;
				for (x = 0; x < 4; x++)
				{
					if (GetShape(shape, rot, x, y))
					{
						bFilled = 1;
						break;
					}
				} // for x

				if (bFilled)
				{
					if (top == -1)
						top = y;
				}
				else
				{
					if (top != -1)
					{
						bottom = y;
						break;
					}
				}
			} // for y
			if (bottom == -1)
				bottom = y;

			left  = -1;
			right = -1;
			for (x = 0; x < 4; x++)
			{
				bFilled = 0;
				for (y = 0; y < 4; y++)
				{
					if (GetShape(shape, rot, x, y))
					{
						bFilled = 1;
						break;
					}
				}

				if (bFilled)
				{
					if (left == -1)
						left = x;
				}
				else
				{
					if (left != -1)
					{
						right = x;
						break;
					}
				}
			}
			if (right == -1)
				right = x;

			acShapeWidth [shape][rot] = right  - left;
			acShapeHeight[shape][rot] = bottom - top;
			acShapeLeft  [shape][rot] = left;
			acShapeTop   [shape][rot] = top;
		} // for rot
	} // for shape
}