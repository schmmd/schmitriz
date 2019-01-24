#ifndef _H_PLAYER
#define _H_PLAYER

class CBoard;

#include "Pieces.h"
#include "Keys.h"

enum dir
{
	UP,
	DOWN
};

struct pkeys
{
	key KeyRotate, KeyRevRotate, KeyLeft, KeyRight, KeyDrop, KeyFast;
};

class CController : public piece, public pkeys
{
public:
	CController();

	void New();
	void Kill();

	void Init (int index, dir newdir);
	void SetBound(long lYBound);

	void SetBoard(CBoard const * const newboard);

	void NextPiece();
	void SetPoint(piece& p);

	bool AddGravity();
	bool OutOfBounds();
	void MoveBack();

	void ProcessKeys();
	long HandleKeys();

	piece NextShape;

	timer Gravity;

	long lInitDrop;
	long lCurScore;
	long lCurLines;

	dir Dir;
	long lBoundryY;

	int iIndex;

private:
	CBoard const * board;
};


#endif