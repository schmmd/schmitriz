#ifndef _H_DEBUG
#define _H_DEBUG

#include <windows.h>
#include <stdio.h>

void Debug(char *szFormat, ...);
void FatalError(char* szFormat, ...);

#endif