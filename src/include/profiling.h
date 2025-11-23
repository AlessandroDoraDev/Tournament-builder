#pragma once
#include "profiling/Instrumentor.h"


#ifdef ACTIVATE_PROFILING
    #define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif