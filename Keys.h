#ifndef _H_KEY
#define _H_KEY

#include "timer.h"

class key
{
private:
	enum keystate
	{
		released,
		up,
		pressed,
		down
	};
private:
	keystate KeyState;
	int   iKey;
	int   iDoubleKey;

	bool  bRepeat;
	char  bHasBegan;
	DWORD dwBegin;
	DWORD dwInitDelay;
	DWORD dwDelay;

public:
	key() { iKey = 0; iDoubleKey = 0; KeyState = up; bRepeat = false; }

	void SetKey(const int iNewKey)
	{
		iKey = iNewKey;
		
		if (iKey >= 48 && iKey < 58)
			iDoubleKey = iKey + 48;
		else
			iDoubleKey = 0;
	}
	int GetKey() const
	{
		return iKey;
	}
	void ProcessInput()
	{
		// Process key
		if (KEYDOWN(iKey) || (iDoubleKey != 0 && KEYDOWN(iDoubleKey)))
		{
			if (KeyState == up || KeyState == released)
				KeyState = pressed;
			else
				KeyState = down;
		}
		else
		{
			if (KeyState == down || KeyState == pressed)
			{
				KeyState = released;
				bHasBegan = false;
			}
			else
			{
				KeyState = up;
			}
		}

		// Handle repeat
		if (bRepeat)
		{
			DWORD dwTickCount = GetTickCount();

			if (KeyState == pressed)
				dwBegin = GetTickCount();

			else if (KeyState == down)
			{
				if (bHasBegan == 0)
				{
					if (dwTickCount - dwBegin > dwInitDelay)
					{
						KeyState = pressed;
						bHasBegan = 1;

						dwBegin = dwTickCount;
					}
				}
				else
				{
					if (dwTickCount - dwBegin > dwDelay)
					{
						KeyState = pressed;

						dwBegin = dwTickCount;
					}	
				}
			}
		}
					
	}
	void SetRepeat(const DWORD dwNewInitDelay, const DWORD dwNewDelay)
	{
		bRepeat   = true;
		bHasBegan = 0;
		dwBegin   = GetTickCount();
		dwInitDelay = dwNewInitDelay;
		dwDelay     = dwNewDelay;
	}
	void KillRepeat()
	{
		bRepeat = false;
	}

	bool IsReleased() const  { return (bool)(KeyState == released); }
	bool IsUp()		  const	 { return (bool)(KeyState == up		 );	}
	bool IsPressed()  const  { return (bool)(KeyState == pressed );	}
	bool IsDown()	  const	 { return (bool)(KeyState == down	 );	}
};

#endif