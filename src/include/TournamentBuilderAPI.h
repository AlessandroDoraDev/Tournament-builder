#pragma once
#include <string>
#include "TournamentConfig.h"
#include <map>
#include <string>

enum ErrorMessageCode{
    ZERO_N_PLAYERS,
    IMPOSSIBLE_ROTATION_SIZE,
    GOOD_TOURNAMENT_RESULT,
    COULD_NOT_OPEN_PLAYERS_CSV,
    COULD_NOT_OPEN_RANKS_CSV,
    RANKS_FILE_ILL_FORMED,
    PLAYERS_FILE_ILL_FORMED,
    FOUND_PLAYERS_NOT_FITTING_TEAMS_SIZE
};

extern std::map<ErrorMessageCode, std::string> ERROR_MESSAGE_MAP;

struct BuildTournamentResult{
    TournamentConfig tournament_config;
    ErrorMessageCode error_code;
};

BuildTournamentResult buildTournamentFromCSV(std::string csv_path, std::string ranks_csv_path, std::size_t n_players, std::size_t rotation_size);