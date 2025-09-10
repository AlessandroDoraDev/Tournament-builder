#include "less/TeamSetLess.h"
#include "globals.h"

bool TeamSetLess::operator()(const TeamSet& team_set, const TeamSet& team_set2) const{
    std::string team1, team2;
    team1.resize(N_PLAYERS);
    team2.resize(N_PLAYERS);
    auto push_casted_rank_lambda = [](const Rank& rank) {return static_cast<char>(rank); };
    std::transform(team_set.begin(), team_set.end(), team1.begin(),
        push_casted_rank_lambda);
    std::transform(team_set.begin(), team_set.end(), team2.begin(),
        push_casted_rank_lambda);
    return team1<team2;
}

bool TeamSetLess::operator()(const TeamSet*& team_set, const TeamSet*& team_set2) const{
    return TeamSetLess{}(*team_set, *team_set2);
}
