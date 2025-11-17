#include <string>
#include <windows.h>
#include "TournamentBuilderAPI.h"
#include "matricesModule.h"
#include "Config.h"
#include "test_includes/utils.h"
#include "strat.h"
#include "TournamentConfig.h"
#include <print>

std::map<ErrorMessageCode, std::string> ERROR_MESSAGE_MAP={
    {COULD_NOT_OPEN_PLAYERS_CSV, "Could not open the players csv file or the file was empty..."},
    {COULD_NOT_OPEN_RANKS_CSV, "Could not open the ranks csv file or the file was empty..."},
    {RANKS_FILE_ILL_FORMED,  "Could not load ranks correctly, the ranks csv file was ill-formed..."},
    {PLAYERS_FILE_ILL_FORMED, "Could not load players correctly, the players csv file was ill-formed..."},
    {FOUND_PLAYERS_NOT_FITTING_TEAMS_SIZE, "Player list doesn't fit the teams' sizes..."}
};

BuildTournamentResult buildTournamentFromCSV(std::string players_csv_path, std::string ranks_csv_path) {
    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
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
    
    int n_teams = static_cast<int>(player_list.size() / N_PLAYERS);
    Config config=Config(n_teams, N_PLAYERS);
    if(!config.init(player_list)){
        return {TournamentConfig(), FOUND_PLAYERS_NOT_FITTING_TEAMS_SIZE};   
    }
    deduceAndApplyMoves(config);
    return {TournamentConfig(config, player_list), GOOD_TOURNAMENT_RESULT};
}
