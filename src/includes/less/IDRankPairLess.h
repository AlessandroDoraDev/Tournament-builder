#pragma once
#include "IDRankPair.h"


struct IDRankPairLess{
    bool operator()(const IDRankPair&, const IDRankPair&) const;
};