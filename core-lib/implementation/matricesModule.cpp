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
