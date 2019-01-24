#include "Debug.h"

void Debug(char *szFormat, ...)
{
	char  szString [MAX_PATH];
	char *szArguments;

	// Retrieve arguments from paramater
	szArguments = (char*)&szFormat + sizeof(szFormat);
	vsprintf(szString, szFormat, szArguments);

	// Write (formatted) string to the standard output
    OutputDebugString (szString);
}

void FatalError(char* szFormat, ...)
{
	char  szString [sizeof(szFormat) + MAX_PATH];
	char *szArguments;

	// Retrieve arguments from paramater
	szArguments = (char*)&szFormat + sizeof(szFormat);
	vsprintf(szString, szFormat, szArguments);

	// Messagebox with (formatted) string
	MessageBox(NULL, szString, "Fatal Error", MB_ICONERROR);
	exit(-1);
}
