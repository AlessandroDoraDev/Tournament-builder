#include "Config.h"
#include <algorithm>
#include <iterator>
#include "to_strings.h"

Config::Config(const PlayerList& player_list, std::size_t m_n_rows, std::size_t m_n_cols)
: m_n_rows(m_n_rows), m_n_cols(m_n_cols){
    if(m_n_rows*m_n_cols!=player_list.size()){
        return;
    }
    m_config_matrix.reserve(player_list.size());
    std::transform(player_list.begin(), player_list.end(), std::back_inserter(m_config_matrix),
        [](const Player& p){return p.rank;});
    for(std::size_t i= 0; i<m_n_rows; i++){
        std::size_t b=m_n_cols*i; //begin
        std::size_t e=b+m_n_cols; //end
        m_team_views.emplace_back(
            m_config_matrix.begin()+b,
            m_config_matrix.begin()+e);
    }
    int total_points= std::accumulate(
        m_config_matrix.begin(), 
        m_config_matrix.end(), 
        0,
        [](int acc, const Rank& r){return acc+static_cast<int>(r);});
    m_at_least_pnts_per_team=total_points/m_n_rows;
    m_avg_rank=static_cast<double>(total_points)/(m_n_rows*m_n_cols);
    sort();
}

Config::operator std::string(){
    int i=0;
    int j=0;
    std::string res="{\n";
    for(int i=0; i<m_n_rows; i++){
        res+="[";
        for(int j=0; j<m_n_cols; j++){
            res+=to_string((*this)[i][j])+", ";
        }
        if(i!=m_n_rows-1){
            res+="\b\b],\n";
        }else{
            res+="\b\b]";
        }
    }
    res+="\n}\nAt least pnts per team: "
        +std::to_string(m_at_least_pnts_per_team)+"\n";
    return res;
}



void Config::sort(){
    for(int i=0; i<m_n_rows; i++){
        (*this)[i].sort();
    }
    std::sort(m_team_views.begin(), m_team_views.end(),
    [this](TeamArray& a, TeamArray&b){
        return std::lexicographical_compare(
            a.m_span.begin(), a.m_span.end(),
            b.m_span.begin(), b.m_span.end());
    });
}