#pragma once

#if defined(_WIN32) || defined(_WIN64)
    #define OS_WINDOWS
#elif defined(__linux__)
    #define OS_LINUX
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_OSX
      	#define OS_MACOSX
    #endif
#else
  	#error "Unsupported OS"
#endif