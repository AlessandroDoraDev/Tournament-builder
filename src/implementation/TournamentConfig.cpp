#include "TournamentConfig.h"
#include "to_strings.h"
#include <format>
#include <ranges>

TournamentConfig::TournamentConfig(const Config& config, PlayerList p_list)
: m_n_rows(config.n_rows), m_n_cols(config.n_cols){
    for(const Rank& r: config.m_team_views | std::views::join){
        auto it=std::find_if(
            p_list.begin(), 
            p_list.end(), 
            [&r](const Player& p){return p.rank==r;});
        m_config.push_back(*it);
        if(it!=p_list.end()){
            p_list.erase(it);
        }
    }
}

TournamentConfig::operator std::string(){
    std::string res="";
    double max_pnts=0;
    double min_pnts=0;
    for(int i=0; i<m_n_rows; i++){
        int team_pnts=0;
        res+="[";
        for(int j=0; j<m_n_cols; j++){
            const Player& p= m_config[i*m_n_cols+j];
            team_pnts+=static_cast<int>(p.rank);
            res+=std::format("{:<25}", to_string(p))+", ";
        }
        res+="\b\b] ("
        +round_to_string(((double)team_pnts)/m_n_cols, 1)
        +", "+std::to_string(team_pnts)+")\n";
        if(max_pnts==0){
            max_pnts=team_pnts;
            min_pnts=team_pnts;
        }else{ 
            if(max_pnts<team_pnts){
                max_pnts=team_pnts;
            }
            if(min_pnts>team_pnts){
                min_pnts=team_pnts;
            }
        }
    }
    res+=std::format("Quality is: {}\n", round_to_string((max_pnts-min_pnts)/m_n_cols, 1));
    return res;
}

