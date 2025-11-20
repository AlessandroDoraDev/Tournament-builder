#pragma once
#include "osMacro.h"


#ifdef OS_WINDOWS

void winf_setupConsole();
#define win_setupConsole winf_setupConsole

#else

#define win_setupConsole

#endif