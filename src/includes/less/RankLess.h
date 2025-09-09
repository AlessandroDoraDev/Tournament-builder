#pragma once
#include "Rank.h"

struct RankLess{
    bool operator()(const Rank&, const Rank&) const;
};