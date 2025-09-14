#include <iostream>
#include <string>
#include <windows.h>
#include <ranges>
#include "matricesModule.h"
#include "globals.h"
#include "MovesSet.h"
#include "Move.h"
#include "MarkedTConfigSet.h"
#include "silly_strats/funcs.h"
#include "test_includes/core.h"
#include "to_strings.h"

void my_assert(bool expression, std::string text_if_failure){
    if(!expression){
        std::cout<<text_if_failure<<"\n";
    }
}
template<typename ConType, typename ElemType>
inline bool my_contains(ConType arr, ElemType elem){
    return std::find(arr.begin(), arr.end(), elem)!=arr.end();
}

template<int rotation_size>
void MovesSet_difference(MovesSet<rotation_size> set1, MovesSet<rotation_size> set2){
    MovesSet<ROTATION_SIZE> difference;

    for (const auto& elem : set1) {
        if (set2.find(elem) == set2.end()) {
            // if(my_contains(elem.m_move, std::pair(3,0))
            // &&my_contains(elem.m_move, std::pair(5,2))
            // &&my_contains(elem.m_move, std::pair(5,2)))
                difference.insert(elem);
        }
    }

    // Stampa gli elementi della differenza
    std::cout<<"Difference: \n";
    for (const auto& elem : difference) {
        std::cout << (std::string)elem << ",\n";
    }
    std::cout<<"...\n";
}

void test_move_generation_more_efficient_not_differing_in_result(){
    
    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows rows = readCSV(TEST_FILE_DIR.data());
    PlayerList player_list = formatRowsToPlayerList(rows);
    int n_teams = static_cast<int>(player_list.size() / N_PLAYERS);
    std::unordered_map<BaseID, Player> player_dict;
    ConfigMatrix config_matrix = teamsToConfigMatrix(player_list, player_dict);
    ConfigMatrix copy = config_matrix;
    //reorderTheMatrixToBeSnakeLike(copy, config_matrix, n_teams, N_PLAYERS);
    PlayerBag player_bag=makePlayerBag(config_matrix);

    ConfigSet config_set=configMatrixToSet(config_matrix, n_teams, N_PLAYERS);
    config_matrix=configSetToMatrix(config_set, player_bag);
    MovesSet<ROTATION_SIZE> moves_set_integral;
    MovesSet<ROTATION_SIZE> moves_set_non_integral;
    Move<ROTATION_SIZE> gen_move= {{0, Rank::Empty}, {0, Rank::Empty}, {0, Rank::Empty}};
    MarkedTConfigSet markedSet;
    std::transform(config_set.begin(), config_set.end(), std::back_inserter(markedSet),
        [](const TeamSet& team) { return &team; });
    std::chrono::time_point start=std::chrono::high_resolution_clock::now();
    generateMoves<ROTATION_SIZE>(markedSet, moves_set_integral, gen_move, true);
    std::chrono::time_point end=std::chrono::high_resolution_clock::now();
    std::cout<<"Moves generation integral took: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start)<<"\n";
    start=std::chrono::high_resolution_clock::now();
    generateMoves<ROTATION_SIZE>(markedSet, moves_set_non_integral, gen_move, false);
    end=std::chrono::high_resolution_clock::now();
    std::cout<<"Moves generation non integral took: "<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start)<<"\n";
    std::cout << "Moves set integral size is "<< moves_set_integral.size() << "\n";
    std::cout << "Moves set non-integral size is "<< moves_set_non_integral.size() << "\n";
    my_assert(moves_set_integral.size()==moves_set_non_integral.size(), "Generated moves are not the same size");
    std::cout<<"Moves in integral and not in sped up...\n";
    MovesSet_difference(moves_set_integral, moves_set_non_integral);
    std::cout<<"Moves in sped up and not in integral...\n";
    MovesSet_difference(moves_set_non_integral, moves_set_integral);
    std::cout<<"MarkedSet is...\n";
    std::cout<<to_string(markedSet);
}

void main_test(){
    std::cout<<"Esecuzione test\n";
    test_move_generation_more_efficient_not_differing_in_result();
}