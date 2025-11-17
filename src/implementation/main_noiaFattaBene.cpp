#include <iostream>
#include "TournamentBuilderGui.h"
#include "TournamentBuilderAPI.h"


int main(int argc, char** argv){
    if(argc==1){
        mainGui();
    }else if(argc==3){
        BuildTournamentResult res= buildTournamentFromCSV(argv[1], argv[2]);
        if(res.error_code!=GOOD_TOURNAMENT_RESULT){
            std::cerr<<ERROR_MESSAGE_MAP.at(res.error_code)<<std::endl;
            return EXIT_FAILURE;
        }else{
            std::cout<<(std::string)res.tournament_config;
        }
    }else{
        std::cerr<<"Bad arguments"<<std::endl;
    }
    return EXIT_SUCCESS;
}