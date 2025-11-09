#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <algorithm>
#include <array>
#include <thread>
#include <utility>
#include <ranges>
#include <chrono>
#include <string>
#include "absl/hash/hash.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/btree_set.h"
#include "profiling/Instrumentor.h"
#include "globals.h"
#include "to_strings.h"
#include "matricesModule.h"
#include "Config.h"
#include "TournamentConfig.h"
#include "strat.h"
#include "TournamentBuilderGui.h"

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

template<typename LambdaToTime>
void timeThis(LambdaToTime l){
    
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    #ifdef PROFILING_FLAG
    Instrumentor::Get().BeginSession("GenMovesProfiling");
    #endif
        l();
    #ifdef PROFILING_FLAG
    Instrumentor::Get().EndSession();
    #endif
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::cout << "Moves generation took " << formatDurationDetailed(end-start) << "\n";
}

void main1(std::string csv_path) {

    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows rows = readCSV(csv_path);
    PlayerList player_list = formatRowsToPlayerList(rows);
    int n_teams = static_cast<int>(player_list.size() / N_PLAYERS);
    Config config=Config(player_list, n_teams, N_PLAYERS);
    timeThis([&](){
        deduceAndApplyMoves(config);
    });
    std::cout<<(std::string)config;
    std::cout<<(std::string)TournamentConfig(config, player_list);
}


void main_test();



int main(){
#ifdef MIO_DEBUG
    std::cout<<"Siamo in debug mode\n";
#elif defined(MIO_RELEASE)
    std::cout<<"Siamo in release mode\n";
#else
    std::cout<<"La definizione non ha funzionato\n";
#endif
    mainGui();
}