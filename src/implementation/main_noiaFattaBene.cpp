
#include "generalIncludes.h"
#include <chrono>
#include <string>
#include "profiling/Instrumentor.h"
#include "absl/hash/hash.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/btree_set.h"


const int N_PLAYERS=5;
const int ROTATION_SIZE=3;

const std::string TEST_FILE_DIR = "../../../../assets/CorrettoOlly.csv";

void main1() {

    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows rows = readCSV(TEST_FILE_DIR);
    PlayerList player_list = formatRowsToPlayerList(rows);
    int n_teams = static_cast<int>(player_list.size() / N_PLAYERS);
    std::unordered_map<BaseID, Player> player_dict;
    //shuffleMatrixArray(player_list);
    //std::sort(player_list.begin(), player_list.end(),
    //    [](const Player& p1, const Player& p2) {
    //        return p1.rank < p2.rank;
    //    });
    ConfigMatrix config_matrix = teamsToConfigMatrix(player_list, player_dict);
    ConfigMatrix copy = config_matrix;
    //reorderTheMatrixToBeSnakeLike(copy, config_matrix, n_teams, N_PLAYERS);
    PlayerBag player_bag=makePlayerBag(config_matrix);

    ConfigSet config_set=configMatrixToSet(config_matrix, n_teams, N_PLAYERS);
    config_matrix=configSetToMatrix(config_set, player_bag);
    MovesSet<ROTATION_SIZE> moves_set;
    Move<ROTATION_SIZE> gen_move= {{0, Rank::Empty}, {0, Rank::Empty}, {0, Rank::Empty}};
    MarkedTConfigSet markedSet;
    std::transform(config_set.begin(), config_set.end(), std::back_inserter(markedSet),
        [](const TeamSet& team) { return &team; });
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    Instrumentor::Get().BeginSession("GenMovesProfiling");
    generateMoves<ROTATION_SIZE>(markedSet, moves_set, gen_move);
    Instrumentor::Get().EndSession();
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::cout << "Moves set size is "<< moves_set.size() << "\n";
    std::cout << "Moves generation took " << formatDurationDetailed(end-start) << "\n";

    /*absl::flat_hash_set<Move<3>> set;
    set.insert({ {0, Rank::Empty}, {0, Rank::Empty}, {0, Rank::Empty} });
    for(const Move<3>& n: set){
        std::cout << "\n" << (std::string)n;
    }*/
    printMatrix(config_matrix, n_teams, N_PLAYERS);
    std::cout << "Matrix quality is " << configMatrixQuality(config_matrix, n_teams, N_PLAYERS) << "\n";
    printMatrix(configMatrixToTeams(config_matrix, player_dict), n_teams, N_PLAYERS);


    // TeamsMatrix teams_matrix=configMatrixToTeams(config_matrix, player_dict);
    // printMatrix(teams_matrix, n_teams, N_PLAYERS);
    // reorderTheMatrixToBeSnakeLike(config_matrix, n_teams, N_PLAYERS);
    // TeamsMatrix teams_matrix2=configMatrixToTeams(config_matrix, player_dict);
    // printMatrix(teams_matrix2, n_teams, N_PLAYERS);

}

void main2() {

    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows rows = readCSV(TEST_FILE_DIR);
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

int main(){
#ifdef MIO_DEBUG
    std::cout<<"Siamo in debug mode\n";
#elif defined(MIO_RELEASE)
    std::cout<<"Siamo in release mode\n";
#else
    std::cout<<"La definizione non ha funzionato\n";
#endif
    main2();
}

/*
 * Bisogna implementare la generazione di mosse a partire da una config che ha gi� un set di mosse e una mossa generatrice.
 * Vanno generate le mosse legate
 */