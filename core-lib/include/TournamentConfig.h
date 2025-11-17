#pragma once
#include "Config.h"
#include <ranges>
#include <span>

class TournamentConfig{
public:
    TournamentConfig(): m_n_rows(0), m_n_cols(0){}
    TournamentConfig(const Config&, PlayerList);
    template<typename ContainerIntMatrix>
    TournamentConfig(const ContainerIntMatrix& config, PlayerList p_list, std::size_t n_rows, std::size_t n_cols):
        m_n_rows(n_rows), m_n_cols(n_cols){
        auto new_config= config | std::views::join;
        for(const int& r: new_config){
            auto it=std::find_if(
                p_list.begin(), 
                p_list.end(), 
                [&r](const Player& p){return static_cast<int>(p.rank)==r;});
            m_config.push_back(*it);
            if(it!=p_list.end()){
                p_list.erase(it);
            }
        }
    }
    operator std::string();
    std::span<Player> operator[](std::size_t i){
        return std::span(m_config.begin()+(m_n_cols*i), m_config.begin()+(m_n_cols*(i+1)));
    }
    void genHTMLTable(std::string path);
    std::size_t nRows(){return m_n_rows;}
    std::size_t nCols(){return m_n_cols;}
    std::size_t matrixSize(){return m_config.size();}
private:
    std::vector<Player> m_config;
    std::size_t m_n_rows;
    std::size_t m_n_cols;
};