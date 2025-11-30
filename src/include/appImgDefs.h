#pragma once
#include <filesystem>

#ifdef APPIMAGE_BUILD
extern const std::filesystem::path APPDIR;
#endif