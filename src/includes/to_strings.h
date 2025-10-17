#pragma once
#include <iostream>
#include <string>
#include "TeamSet.h"
#include "Player.h"


std::string to_string(const Rank& rank);

std::string to_string(const Player& player);

std::string to_string(const TeamSet& team_set);

std::string round_to_string(double value, int n);


template<typename BeginIt, typename EndIt, typename ElemToStringFunc>
std::string sequence_to_string(BeginIt begin, EndIt end, ElemToStringFunc elem_to_string_func, std::string separator=", "){
    std::string res; 
    for(; begin!=end; ++begin){
        res+=elem_to_string_func(*begin)+separator;
    }
    res.erase(res.size()-separator.size());
    return res;
}