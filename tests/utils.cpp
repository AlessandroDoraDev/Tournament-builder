#include "test_includes/utils.h"



std::string formatDurationDetailed(std::chrono::nanoseconds ns) {
    using namespace std::chrono;

    auto secs = duration_cast<seconds>(ns);
    ns -= secs;

    auto millis = duration_cast<milliseconds>(ns);
    ns -= millis;

    auto micros = duration_cast<microseconds>(ns);
    ns -= micros;

    auto nanosecs = ns;

    // Format with fixed width for nice alignment
    return std::format("{} s {} ms {} us {} ns",
        secs.count(),
        millis.count(),
        micros.count(),
        nanosecs.count());
}