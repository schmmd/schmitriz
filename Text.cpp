#include "Text.h"

char ScanCodeToAscii(DWORD ScanCode, BYTE* Keyboard)
{
  // Use static to avoid allocating the array at every call
  WORD word[2];
  static BYTE State[256];

  // Get the layout of the user's keyboard
  // HKL hLayout = GetKeyboardLayout(0);

  // We need to know if any modifiers  
  // are pressed, e.g. SHIFT or CTRL
  //if( GetKeyboardState(State) == FALSE )
  //  return 0;
   
  // Convert the scan code to a Windows virtual key    
  UINT vk = MapVirtualKey(ScanCode, 1);
  
  // Convert the virtual key to an ASCII character
  int ret = ToAscii(vk, ScanCode, Keyboard, word, 0);

  if (ret == 0)
	  return NULL;
  else
	 return LOBYTE(*word);
}

void CGUIText::Init(DWORD dwNewID, char* szNewCaption, COLORREF rgbNewForeColor, bool bNewLock, UINT uNewFormat, int iNewCaptionLength)
{
	// Base class
	CGUIStatic::Init(dwNewID, szNewCaption, rgbNewForeColor, uNewFormat, iNewCaptionLength);

	// Independent

	// Dependent
	bLocked = bNewLock;
}

void CGUIText::Lock()
{
	bLocked = 1;
}

void CGUIText::Unlock()
{
	bLocked = 0;
}

bool CGUIText::IsLocked() const
{
	return bLocked;
}

int CGUIText::wm_keydown(BYTE vk, BYTE* keyboard)
{
	if (!bLocked)
	{
		int len = strlen(szCaption);

		if (vk == VK_BACK || vk == VK_DELETE)
		{
			if (len > 0)
				szCaption[len - 1] = NULL;
		}
		else if (iCaptionLen >= len + 1)
		{
			// Charectar can fit
			//char cNewChar = (char)LOWORD(MapVirtualKey(vk, 2));
			char cNewChar = ScanCodeToAscii(MapVirtualKey(vk, 0), keyboard);
			//UINT newvk = MapVirtualKey(state, 3);

			if (cNewChar != 13 && cNewChar != 10)
			{
				sprintf(szCaption, "%s%c", szCaption, cNewChar);
				//szCaption[len]   = cNewChar;
				//szCaption[len+1] = NULL;
			}
		}
		return 1;
	}

	return 0;
}