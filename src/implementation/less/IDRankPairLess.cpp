#include "less/IDRankPairLess.h"

bool IDRankPairLess::operator()(const IDRankPair& pair, const IDRankPair& pair2) const{
    return pair.second<pair2.second;
}