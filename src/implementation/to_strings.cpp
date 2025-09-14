#include "to_strings.h"
#include <format>


inline std::string to_string(const Rank& rank){
    return std::to_string(static_cast<int>(rank));
}

std::string to_string(const TeamSet& team_set){
    std::string pre_res=sequence_to_string(
        team_set.begin(), 
        team_set.end(), 
        static_cast<std::string(*)(const Rank&)>(to_string));
    return std::format("[{}]", pre_res);
}

std::string to_string(const MarkedTConfigSet& config_set){
    std::string res;
    int i=0;
    for(const TeamSet*const& team_ptr: config_set){
        res+=std::format("{{{}, {}}}\n", i, to_string(*team_ptr));
        i++;
    }
    return res;
}