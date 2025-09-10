#include "less/RankLess.h"

bool RankLess::operator()(const Rank& rank, const Rank& rank2) const{
    return rank<rank2;
}
