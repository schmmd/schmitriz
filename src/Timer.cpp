#include "timer.h"

timer::timer()
{
	dwLastRing = 0;
	dwDuration = 0;
	bEnabled = 1;
}

DWORD timer::Get()
{
	return dwDuration;
}

void timer::Set  (DWORD dwNewDuration)
{
	dwDuration = dwNewDuration;
	Reset();
}

void timer::Reset()
{
	dwLastRing = GetTickCount();
}

bool timer::Ring()
{
	if (bEnabled)
	{
		if (GetTickCount() - dwLastRing > dwDuration)
			return true;
		else
			return false;
	}

	return false;
}

void timer::Enable()
{
	if (!bEnabled)
	{
		dwLastRing += GetTickCount() - dwDisable;
		bEnabled = true;
	}
}

void timer::Disable()
{
	Enable(); // Accounts for being disabled already
	dwDisable = GetTickCount();
	bEnabled = false;
}
