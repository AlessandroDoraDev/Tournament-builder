#pragma once
#include "osMacro.h"


#ifdef OS_WINDOWS

void winf_setupConsole();
void winf_setupConsoleEncoding();
void winf_freeConsole();
#define win_setupConsole() winf_setupConsole()
#define win_setupConsoleEncoding() winf_setupConsoleEncoding()
#define win_freeConsole() winf_freeConsole()

#else

#define win_setupConsole()
#define win_setupConsoleEncoding()
#define win_freeConsole()

#endif