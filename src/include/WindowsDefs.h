#pragma once
#include "osMacro.h"


#ifdef OS_WINDOWS

void winf_setupConsole();
void winf_setupConsoleEncoding();
#define win_setupConsole() winf_setupConsole()
#define setupConsoleEncoding() winf_setupConsoleEncoding()

#else

#define win_setupConsole()
#define setupConsoleEncoding()

#endif