#ifndef _H_TITLE
#define _H_TITLE

#include "Object.h"
#include "Image.h"

class CGUITitle : public CGUIObject
{
public:
	CGUITitle() : bPress(0) 
		{	}
	void Create();

	virtual int wm_command(CGUIObject* guiObj, DWORD dwCommand, long lParam);

	CGUIImage Background;

private:
	DWORD dwStartTime;
	bool bPress;
};

#define ID_STATS_STATIC 1

#endif