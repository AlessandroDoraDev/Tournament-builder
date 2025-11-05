#pragma once
#include "TeamArray.h"
#include "Rank.h"
#include <vector>
#include "PlayerList.h"
#include <cstddef>
#include "MoveArrT.h"
#include <algorithm>

class Config{
public:
    friend class TournamentConfig;
public:
    Config(const PlayerList&, std::size_t n_rows, std::size_t n_cols);
    operator std::string();
    std::string obj_to_string(){return (std::string)(*this);}
    inline constexpr TeamArray& operator[](std::size_t index){return m_team_views[index];}
    template<int rotation_size=2>
    void applyMove(const MoveArrT<rotation_size>&);
    std::vector<Rank>::const_iterator begin() const {return m_config_matrix.cbegin();}
    std::vector<Rank>::const_iterator end() const { return m_config_matrix.cend();}
    std::vector<Rank>::const_iterator cbegin() const {return m_config_matrix.cbegin();}
    std::vector<Rank>::const_iterator cend() const {return m_config_matrix.cend();}
    void sort();
public:
    const std::size_t& n_rows=m_n_rows;
    const std::size_t& n_cols=m_n_cols;
    const int& at_least_pnts_per_team=m_at_least_pnts_per_team;
    const double& avg_rank=m_avg_rank;
private:
    std::vector<Rank> m_config_matrix;
    std::vector<TeamArray> m_team_views;
    std::size_t m_n_rows;
    std::size_t m_n_cols;
    int m_at_least_pnts_per_team; //this is at least. At most is the same number +1
    double m_avg_rank;
};


template<int rotation_size>
void Config::applyMove(const MoveArrT<rotation_size>& move){
    auto end=move.end()-1;
    auto it= move.begin();
    auto next=it+1;
    for(; it!=end; ++it, ++next){
        const MovePieceNRankPair& m= *it;
        const MovePieceNRankPair& m2= *next;
        TeamArray& team= m_team_views[m2.first];
        team.replaceAt(m2.second, m.second);
    }
    m_team_views[move.begin()->first].replaceAt(
        move.front().second, 
        end->second);
    std::sort(m_team_views.begin(), m_team_views.end(),
    [this](TeamArray& a, TeamArray&b){
        return std::lexicographical_compare(
            a.m_span.begin(), a.m_span.end(),
            b.m_span.begin(), b.m_span.end());
    });
}