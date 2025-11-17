#include "TournamentConfig.h"
#include "to_strings.h"
#include <format>
#include <ranges>
#include "globals.h"
#include <fstream>

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
        res+="[\n";
        for(int j=0; j<m_n_cols; j++){
            const Player& p= m_config[i*m_n_cols+j];
            team_pnts+=static_cast<int>(p.rank);
            res+=std::format("{:<25}", to_string(p))+"\n";
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


void TournamentConfig::genHTMLTable(std::string path){
    static constexpr std::string_view style_settings=R"(
        *{
            color:white;
        }
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #151415;
        }

        header {
            margin-bottom: 20px;
        }

        table {
            border-collapse: collapse;
            width: 100%;
        }
        
        th, td {
            border: 3px solid yellow;
            padding: 8px 12px;
            text-align: center;
            vertical-align: middle;
        }

        tr {
        background-color: #424242;
        })";
    static constexpr std::string_view html_base=
    R"(<!DOCTYPE html>
    <html lang="en">
    <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tournament</title>
    <style>
        {}
    </style>
    </head>
    <body>
    <header>
        <h1>My Table Header</h1>
    </header>
    <main>
    {}
    </main>
    </body>
    </html>)";
    static constexpr std::string_view table_base="<table><tbody>{}</tbody></table>";
    static constexpr std::string_view row_base="<tr>{}</tr>";
    static constexpr std::string_view cell_base="<td>{}<br>{}</td>";
    std::string out_html;
    std::string table;
    
    std::string rows;
    for(int i=0; i<m_n_rows; i++){
        std::string cols;
        for(int j=0; j<m_n_cols; j++){
            const Player& p= m_config[i*m_n_cols+j];
            cols+=std::format(cell_base, p.name, ENUM_RANK_TO_STRING_MAP[p.rank]);
        }
        rows+=std::format(row_base, cols);
    }
    table=std::format(table_base, rows);
    out_html=std::format(html_base, style_settings, table);
    std::ofstream out(path);
    if(!out){
        std::cerr<<"Failed to open file: "<<path<<"\n";
        return;
    }
    out<<out_html;
    out.close();
}
