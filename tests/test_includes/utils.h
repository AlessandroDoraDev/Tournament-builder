#pragma once
#include <iostream>
#include <chrono>


std::string formatDurationDetailed(std::chrono::nanoseconds ns);
template<typename LambdaToTime>
std::chrono::nanoseconds timeThis(LambdaToTime l);


/////////////////////////////////////////////////////////////////////////////

template<typename LambdaToTime>
std::chrono::nanoseconds timeThis(LambdaToTime l){
    
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    #ifdef PROFILING_FLAG
    Instrumentor::Get().BeginSession("GenMovesProfiling");
    #endif
        l();
    #ifdef PROFILING_FLAG
    Instrumentor::Get().EndSession();
    #endif
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds elapsed=end-start;
    std::cout << "Moves generation took " << formatDurationDetailed(elapsed) << "\n";
    return elapsed;
}
