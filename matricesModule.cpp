#include "matricesModule.h"
#include <fstream>
#include <sstream>
#include <ranges>
#include <functional>
#include <algorithm>

bool IDRankPairLess::operator()(const IDRankPair& pair, const IDRankPair& pair2) const{
    return pair.second<pair2.second;
}

bool TeamSetLess::operator()(const TeamSet& team_set, const TeamSet& team_set2) const{
    std::string team1, team2;
    team1.resize(N_PLAYERS);
    team2.resize(N_PLAYERS);
    auto push_casted_rank_lambda = [](const Rank& rank) {return static_cast<char>(rank); };
    std::transform(team_set.begin(), team_set.end(), team1.begin(),
        push_casted_rank_lambda);
    std::transform(team_set.begin(), team_set.end(), team2.begin(),
        push_casted_rank_lambda);
    return team1<team2;
}

bool TeamSetLess::operator()(const TeamSet*& team_set, const TeamSet*& team_set2) const{
    return TeamSetLess{}(*team_set, *team_set2);
}

bool RankLess::operator()(const Rank& rank, const Rank& rank2) const{
    return rank<rank2;
}


size_t VisitedConfigSetHash::operator()(const ConfigSet& config_set) const{
    std::string stringification;
    for(const TeamSet& team: config_set){
        std::transform(team.begin(), team.end(), std::back_inserter(stringification),
            [](const Rank& rank) { return static_cast<char>(static_cast<BaseRank>(rank) + '0'); });
    }
    std::hash<std::string> hasher;
    return hasher(stringification);
}


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

ConfigMatrix configSetToMatrix(const ConfigSet& config_set, PlayerBag player_bag){ // need to know Matrix dims beforehand. Might make it more efficient by passing it. But it's not needed rn
    ConfigMatrix out;
    for(TeamSet team: config_set){
        for(Rank rank: team){
            size_t pos=static_cast<size_t>(rank);
            IDRankPair p=player_bag[pos].back();
            player_bag[pos].pop_back();
            out.push_back(p);
        }
    }
    return out;
}


PlayerBag makePlayerBag(const ConfigMatrix& player_list){
    PlayerBag bag;
    bag.resize(static_cast<size_t>(Rank::COUNT));
    for(std::vector<IDRankPair>& tier: bag){
        tier.reserve(N_PLAYERS);
    }
    for(IDRankPair p: player_list){
        bag[static_cast<size_t>(p.second)].push_back(p);
    }
    return bag;
}

float configMatrixQuality(const ConfigMatrix& matrix, int rows, int cols){
    auto range= std::ranges::views::iota;
    float min=-1;
    float max=-1;
    for(int i: range(0, rows)){
        float team_avg = std::accumulate(
            matrix.begin() + i * cols, 
            matrix.begin() + (i + 1) * cols, 
            0.0,
            [](float acc, const IDRankPair& player) {return acc + player.second; });
        team_avg/=cols;
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