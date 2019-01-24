#ifndef _H_TIMER
#define _H_TIMER

#include <windows.h>

#ifndef KEYDOWN
	#define KEYDOWN(vk_code) ((GetKeyState(vk_code) & 0x8000) ? 1 : 0)
#endif
#ifndef KEYUP
	#define KEYUP(vk_code)   ((GetKeyState(vk_code) & 0x8000) ? 0 : 1)
#endif

class timer
{
private:
	DWORD dwLastRing;
	DWORD dwDuration;
	DWORD dwDisable; // used to ignore time spent disabled

	bool bEnabled;

public:
	timer();
	DWORD Get  ();
	void  Set  (DWORD dwNewDuration);
	void  Reset();
	void  Enable ();
	void  Disable();

	bool Ring();
};

#endif