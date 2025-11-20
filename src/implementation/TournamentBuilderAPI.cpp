#include <string>
#include <windows.h>
#include "TournamentBuilderAPI.h"
#include "matricesModule.h"
#include "Config.h"
#include "test_includes/utils.h"
#include "strat.h"
#include "TournamentConfig.h"
#include <print>
#include <cassert>

std::map<ErrorMessageCode, std::string> ERROR_MESSAGE_MAP={
    {ZERO_N_PLAYERS, "There are no possible arrangements for teams with no players."},
    {IMPOSSIBLE_ROTATION_SIZE, "The algorithm pace number must be more than 1 and less or equal than the total number of players"},
    {COULD_NOT_OPEN_PLAYERS_CSV, "Could not open the players csv file or the file was empty..."},
    {COULD_NOT_OPEN_RANKS_CSV, "Could not open the ranks csv file or the file was empty..."},
    {RANKS_FILE_ILL_FORMED,  "Could not load ranks correctly, the ranks csv file was ill-formed..."},
    {PLAYERS_FILE_ILL_FORMED, "Could not load players correctly, the players csv file was ill-formed..."},
    {FOUND_PLAYERS_NOT_FITTING_TEAMS_SIZE, "Player list doesn't fit the teams' sizes..."}
};

BuildTournamentResult buildTournamentFromCSV(std::string players_csv_path, std::string ranks_csv_path, std::size_t n_players, std::size_t rotation_size) {
    

    CSVRows rows = readCSV(players_csv_path);
    if(rows.size()==0){
        return {TournamentConfig(), COULD_NOT_OPEN_PLAYERS_CSV};
    }
    CSVRows ranks_csv_rows= readCSV(ranks_csv_path);
    if(ranks_csv_rows.size()==0){
        return {TournamentConfig(), COULD_NOT_OPEN_RANKS_CSV};
    }
    if(!initRankMaps(ranks_csv_rows)){
        return {TournamentConfig(), RANKS_FILE_ILL_FORMED};
    }
    PlayerList player_list = formatRowsToPlayerList(rows);
    if(player_list.size()==0){
        return {TournamentConfig(), PLAYERS_FILE_ILL_FORMED};
    }
    if(n_players<=0){
        return {TournamentConfig(), ZERO_N_PLAYERS};
    }
    if(rotation_size<=1||rotation_size>player_list.size()){
        return {TournamentConfig(), IMPOSSIBLE_ROTATION_SIZE};
    }
    
    int n_teams = static_cast<int>(player_list.size() / n_players);
    Config config=Config(n_teams, n_players);
    if(!config.init(player_list)){
        return {TournamentConfig(), FOUND_PLAYERS_NOT_FITTING_TEAMS_SIZE};   
    }
    deduceAndApplyMoves(config, rotation_size);
    return {TournamentConfig(config, player_list), GOOD_TOURNAMENT_RESULT};
}
