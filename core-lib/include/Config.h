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
    Config(std::size_t n_rows, std::size_t n_cols);
    bool init(const PlayerList&);
    operator std::string();
    std::string obj_to_string(){return (std::string)(*this);}
    inline constexpr TeamArray& operator[](std::size_t index){return m_team_views[index];}
    void applyMove(const MoveArrT&);
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

