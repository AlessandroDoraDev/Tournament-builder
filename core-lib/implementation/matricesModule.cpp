#include "matricesModule.h"
#include "globals.h"
#include <fstream>
#include <sstream>
#include <ranges>
#include <functional>
#include <algorithm>
#include <charconv>
#include <system_error>


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

struct RankCSVValidationToken{
    bool success;
    std::size_t first_line_size;
    std::size_t n_rows;
};

RankCSVValidationToken isRankCSVValid(const CSVRows& rank_rows){
    std::size_t first_line_size= rank_rows[0].size();
    std::size_t n_rows=0;
    for(const std::vector<std::string>& row: rank_rows){
        if(!(row.size()==1||row.size()==2)){
            return {false, first_line_size, n_rows};
        } 
        n_rows++;
    }
    return {true, first_line_size, n_rows};
}

std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    size_t end = s.find_last_not_of(" \t\n\r");

    if (start == std::string::npos)
        return "";  // String contains only whitespace
    else
        return s.substr(start, end - start + 1);
}

bool initRankMaps(const CSVRows& rank_rows){
    /*
    The csv has to be two columns of values of the kind: 
    any string, integer number
    or
    any string
    
    any string obviously not comma separated
    */
    RankCSVValidationToken token= isRankCSVValid(rank_rows);
    if(!token.success){
        return false;
    }
    ENUM_RANK_TO_STRING_MAP.clear();
    STRING_RANK_TO_ENUM_MAP.clear();
    for(std::size_t i=0; i<token.n_rows; i++){
        int rank_value;
        if(token.first_line_size==1){
            rank_value=i;
        }else{ // must be 2
            std::string entry=trim(rank_rows[i][1]);
            auto result=std::from_chars(entry.data(), entry.data()+entry.size(), rank_value);
            if(result.ec==std::errc::result_out_of_range 
                || result.ec==std::errc::invalid_argument 
                || result.ptr!=&(entry.back())+1){
                return false;
            }
        }
        STRING_RANK_TO_ENUM_MAP[rank_rows[i][0]]=rank_value;
        ENUM_RANK_TO_STRING_MAP[rank_value]=rank_rows[i][0];
    }
    if(!ENUM_RANK_TO_STRING_MAP.contains(0)){
        STRING_RANK_TO_ENUM_MAP["-"]=0;
        ENUM_RANK_TO_STRING_MAP[0]="-";
    }
    return true;
}

PlayerList formatRowsToPlayerList(const CSVRows& rows){
    //before reading:
    //this thing is extracting only and ONLY the first two columns of the csv
    //and it is considering only and ONLY a row containing a pair of cols made like this: any string, valid_rank
    //and that pair of cols must be aligned with an header row made like this: Player, Rank
    static std::string empty_rank_name="-";
    PlayerList ret;
    if(rows.size()==0){
        return ret;
    }
    std::size_t player_header_index;
    bool found_header=false;
    for(std::size_t i=0; i<rows[0].size()-1; i++){
        if(rows[0][i]=="Player"&&rows[0][i+1]=="Rank"){
            player_header_index=i;
            found_header=true;
        }
    }
    if(!found_header){
        return ret;
    }
    std::size_t rank_header_index=player_header_index+1;
    for(std::vector<std::string> cols: rows){
        if(player_header_index>=cols.size()){
            continue;
        }
        std::string player_name=trim(cols[player_header_index]);
        std::string rank_name;
        if(rank_header_index<cols.size()){
            rank_name=trim(cols[rank_header_index]);
        }else{
            rank_name="-";
        }
        if(!player_name.empty()){
            if(STRING_RANK_TO_ENUM_MAP.contains(rank_name)){
                ret.emplace_back(player_name, STRING_RANK_TO_ENUM_MAP.at(rank_name));
            }else if(rank_name=="-"){ // if no rank specified treat as - (zero value)
                ret.emplace_back(player_name, STRING_RANK_TO_ENUM_MAP.at(ENUM_RANK_TO_STRING_MAP.at(0)));
            }
        }
    }
    return ret;
}

std::ostream& operator<<(std::ostream& o, const Player& p){
    return o<<(std::string)p;
}
