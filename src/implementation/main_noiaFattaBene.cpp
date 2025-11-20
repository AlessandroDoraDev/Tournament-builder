#include <iostream>
#include "TournamentBuilderGui.h"
#include "TournamentBuilderAPI.h"
#include "WindowsDefs.h"
#include <filesystem>
#include <charconv>
#include <cstring>
#include <system_error>

int cmdExecute(
    std::string_view players_csv_path, 
    std::string_view ranks_csv_path, 
    std::size_t n_players,
    std::size_t rotation_size,
    std::string_view working_directory
);

int parsePlayersAndRanks(
    std::size_t& n_players, 
    std::size_t& rotation_size, 
    std::string_view p_str,
    std::string_view r_str
);

enum Args: std::size_t{
    PLAYERS_CSV_PATH,
    RANKS_CSV_PATH,
    N_PLAYERS,
    ROTATION_SIZE,
    WORKING_DIRECTORY
};

int main(int argc, char** argv){
    
    win_setupConsole();
    
    std::size_t n_players=0;
    std::size_t rotation_size=0;


    switch(argc){
    case 1:
        mainGui();
        break;
    case 5:
        if(EXIT_FAILURE==parsePlayersAndRanks(n_players, rotation_size, argv[N_PLAYERS], argv[ROTATION_SIZE])){
            return EXIT_FAILURE;
        }
        
        return cmdExecute(
            argv[PLAYERS_CSV_PATH], 
            argv[RANKS_CSV_PATH], 
            n_players, 
            rotation_size,
            std::filesystem::current_path().string()
        ); 
        break; //<- no need but it will be optimized out anyway
    case 6:
        if(EXIT_FAILURE==parsePlayersAndRanks(n_players, rotation_size, argv[N_PLAYERS], argv[ROTATION_SIZE])){
            return EXIT_FAILURE;
        }
        return cmdExecute(
            argv[PLAYERS_CSV_PATH], 
            argv[RANKS_CSV_PATH], 
            n_players, 
            rotation_size,
            argv[WORKING_DIRECTORY]
        );
        break;
    default:
        std::cerr<<"Bad arguments"<<std::endl;
        break;
    }
    return EXIT_SUCCESS;
}


int cmdExecute(
    std::string_view players_csv_path, 
    std::string_view ranks_csv_path, 
    std::size_t n_players,
    std::size_t rotation_size,
    std::string_view working_directory){
    BuildTournamentResult res= buildTournamentFromCSV(players_csv_path.data(), ranks_csv_path.data(), n_players, rotation_size);
    if(res.error_code!=GOOD_TOURNAMENT_RESULT){
        std::cerr<<ERROR_MESSAGE_MAP.at(res.error_code)<<std::endl;
        return EXIT_FAILURE;
    }else{
        std::cout<<(std::string)res.tournament_config;
        res.tournament_config.genHTMLTable(working_directory.data());
    }
    return EXIT_SUCCESS;
}

int parsePlayersAndRanks(
    std::size_t& n_players, 
    std::size_t& rotation_size, 
    std::string_view p_str,
    std::string_view r_str
){
    auto n_players_res=std::from_chars(
        p_str.data(), 
        p_str.data()+p_str.length(),
        n_players
    );
    if(n_players_res.ec==std::errc::invalid_argument || n_players_res.ec==std::errc::result_out_of_range){
        std::cerr<<"Number of rotations badly parsed"<<std::endl;
        return EXIT_FAILURE;
    }
    auto rotation_size_res=std::from_chars(
        r_str.data(), 
        r_str.data()+p_str.length(),
        rotation_size
    );
    if(rotation_size_res.ec==std::errc::invalid_argument || rotation_size_res.ec==std::errc::result_out_of_range){
        std::cerr<<"Number of rotations badly parsed"<<std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}