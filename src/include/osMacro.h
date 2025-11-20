#pragma once

#if defined(_WIN32) || defined(_WIN64)
  #define OS_WINDOWS
#elif defined(__linux__)
  #define OS_LINUX
#else
  #error "Unsupported OS"
#endif