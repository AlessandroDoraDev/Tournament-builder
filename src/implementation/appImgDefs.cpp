#include "appImgDefs.h"
#ifdef APPIMAGE_BUILD
#include <cstdlib>

const std::filesystem::path APPDIR=std::filesystem::path(std::getenv("APPDIR"))/"usr/bin";;
#endif