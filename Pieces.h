#ifndef _H_PIECES
#define _H_PIECES

#include <windows.h>

HRESULT Piece_DrawSquareByPixel (long  lType, int x, int y);
HRESULT Piece_DrawSquareByGrid  (long  lType, int x, int y);

void InitPieces();

#define EMPTY -1

class piece
{
public:
	piece();

	void New(char iRot = 0);
	void Kill();

	HRESULT DrawPix(int x, int y);
	HRESULT Draw(int xoffset = 0);

	void operator =(const piece& piece);
	void Set(const piece*);

	short Top()    const;
	short Left()   const;
	short Right()  const;
	short Bottom() const;
	short Width()  const;
	short Height() const;

	int   GetX() const;
	int   GetY() const;
	char  GetType() const;
	char  GetRotation() const;

	void MoveRight();
	void MoveLeft ();
	void MoveUp();
	void Fall  ();

	inline char GetShape(short ix, short iy) const;

	short x, y;
	char type, rotation;
};

const iShapes = 7;
const iRotations = 4;

/*
const char iShapeWidth[iShapes][2] = 
{
	// Straight
	{
		{ 4 },
		{ 1 },
	},
	// Left L
	{
		{ 3 },
		{ 2 },
	},
	// Right L
	{
		{ 3 },
		{ 2 },
	},
	// T-Shape
	{
		{ 3 },
		{ 2 },
	},
	// S-Shape
	{
		{ 3 },
		{ 2 },
	},
	// Z-Shape
	{
		{ 3 },
		{ 2 },
	},
	// Square
	{
		{ 2 },
		{ 2 },
	}
};

const char iShapeHeight[iShapes][2] = 
{
	// Straight
	{
		{ 1 },
		{ 4 },
	},
	// Left L
	{
		{ 2 },
		{ 3 },	
	},
	// Right L
	{
		{ 2 },
		{ 3 },
	},
	// T-Shape
	{
		{ 2 },
		{ 3 },
	},
	// S-Shape
	{
		{ 2 },
		{ 3 },	
	},
	// Z-Shape
	{
		{ 2 },
		{ 3 },
	},
	// Square
	{
		{ 2 },
		{ 2 },
	}
};
*/

const char cShapes[iShapes][4][4][4] = 
{
	// Straight
	{
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
		},
	},
	// Left L
	{
		{
			{ 0, 0, 1, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		 {
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
		},
	},
	// Right L
	{
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
	},
	// T-Shape
	{
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
	},
	// S-Shape
	{
		{
			{ 0, 1, 1, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 1, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
	},
	// Z-Shape
	{
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
	},
	// Square
	{
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
	},
};

#endif