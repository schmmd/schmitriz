#ifndef _H_GAME_
#define _H_GAME_

#include <windows.h>

#include "Pieces.h"
#include "Player.h"
#include "Timer.h"

#include "Object.h"
#include "Image.h"
#include "Static.h"
#include "Text.h"
#include "Button.h"
#include "Radio.h"

#define GAME_WIDTH  200
#define GAME_HEIGHT 400

class CBoard
{
public:
	CBoard(short, short, short, short);
	~CBoard();

	short nX;
	short nY;
	short nTileX;
	short nTileY;
	short nTileW;
	short nTileH;

	char* GetPtr();

	void Clear();
	void ShiftUp   (int);
	void ShiftDown (int);

	HRESULT Render() const;

	bool HorCollide   (const piece& Piece) const;
	bool UpCollide    (const piece& Piece, long lBoundryY) const;
	bool DownCollide  (const piece& Piece, long lBoundryY) const;
	bool BoardCollide (const piece& Piece) const;

	void CopyTo(const piece& Piece);

	void SetBoard(char value, short x, short y);
	char GetBoard(short x, short y) const;

private:
	char* cBoard;
};

class CGUIGame : public CGUIObject
{
public:
	CGUIGame(long lNum, short tileX, short tileY, short W, short H, int LinesPerLevel);

	void Create();

	virtual HRESULT Render();

	virtual void New(long, long);
	void End();
	void Run();

	void CloseGame();

	void InitHeight(long);

	virtual void ExtraClear(CController&, long){};
	long ClearLines(CController&);

	void Pause(bool bPause = 1);

	bool IsGameover() { return bGameover; }

	void AddGravity(CController& ctr);
	void HandleCollision(CController& ctr);
	void SetGravity(CController& ctr);

	int wm_command(CGUIObject*, DWORD dwCommand, long lParam);

	CController* Controller;

	void LevelUp();

	CBoard board;

	const long NumPlayers;

protected:
	// Children
	CGUIImage Background, Gameover, Paused;

	long lCurLevel;
	long lInitHeight;

	bool bPaused, bGameover;

	int iLinesPerLevel;
};

class CGUIGame1 : public CGUIGame
{
public:
	CGUIGame1();

	virtual void New(long, long);
};

class CGUIGame2 : public CGUIGame
{
public:
	CGUIGame2();

	virtual void New(long, long);

	virtual HRESULT Render();
	virtual void ExtraClear(CController&, long); // Adjusts boundries
};

#define ID_GAME_NEW   1
#define ID_GAME_RADIO 2

#endif