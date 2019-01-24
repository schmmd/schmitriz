#include "Player.h"

#include "Main.h"
#include "Game.h"

CController::CController()
{
	// Clear Key Buffers
	ProcessKeys();
};

void CController::Init(int index, dir newdir)
{
	Dir = newdir;
	iIndex = index;
}

void CController::SetBound(long lYBound)
{
	lBoundryY = lYBound;
}

void CController::SetBoard(CBoard const * const newboard)
{
	board = newboard;
}

void CController::New()
{
	piece::New(Dir == UP ? 2 : 0);
	NextShape.New(Dir == UP ? 2 : 0);

	SetPoint(*this    );
	SetPoint(NextShape);

	lCurScore = 0;
	lCurLines = 0;
	lInitDrop = 0;
}

void CController::Kill()
{
	piece::Kill();
	NextShape.Kill();
}

bool CController::OutOfBounds()
{
	if (Dir == UP)
	{
		if (board->DownCollide(*this, board->nTileY))
			return true;
	}
	else
	{
		if (board->UpCollide(*this, 0))
			return true;
	}
	
	return false;
}

void CController::MoveBack()
{
	if (Dir == UP)
		Fall();
	else
		MoveUp();
}

void CController::NextPiece()
{
	Set(&NextShape);
	NextShape.New(Dir == UP ? 2 : 0);
	SetPoint(NextShape);
}

void CController::SetPoint(piece& p)
{
	p.x = (board->nTileX - p.Right()) / 2;
	if (Dir == DOWN)
		p.y = -p.Height() - p.Top() + 1; 
	else
		p.y = board->nTileY - p.Top() - 1; 
}

void CController::ProcessKeys()
{
	KeyRevRotate.ProcessInput();
	KeyRotate   .ProcessInput();
	KeyLeft     .ProcessInput();
	KeyRight    .ProcessInput();
	KeyDrop     .ProcessInput();
	KeyFast     .ProcessInput();
}

bool CController::AddGravity()
{
	if (Dir == DOWN)
	{
		Fall();
		if (board->DownCollide(*this, lBoundryY) || board->BoardCollide(*this))
			return false;
		else
			return true;
	}
	else
	{
		MoveUp();
		if (board->UpCollide(*this, lBoundryY) || board->BoardCollide(*this))
			return false;
		else
			return true;
	}
}

// Return codes:
// 0 >= : Drop key pressed
// -1 = : Proceeded as normal
// -2 = : Reset gravity

long CController::HandleKeys()
{
	piece NewPiece;
	NewPiece.Set(this);

	// Update keystates
	ProcessKeys();

	if (KeyDrop.IsPressed())
	{
		if (Dir == DOWN)
			lInitDrop = board->nTileY - GetY() - Bottom();
		else
			lInitDrop = GetY();

		while(AddGravity());

		return lInitDrop;
	}
	else
	{
		if (KeyRotate.IsPressed() && !KeyRevRotate.IsDown())
		{
			NewPiece.rotation += 1;
			NewPiece.rotation &= 3;
		}
		if (KeyRevRotate.IsPressed() && !KeyRotate.IsDown())
		{
			NewPiece.rotation -= 1;
			if (NewPiece.rotation < 0)
				NewPiece.rotation = 3;
		}

		if (KeyLeft.IsPressed() && !KeyRight.IsDown())
			NewPiece.MoveLeft();
		if (KeyRight.IsPressed() && !KeyLeft.IsDown())
			NewPiece.MoveRight();

		if (KeyFast.IsPressed())
		{
			if (Main.Settings.dwFastDelay < Gravity.Get())
				Gravity.Set(Main.Settings.dwFastDelay);
		}
		else if (KeyFast.IsReleased())
			return -2;

		if (!board->HorCollide(NewPiece) && !board->BoardCollide(NewPiece))
		{
			Set(&NewPiece);
		}
	}

	return -10;
}