#include "WindowsDefs.h"
#include <iostream>

#ifdef OS_WINDOWS
#include <windows.h>


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

winf_setupConsoleEncoding(){
    SetConsoleOutputCP(CP_UTF8);
}

#endif