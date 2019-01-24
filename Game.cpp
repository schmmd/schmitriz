#include "Game.h"

#include "Main.h"
#include "resource.h"

// ***********************************************************
// * class CGUIGame ******************************************
// ***********************************************************

CGUIGame1::CGUIGame1() : CGUIGame(1, 10, 20, 20, 20, 20)
{
	Controller = new CController[1];

	for (int i = 0; i < NumPlayers; i++)
		Controller[i].SetBoard(&board);

	Controller[0].SetBound(board.nTileY);
	Controller[0].Init(0, DOWN);
}

void CGUIGame1::New(long arg1, long arg2)
{
	CGUIGame::New(arg1, arg2);

	Main.Settings.Init(); // Show selected stats
	((CGUIDesktop*)pParent)->Stats.ShowIndex(1, 0); // Hide 2 player stats
}

CGUIGame2::CGUIGame2() : CGUIGame(2, 10, 40, 10, 10, 5)
{
	Controller = new CController[2];

	for (int i = 0; i < NumPlayers; i++)
		Controller[i].SetBoard(&board);

	Controller[0].SetBound(board.nTileY / 2);
	Controller[0].Init(0, DOWN);
	Controller[1].SetBound(board.nTileY / 2);
	Controller[1].Init(1, UP);
}

void CGUIGame2::New(long arg1, long arg2)
{
	CGUIGame::New(arg1, arg2);

	// Show selected stats
	Main.Settings.Init();
}

HRESULT CGUIGame2::Render()
{
	HRESULT hr = CGUIGame::Render();
	if (hr != DD_OK)
		return hr;

	RECT dest;
	DDBLTFX fx;
	memset(&fx, 0, sizeof(DDBLTFX));
	fx.dwSize = sizeof(DDBLTFX);
	fx.dwFillColor = Main.DirectX.CreateRGB(25, 25, 25);

	int yTop = Controller->lBoundryY * board.nTileH;
	SetRect(&dest, board.nX, yTop - 1, board.nX + board.nTileX * board.nTileW, yTop + 1);

	// Center line
	hr = lpdds->Blt(&dest, NULL, NULL, DDBLT_COLORFILL, &fx);
	if (hr != DD_OK)
		return hr;

	if (bGameover)
	{
		hr = Gameover.Render();
		if (hr != DD_OK)
			return hr;
	}
	else if (bPaused)
	{
		hr = Paused.Render();
		if (hr != DD_OK)
			return hr;
	}

	return DD_OK;
}

void CGUIGame2::ExtraClear(CController& ctr, long y)
{
	if (ctr.Dir == DOWN)
	{
		if (y == ctr.lBoundryY - 1)
		{
			for (int i = 0; i < NumPlayers; i++)
				Controller[i].lBoundryY++;
		}
	}
	else
	{
		if (y == ctr.lBoundryY)
		{
			for (int i = 0; i < NumPlayers; i++)
				Controller[i].lBoundryY--;
		}
	}
}

int CGUIGame::wm_command(CGUIObject* guiObject, DWORD dwCommand, long lParam)
{
	static bool bMouseDown = false;
	if (dwCommand == OM_MOUSEDOWN)
	{
		bMouseDown = true;
	}
	if (dwCommand == OM_MOUSEUP && bMouseDown && bGameover)
	{
		CloseGame();
	}

	return CGUIObject::wm_command(guiObject, dwCommand, lParam);
}

CGUIGame::CGUIGame(long lNum, short tileX, short tileY, short W, short H, int LinesPerLevel) 
: NumPlayers(lNum), board(tileX, tileY, W, H), iLinesPerLevel(LinesPerLevel)
{
	InitPieces();
	board.nX = (200 - tileX * W) / 2;
	board.nY = 0;
}

void CGUIGame::New(long lLevel, long lHeight)
{
	// Clear paused flag
	Pause(false);
	CheckMenuItem(GetMenu(Main.App.GetWnd()), IDM_FILE_PAUSE, MF_UNCHECKED);

	// Adjust enabled menus
	EnableMenuItem(Main.App.GetMenu(), IDM_FILE_NEW,       MF_ENABLED);
	EnableMenuItem(Main.App.GetMenu(), IDM_FILE_PAUSE,     MF_ENABLED);
	EnableMenuItem(Main.App.GetMenu(), IDM_FILE_END,       MF_ENABLED);
	EnableMenuItem(Main.App.GetMenu(), IDM_FILE_HIGHSCORE, MF_GRAYED);

	// Clear gameover flag
	bGameover = false;

	// Initialize board
	board.Clear();

	// Init level
	lCurLevel = lLevel;

	// Place random squares
	lInitHeight = lHeight;
	InitHeight(lHeight);

	// Init pieces
	for (int i = 0; i < NumPlayers; i++)
	{
		Controller[i].New();
		Controller[i].Gravity.Enable();
		SetGravity(Controller[i]);
	}

	// Init stats
	char szBuffer[12];
	sprintf(szBuffer, "%i", lCurLevel);
	((CGUIDesktop*)(pParent))->Stats.High[0].SetCaption (((CGUIDesktop*)(pParent))->Highscore.GetHighscore());
	((CGUIDesktop*)(pParent))->Stats.Score[0].SetCaption("0");
	((CGUIDesktop*)(pParent))->Stats.Level[0].SetCaption(szBuffer);
	((CGUIDesktop*)(pParent))->Stats.Lines[0].SetCaption("0");
	((CGUIDesktop*)(pParent))->Stats.High[1].SetCaption (((CGUIDesktop*)(pParent))->Highscore.GetHighscore());
	((CGUIDesktop*)(pParent))->Stats.Score[1].SetCaption("0");
	((CGUIDesktop*)(pParent))->Stats.Level[1].SetCaption(szBuffer);
	((CGUIDesktop*)(pParent))->Stats.Lines[1].SetCaption("0");

	Show();
}

void CGUIGame::End()
{
	if (IsVisible())
	{
		// Set the gameover flag
		bGameover = true;

		// Unpause
		Pause(false);
		CheckMenuItem(GetMenu(Main.App.GetWnd()), IDM_FILE_PAUSE, MF_UNCHECKED);

		// Adjust enabled menus
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_NEW,       MF_ENABLED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_PAUSE,     MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_END,       MF_GRAYED);
		EnableMenuItem(Main.App.GetMenu(), IDM_FILE_HIGHSCORE, MF_ENABLED);

		//Hide();
		//((CGUIDesktop*)(pParent))->Stats.SetAllForeColors(Main.Settings.rgbHighscores);
		//((CGUIDesktop*)(pParent))->Highscore.Show();
		//((CGUIDesktop*)(pParent))->Highscore.NewScore(Controller->lCurScore);

		//for (int i = 0; i < NumPlayers; i++)
		//	Controller[i].Kill();
	}
}

void CGUIGame::CloseGame()
{
	CGUIDesktop* desktop = (CGUIDesktop*)pParent;

	Hide();
	if (NumPlayers == 1)
	{
		desktop->Stats.SetAllForeColors(Main.Settings.rgbHighscores);
		desktop->Highscore.Show();
		desktop->Highscore.NewScore(Controller->lCurScore);
	}
	else
	{
		desktop->Stats.Hide();
		desktop->NewGame.Show();
	}
}

void CGUIGame::Pause(bool bSetPaused)
{
	bPaused = bSetPaused;
}

void CGUIGame::Run()
{
	if (!bPaused && !bGameover)
	{
		// Gravity
		for (int i = 0; i < NumPlayers; i++)
		{
			Controller[i].Gravity.Enable();
			if (Controller[i].Gravity.Ring())
			{
				Controller[i].Gravity.Reset();

				AddGravity(Controller[i]);
			}

			long ret = Controller[i].HandleKeys();
			if (ret == -2)
				SetGravity(Controller[i]);
			else if (ret != -10)
				HandleCollision(Controller[i]);
		}
	}
}

void CGUIGame::AddGravity(CController& ctr)
{
	if (!ctr.AddGravity())
		HandleCollision(ctr);
}

void CGUIGame::HandleCollision(CController& ctr)
{
	// Alocate variables
	char szBuffer[10 + 1];

	ctr.MoveBack(); // Restore pre-collision state

	// Gameover
	if (ctr.OutOfBounds())
	{
		End();
		return;
	}

	// Object hit floor
	board.CopyTo(ctr);

	// Clear lines clears all full lines
	// and returns the number cleared
	ctr.lCurLines += ClearLines(ctr);

	// Update status
	sprintf(szBuffer, "%i", ctr.lCurLines);
	((CGUIDesktop*)(pParent))->Stats.Lines[ctr.iIndex].SetCaption(szBuffer);

	/**************** Scoring ****************/
	long lPlusScore;
	// Shape points
	lPlusScore = 5;
	// Level point
	lPlusScore += 2 * lCurLevel;
	// Drop height points
	lPlusScore += ctr.lInitDrop;
	// Height landed points
	// ctr.lCurScore += board.nTileY - ctr.GetY() - ctr.Bottom();
	// Show next piece deduction
	lPlusScore -= Main.Settings.bShowNextPiece[ctr.iIndex] * (lCurLevel / 2 + 2);

	/**************** Level ****************/
	// Level (goes by individual)
	if (ctr.lCurLines > (lCurLevel + 1) * iLinesPerLevel)
		LevelUp();

	bool bCollide = false;

	// The new shape must be tested. Else it
	// will be shown over the existing board
	if (board.BoardCollide(ctr.NextShape))
	{
		End();
		return;
	}
	/*
	if (board.BoardCollide(ctr.NextShape))
	{
		bCollide = true;
		// ctr.MoveBack();
	}
	else if (ctr.OutOfBounds())
	{
		ctr.Fall();
		if (board.BoardCollide(ctr.NextShape))
			bCollide = true;
		ctr.MoveUp();
	}
	if (bCollide)
	{
		End();
		return;
	}*/

	// Next piece
	ctr.NextPiece();
	ctr.lInitDrop = 0;

	// Increase score
	ctr.lCurScore += lPlusScore;

	// Update score
	sprintf(szBuffer, "%i", ctr.lCurScore);
	Main.Desktop.Stats.Score[ctr.iIndex].SetCaption(szBuffer);

	ctr.Gravity.Reset();
}

void CGUIGame::LevelUp()
{
	char szBuffer[32];

	lCurLevel++;
	sprintf(szBuffer, "%i", lCurLevel);

	for (int i = 0; i < NumPlayers; i++)
	{
		((CGUIDesktop*)(pParent))->Stats.Level[i].SetCaption(szBuffer);

		if (Controller[i].KeyFast.IsUp())
			SetGravity(Controller[i]);
	}
}

void CGUIGame::SetGravity(CController& ctr)
{
	long lNewTick;

	switch (lCurLevel)
	{
	case 0:
		lNewTick = 800;
		break;
	case 1:
		lNewTick = 650;
		break;
	case 2:
		lNewTick = 500;
		break;
	case 3:
		lNewTick = 350;
		break;
	case 4:
		lNewTick = 275;
		break;
	case 5:
		lNewTick = 200;
		break;
	case 6:
		lNewTick = 162;
		break;
	case 7:
		lNewTick = 125;
		break;
	case 8:
		lNewTick = 87;
		break;
	default:
		lNewTick = 90 - (lCurLevel - 8) * 10;
		break;
	}

	if (lNewTick < 0)
		lNewTick = 0;
	/*
	if (lCurLevel < 
	if (lCurLevel < 8)
		lNewTick = (8 - lCurLevel) * 100;
	else if (lCurLevel == 7)
		lNewTick = 250;
	else if (lCurLevel == 8)
		lNewTick = 225;
	else
		lNewTick = 100 - ((lCurLevel - 9) * 10);
	*/
	ctr.Gravity.Set((DWORD)lNewTick);
	// long lNewTick = (long)((16.0 - lCurLevel * 1.5) / nTileY * 1000);
	// if (lNewTick <= 0)
	// 	lNewTick = 25;
	// Gravity.Set((DWORD)lNewTick);
}

HRESULT CGUIGame::Render()
{
	HRESULT hr;

	Run();

	if ((hr = Background.Render()) != DD_OK)
		return hr;

	if ((hr = board.Render()) != DD_OK)
		return hr;

	for (int i = 0; i < NumPlayers; i++)
	{
		if ((hr = Controller[i].Draw(board.nX)) != DD_OK)
			return hr;
	}

	// Grid
	RECT dest;
	DDBLTFX fx;
	memset(&fx, 0, sizeof(DDBLTFX));
	fx.dwSize = sizeof(DDBLTFX);
	fx.dwFillColor = Main.DirectX.CreateRGB(Main.Settings.rGrid, Main.Settings.gGrid, Main.Settings.bGrid);

	if (Main.Settings.bDisplayGrid)
	{
		for (int x = 0; x <= board.nTileX * board.nTileW; x +=board. nTileW)
		{
			SetRect(&dest, board.nX + x, client.left, board.nX + x+1, client.bottom);
			if ((hr = lpdds->Blt(&dest, NULL, NULL, DDBLT_COLORFILL, &fx)) != DD_OK)
				return hr;
		}
		for (int y = 0; y < board.nTileY * board.nTileH; y += board.nTileH)
		{
			SetRect(&dest, board.nX, y, board.nX + board.nTileW * board.nTileX, y + 1);
			if ((hr = lpdds->Blt(&dest, NULL, NULL, DDBLT_COLORFILL, &fx)) != DD_OK)
				return hr;
		}
	}
	else
	{
		// Draw left line if not bordering window edge
		SetRect(&dest, board.nX, 0, board.nX + 1, board.nTileY * board.nTileH);
		if ((hr = lpdds->Blt(&dest, NULL, NULL, DDBLT_COLORFILL, &fx)) != DD_OK)
			return hr;

		SetRect(&dest, board.nX + board.nTileX * board.nTileW, 0, board.nX + board.nTileX * board.nTileW + 1, board.nTileY * board.nTileH);
		if ((hr = lpdds->Blt(&dest, NULL, NULL, DDBLT_COLORFILL, &fx)) != DD_OK)
			return hr;
	}

	if (bPaused)
		if ((hr = Paused.Render()) != DD_OK)
			return hr;

	if (bGameover)
		if ((hr = Gameover.Render()) != DD_OK)
			return hr;

	return DD_OK;
}

void CGUIGame::Create()
{
	RECT client;

	AddObject(&Gameover);

	SetRect(&client, 0, 187, 200, 222);
	Gameover.Init(0, NULL, Main.DirectX.lpddsGameover);
	Gameover.SetClientRect(client);
	Gameover.Hide();

	AddObject(&Paused);

	SetRect(&client, 0, 187, 200, 213);
	Paused.Init(0, NULL, Main.DirectX.lpddsPaused);
	Paused.SetClientRect(client);
	Paused.Hide();

	AddObject(&Background);

	Background.Init(0, NULL, Main.DirectX.lpddsBackground);
	Background.SetClientRect(pParent->GetClientRect());
	Background.Show();
}

long CGUIGame::ClearLines(CController& ctr)
{
	long lret;

	if (ctr.Dir == DOWN)
	{
		lret = 0;
		for (int y = 0; y < board.nTileY; y++)
		{
			bool bIsFull = 1;
			for (int x = 0; x < board.nTileX && bIsFull; x++)
			{
				if (board.GetBoard(x, y) == EMPTY)
					bIsFull = 0;
			}

			if (bIsFull)
			{
				lret++;

				//memmove((cBoard + lTileX), cBoard, y * lTileX - 1);
				//memset(cBoard, 0, lTileX * sizeof(char));

				board.ShiftDown(y);
				ExtraClear(ctr, y);
			}
		}
	}
	else
	{
		lret = 0;
		for (int y = board.nTileY - 1; y >= 0; y--)
		{
			bool bIsFull = 1;
			for (int x = 0; x < board.nTileX && bIsFull; x++)
			{
				if (board.GetBoard(x, y) == EMPTY)
					bIsFull = 0;
			}

			if (bIsFull)
			{
				lret++;

				//memmove((cBoard + lTileX), cBoard, y * lTileX - 1);
				//memset(cBoard, 0, lTileX * sizeof(char));

				board.ShiftUp(y);
				ExtraClear(ctr, y);
			}
		}
	}

	return lret;
}

void CGUIGame::InitHeight(long lHeight)
{
	if (lHeight != 0)
	{
		int iNumSquares = board.nTileX / 2 - 1;

		for (int i = 0; i < NumPlayers; i++)
		{
			char* pcCur;

			if (Controller[i].Dir == DOWN)
				pcCur = board.GetPtr() + (Controller[i].lBoundryY - 1) * board.nTileX;
			else
				pcCur = board.GetPtr() + (Controller[i].lBoundryY    ) * board.nTileX;

			long lLine = 0;
			while(lLine != lHeight)
			{
				int iFilledTiles = 0;
				while (iFilledTiles < iNumSquares)
				{
					char* pcRand = pcCur + rand()%board.nTileX;
					if (*pcRand == EMPTY)
					{
						*pcRand = rand()%7;
						iFilledTiles++;
					}
				}

				lLine++;

				if (Controller[i].Dir == DOWN)
					pcCur -= board.nTileX;
				else
					pcCur += board.nTileX;
			}
		}
	}
}

// ***********************************************************
// * class CBoard ********************************************
// ***********************************************************

CBoard::CBoard(short tileX, short tileY, short W, short H) : nTileX(tileX), nTileY(tileY), nTileW(W), nTileH(H)
{
	cBoard = new char[nTileX * nTileY];
}

CBoard::~CBoard()
{
	delete cBoard;
}

bool CBoard::HorCollide(const piece& Piece) const
{
	// Check if hits walls
	if (Piece.GetX() < -(Piece.Left()) || Piece.GetX() > (nTileX - Piece.Right()))
		return true;

	return false;
}

bool CBoard::DownCollide(const piece& Piece, long lBoundryY) const
{
	// Check if hits floor
	if (Piece.GetY() + Piece.Bottom() > lBoundryY)
		return true;

	return false;
}

bool CBoard::UpCollide(const piece& Piece, long lBoundryY) const
{
	// Check if hits top
	if (Piece.GetY() + Piece.Top() < lBoundryY)
		return true;

	return false;
}

bool CBoard::BoardCollide(const piece& Piece) const
{
	// Another piece
	for (int r = 0; r < Piece.Bottom(); r++)
	{
		if (r + Piece.GetY() < nTileY && r + Piece.GetY() >= 0)
		{
			for (int c = 0; c < Piece.Right() && c + Piece.GetX() < nTileX; c++)
			{
				if (GetBoard(c + Piece.GetX(), r + Piece.GetY()) != EMPTY && Piece.GetShape(c, r) == 1)
					return true;
			}
		}
	}

	return false;
}

void CBoard::CopyTo(const piece& Piece)
{
	for (int i = 0; i < 4; i++)
	{
		for (int n = 0; n < 4; n++)
		{
			if (Piece.GetShape(i, n) != 0)
				// +1 to convert to tile specs
				SetBoard(Piece.GetType(), Piece.GetX() + i, Piece.GetY() + n);
		}
	}
}

char CBoard::GetBoard(short x, short y) const
{
	return cBoard[y * nTileX + x];
}

void CBoard::SetBoard(char value, short x, short y)
{
	long loc = y * nTileX + x;
	assert(loc >= 0);
	assert(loc <= nTileX * nTileY);
	cBoard[y * nTileX + x] = value;
}

void CBoard::Clear()
{
	memset(cBoard, -1, sizeof(char) * nTileX * nTileY);
}
/*
void CBoard::ClearLine(int iLine)
{
	char *pcOld = cBoard + nTileX * (iLine + 1); // One item past last item of the line to delete
	char *pcNew = pcOld - nTileX; // One item past the last item of the line before
	do
	{
		*(--pcOld) = *(--pcNew);

	} while (pcNew != cBoard);

	pcOld = cBoard + nTileX - 1;
	do
	{
		*(pcOld--) = 0;

	} while (pcOld != cBoard);
}
*/

void CBoard::ShiftDown(int iLine)
{
	/*
	char* pcLast = cBoard + nTileX * nTileY;

	char *pcOld = cBoard + nTileX * (iLine + 1); // One item past last item of the line to delete
	char *pcNew = pcOld - nTileX; // One item past the last item of the line before
	do
	{
		*(--pcOld) = *(--pcNew);

	} while (pcNew != cBoard);

	pcOld = cBoard + nTileX - 1;
	do
	{
		*(pcOld--) = 0;

	} while (pcOld != cBoard);
	*/

	memmove(cBoard + nTileX, cBoard, iLine * nTileX);
	memset (cBoard, EMPTY, nTileX);
}

void CBoard::ShiftUp(int iLine)
{
	/*
	char *pcOld = cBoard + nTileX * (iLine + 1); // One item past last item of the line to delete
	char *pcNew = pcOld - nTileX; // One item past the last item of the line before
	do
	{
		*(--pcOld) = *(--pcNew);

	} while (pcNew != cBoard);

	char *pcOld = cBoard + nTileX - 1;
	do
	{
		*(pcOld--) = 0;

	} while (pcOld != cBoard);
	*/

	memmove(cBoard + nTileX * iLine, cBoard + nTileX * (iLine + 1), nTileX * (nTileY - iLine));
	memset (cBoard + nTileX * (nTileY - 1), 0, nTileX);
}

HRESULT CBoard::Render() const
{
	HRESULT hr;
	for (int x = 0; x < nTileX; x++)
	{
		for (int y = 0; y < nTileY; y++)
		{
			char chType;
			if ((chType = GetBoard(x, y)) != -1)
			{
				hr = Piece_DrawSquareByPixel(chType, x * nTileW + nX, y * nTileH + nY);
				if (hr != DD_OK)
				{
					Debug("Could not draw square of type %i at grid point %i, %i.", chType, x, y);
					return hr;
				}
			}
		}
	}

	return DD_OK;
}

char* CBoard::GetPtr()
{
	return cBoard;
}