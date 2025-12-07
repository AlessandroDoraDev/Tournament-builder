#include "TournamentConfig.h"
#include "to_strings.h"
#include <format>
#include <ranges>
#include "globals.h"
#include <fstream>
#include "Colors.h"
#include <functional>
#include <print>

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

double TournamentConfig::quality(){
    double max_pnts=0;
    double min_pnts=0;
    for(int i=0; i<m_n_rows; i++){
        int team_pnts=0;
        for(int j=0; j<m_n_cols; j++){
            const Player& p= m_config[i*m_n_cols+j];
            team_pnts+=static_cast<int>(p.rank);
        }
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
    return (max_pnts-min_pnts)/m_n_cols;
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

        td, td *{
            max-height: 60px !important;
            align-content: center;
        }
        
        
        td:nth-child(2n-1){
            border-right: none;
        }
        td:nth-child(2n){
            border-left: none;
        }

        tr {
            background-color: #424242;
        }
        
        .square-bg {
            aspect-ratio: 1;
            display: grid;
            place-items: center;
        }
        .square-fg{
            width: 80%;
            aspect-ratio: 1;
            display: grid;
            place-items: center;
        }
        .inner-gem {
            transform: rotate(45deg);
        }
        .rank-text{
            font-weight: bold;
            color: white;
            max-height: 10px;
        }
        .rank-gem {
            display:grid; 
            place-items: center;
        }
            
        .player-name{
            width:80%;
        }
    )";
    static constexpr std::string_view html_base=R"(
        <!DOCTYPE html>
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
                    <h1>Matchmaking</h1>
                </header>
                <main>
                    {}
                </main>
            </body>
        </html>
    )";
    static constexpr std::string_view rank_gem_base=R"(
        <div class="rank-gem">
            <div class="square-bg" style="width:100%; max-width:30px; background-color: hsl({}, {}, {});">
                <div class="square-fg" style="background-color: hsl({}, {}, {});">
                    <div class="inner-gem square-bg" style="width:100%; background-color: hsl({}, {}, {});">
                        <div class="square-fg" style="background-color: hsl({}, {}, {});"></div>
                    </div>
                </div>
            </div>
            <div class="rank-text" style="margin-top: 5px;">
                {}
            </div>
        </div>
    )";
    static constexpr std::string_view table_base="<table><tbody>{}</tbody></table>";
    static constexpr std::string_view row_base="<tr>{}</tr>";
    static constexpr std::string_view cell_base=R"(
        <td>
            <div class="player-name">{}</div>
        </td>
        <td>
            {}
        </td>
    )";
    static constexpr double bg_gem_light_increase=0.00;
    static constexpr double fg_gem_light_increase=0.15;
    auto [min_r, max_r]= std::minmax_element(STRING_RANK_TO_ENUM_MAP.begin(), STRING_RANK_TO_ENUM_MAP.end(),
        [](const auto& first_entry, const auto& second_entry){ //std::pair<const std::string, Rank>
            return first_entry.second<second_entry.second;
        }
    );
    static bool f=true;
    auto toHTMLPcntg=[](double val){return std::format("{}%", (int64_t)(val*100));};
    std::string out_html;
    std::string table;
    
    std::string rows;
    for(int i=0; i<m_n_rows; i++){
        std::string cols;
        for(int j=0; j<m_n_cols; j++){
            const Player& p= m_config[i*m_n_cols+j];
            ColorRGB col_rgb= mapValueToGradient(p.rank, min_r->second, max_r->second);
            ColorHSL col_hue= rgbToHsl(col_rgb);
            cols+=std::format(cell_base, 
                p.name, 
                std::format(rank_gem_base,
                    col_hue.hue, toHTMLPcntg(col_hue.saturation), toHTMLPcntg(col_hue.lighting+(1*bg_gem_light_increase)),
                    col_hue.hue, toHTMLPcntg(col_hue.saturation), toHTMLPcntg(col_hue.lighting+(1*fg_gem_light_increase)),
                    col_hue.hue, toHTMLPcntg(col_hue.saturation), toHTMLPcntg(col_hue.lighting+(1*bg_gem_light_increase)),
                    col_hue.hue, toHTMLPcntg(col_hue.saturation), toHTMLPcntg(col_hue.lighting+(1*fg_gem_light_increase)),
                    ENUM_RANK_TO_STRING_MAP[p.rank]
                )
            );
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
