#include "hash/VisitedConfigSetHash.h"
#include "BaseRank.h"


size_t VisitedConfigSetHash::operator()(const ConfigSet& config_set) const{
    std::string stringification;
    for(const TeamSet& team: config_set){
        std::transform(team.begin(), team.end(), std::back_inserter(stringification),
            [](const Rank& rank) { return static_cast<char>(static_cast<BaseRank>(rank) + '0'); });
    }
    std::hash<std::string> hasher;
    return hasher(stringification);
}
