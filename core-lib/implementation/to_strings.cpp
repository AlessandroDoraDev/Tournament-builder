#include "to_strings.h"
#include <format>
#include <iomanip>
#include <sstream>
#include <cmath>
#include "globals.h"

std::string to_string(const Rank& rank){
    return std::to_string(static_cast<int>(rank));
}


std::string to_string(const Player& player){
    return std::format("({}, {})", 
        player.name, 
        ENUM_RANK_TO_STRING_MAP.at(player.rank));
}

std::string round_to_string(double value, int n) {
    double factor = std::pow(10.0, n);
    double rounded_value = std::round(value * factor) / factor;
    std::ostringstream ss;
    ss<<std::fixed << std::setprecision(n)<<rounded_value;
    return ss.str();
}