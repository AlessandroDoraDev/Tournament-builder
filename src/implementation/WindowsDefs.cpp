#include "WindowsDefs.h"

#ifdef OS_WINDOWS
#include <iostream>
#define NOMINMAX
#include <windows.h>
#include <limits>


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

void winf_setupConsoleEncoding(){
    SetConsoleOutputCP(CP_UTF8);
}

void winf_freeConsole(){
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout.flush();
    std::cerr.flush();
    if (!FreeConsole()) {
        DWORD err = GetLastError();
        MessageBoxA(nullptr, "FreeConsole failed", "Error", MB_OK);
    }
}

#endif