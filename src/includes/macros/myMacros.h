#include "profiling/Instrumentor.h"

#define PROFILING_FLAG 1

#define SIMPLE_RANGE(i, init, end) size_t i=0; i<end; i++

#if PROFILING_FLAG
    #define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCSIG__)
#else
    #define PROFILE_SCOPE(name)
    #define PROFILE_FUNCTION()
#endif

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline __attribute__((always_inline))
#endif