#include "WindowsDefs.h"
#include <windows.h>
#include <iostream>

#ifdef OS_WINDOWS


void winf_setupConsole(){
    //I expect this being the case only if there is a parent process console
    //Don't need to allocate one myself
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$",  "r", stdin);

        // optional: sync C++ streams
        std::ios::sync_with_stdio();
    }
}

#endif