#include <string>
#include <windows.h>
#include "TournamentBuilderAPI.h"
#include "matricesModule.h"
#include "Config.h"
#include "test_includes/utils.h"
#include "strat.h"
#include "TournamentConfig.h"
#include <print>

void buildTournamentFromCSV(std::string players_csv_path, std::string ranks_csv_path) {

    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    CSVRows players_csv_rows = readCSV(players_csv_path);
    CSVRows ranks_csv_rows= readCSV(ranks_csv_path);
    if(!initRankMaps(ranks_csv_rows)){
        std::println("Could not load ranks...");
        return;
    }
    PlayerList player_list = formatRowsToPlayerList(players_csv_rows);
    int n_teams = static_cast<int>(player_list.size() / N_PLAYERS);
    Config config=Config(n_teams, N_PLAYERS);
    if(!config.init(player_list)){
        std::println("Player list doesn't fit the teams' sizes... player list size: {}", 
            player_list.size());
    }
    timeThis([&](){
        deduceAndApplyMoves(config);
    });
    std::cout<<(std::string)config;
    std::cout<<(std::string)TournamentConfig(config, player_list);
}
