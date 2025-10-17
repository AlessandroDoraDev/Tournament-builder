#include "matricesModule.h"
#include "globals.h"
#include <fstream>
#include <sstream>
#include <ranges>
#include <functional>
#include <algorithm>


CSVRows readCSV(const std::string& filename) {
    CSVRows rows;
    std::ifstream file(filename);
    if (!file.is_open()) return rows;
    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        rows.push_back(row);
    }
    return rows;
}

PlayerList formatRowsToPlayerList(const CSVRows& rows){
    PlayerList ret;
    static std::string empty_rank_name="-";
    for(std::vector<std::string> cols: rows){
        if(cols.size()>=2 && STRING_RANK_TO_ENUM_MAP.contains(cols[1])){
            Player p={cols[0], STRING_RANK_TO_ENUM_MAP.at(cols[1])};
            ret.push_back(p);
        }else if(!(cols.size()==1&&cols[0].size()==0)
            &&cols[1]==std::string(empty_rank_name)){
            Player p={cols[0], Rank::Empty};
            ret.push_back(p);
        }
    }
    return ret;
}

std::ostream& operator<<(std::ostream& o, const Player& p){
    return o<<(std::string)p;
}

TeamsMatrix configMatrixToTeams(const ConfigMatrix& config_matrix, const std::unordered_map<BaseID, Player>& player_dict){
    TeamsMatrix teams_matrix;
    for(const IDRankPair& pair: config_matrix){
        int id= pair.first;
        if(player_dict.contains(id)){
            Player p{player_dict.at(id).name, static_cast<Rank>(pair.second)};
            teams_matrix.push_back(p);
        }
    }
    return teams_matrix;
}

ConfigMatrix teamsToConfigMatrix(const TeamsMatrix& teams_matrix, std::unordered_map<BaseID, Player>& player_dict){
    auto range= std::ranges::views::iota; //just aliasing the function
    if(!player_dict.empty()){
        player_dict.clear();
        player_dict.reserve(teams_matrix.size());
    }
    ConfigMatrix config_matrix;
    config_matrix.reserve(teams_matrix.size());
    for(size_t given_player_id: range((size_t)0, teams_matrix.size())){
        Player p=teams_matrix.at(given_player_id);
        BaseRank corresponding_rank=static_cast<BaseRank>(p.rank);
        player_dict.emplace(static_cast<BaseID>(given_player_id), std::move(p));
        config_matrix.emplace_back(static_cast<BaseID>(given_player_id), corresponding_rank);
    }
    return config_matrix;
}

ConfigSet configMatrixToSet(const ConfigMatrix& config_matrix, int rows, int cols){
    auto range= std::ranges::views::iota; //just aliasing the function
    ConfigSet out;
    for(int i: range(0, rows)){
        TeamSet team;
        for(int j: range(0, cols)){
            team.insert(static_cast<Rank>(config_matrix[i*cols+j].second));
        }
        out.insert(team);
    }
    return out;
}



double configMatrixQuality(const ConfigMatrix& matrix, int rows, int cols){
    auto range= std::ranges::views::iota;
    double min=-1;
    double max=-1;
    for(int i: range(0, rows)){
        double team_avg = std::accumulate(
            matrix.begin() + i * cols, 
            matrix.begin() + (i + 1) * cols, 
            0.0,
            [](double acc, const IDRankPair& player) {return acc + player.second; });
        //team_avg/=cols;   //no need to involve division. It can be done manually with no worries
        if(min==-1 && max==-1){
            min=max=team_avg;
        }else{
            if(min>team_avg){
                min=team_avg;
            }
            if(max<team_avg){
                max=team_avg;
            }
        }
    }
    return max-min;
}




std::ostream& operator<<(std::ostream& os, const IDRankPair& p){
    return os<<"("<<static_cast<int>(p.first)<<", "<<static_cast<int>(p.second)<<")";
}