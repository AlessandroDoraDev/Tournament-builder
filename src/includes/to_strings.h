#include <iostream>
#include <string>
#include "MarkedTConfigSet.h"
#include "TeamSet.h"


std::string to_string(const Rank& rank);

std::string to_string(const MarkedTConfigSet& config_set);

std::string to_string(const TeamSet& team_set);

template<typename BeginIt, typename EndIt, typename ElemToStringFunc>
std::string sequence_to_string(BeginIt begin, EndIt end, ElemToStringFunc elem_to_string_func){
    std::string res;
    static const std::string separator=", "; 
    for(; begin!=end; ++begin){
        res+=elem_to_string_func(*begin)+separator;
    }
    res.erase(res.size()-separator.size());
    return res;
}