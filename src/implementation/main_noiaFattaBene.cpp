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
#include "core.h"
#include "to_strings.h"
#include "matricesModule.h"
#include "silly_strats/funcs.h"
#include "Config.h"
#include "TournamentConfig.h"
#include "strat.h"

template<typename LambdaToTime>
void timeThis(LambdaToTime l){
    
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    Instrumentor::Get().BeginSession("GenMovesProfiling");
        l();
    Instrumentor::Get().EndSession();
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::cout << "Moves generation took " << formatDurationDetailed(end-start) << "\n";
}

void main1() {

    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows rows = readCSV(TEST_FILE_DIR.data());
    PlayerList player_list = formatRowsToPlayerList(rows);
    int n_teams = static_cast<int>(player_list.size() / N_PLAYERS);
    Config config=Config(player_list, n_teams, N_PLAYERS);
    // std::vector<Move<ROTATION_SIZE>> moves={
    //     {
    //     {0, static_cast<Rank>(0)}, 
    //     {9, static_cast<Rank>(5)}, 
    //     {10, static_cast<Rank>(7)}
    //     },
    //     {
    //     {0, static_cast<Rank>(0)}, 
    //     {9, static_cast<Rank>(0)}, 
    //     {10, static_cast<Rank>(7)}
    //     },
    //     {
    //     {0, static_cast<Rank>(0)}, 
    //     {9, static_cast<Rank>(5)}, 
    //     {10, static_cast<Rank>(8)}
    //     },
    //     {
    //     {0, static_cast<Rank>(0)}, 
    //     {9, static_cast<Rank>(5)}, 
    //     {10, static_cast<Rank>(8)}
    //     },
    //     {
    //     {0, static_cast<Rank>(0)}, 
    //     {9, static_cast<Rank>(5)}, 
    //     {10, static_cast<Rank>(8)}
    //     },
    //     {
    //     {0, static_cast<Rank>(5)}, 
    //     {9, static_cast<Rank>(7)}, 
    //     {10, static_cast<Rank>(8)}
    //     },
    //     {
    //     {0, static_cast<Rank>(6)}, 
    //     {9, static_cast<Rank>(5)}, 
    //     {10, static_cast<Rank>(7)}
    //     },
    //     {
    //     {0, static_cast<Rank>(6)}, 
    //     {1, static_cast<Rank>(6)}, 
    //     {7, static_cast<Rank>(8)}
    //     },
    //     {
    //     {3, static_cast<Rank>(8)}, 
    //     {2, static_cast<Rank>(8)}, 
    //     {1, static_cast<Rank>(7)}
    //     }
    // };
    // for(const Move<ROTATION_SIZE>& m: moves){
    //     config.applyMove(m);
    // }
    timeThis([&](){
        deduceAndApplyMoves(config);
    });
    std::cout<<(std::string)config;
    std::cout<<(std::string)TournamentConfig(config, player_list);
    // MovesSet<ROTATION_SIZE> moves_set;



    // timeThis([&moves_set](){
    //     //generateMoves<ROTATION_SIZE>(team_to_int_map, markedSet, moves_set, gen_move);
    //     //alt_generateMovesFromEmptySet(config_set_on_maps, moves_set);
    // });
    
    // std::cout << "Moves set size is "<< moves_set.size() << "\n";
}

void main2() {

    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows rows = readCSV(TEST_FILE_DIR.data());
    PlayerList player_list = formatRowsToPlayerList(rows);
    int n_teams = static_cast<int>(player_list.size() / N_PLAYERS);
    std::unordered_map<BaseID, Player> player_dict;
    std::sort(player_list.begin(), player_list.end(),
        [](const Player& p1, const Player& p2) {
            return p1.rank < p2.rank;
        });
    ConfigMatrix config_matrix = teamsToConfigMatrix(player_list, player_dict);
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    config_matrix = dumbStrategy(config_matrix, n_teams, N_PLAYERS);
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::cout << "Teams generation took " << formatDurationDetailed(end - start) << "\n";
    std::cout << configMatrixQuality(config_matrix, n_teams, N_PLAYERS) << "\n";
    printMatrix(configMatrixToTeams(config_matrix, player_dict), n_teams, N_PLAYERS);

}

void main3(){
    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows rows = readCSV(TEST_FILE_DIR.data());
    PlayerList player_list = formatRowsToPlayerList(rows);
    std::vector<int> v;
    v.resize(player_list.size());
    std::transform(player_list.begin(), player_list.end(), v.begin(),
    [](const Player& p){return static_cast<int>(p.rank);});
    auto matrix=algoritmoGreedy(v, 5);
    TournamentConfig t_config(matrix, player_list, matrix.size(), (std::size_t)5);
    std::cout<<(std::string)t_config;
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
    main1();
}

/*
 * Bisogna implementare la generazione di mosse a partire da una config che ha gi� un set di mosse e una mossa generatrice.
 * Vanno generate le mosse legate
 */