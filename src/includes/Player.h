#pragma once
#include "globals.h"
#include <string>
#include "Rank.h"

struct Player {
    std::string name;
    Rank rank;
    friend std::ostream& operator<<(std::ostream& o, const Player& p);
    operator std::string() const{
        return name+" ("+ENUM_RANK_TO_STRING_MAP.at(rank)+")";
    }
};