#pragma once
#include "TeamSet.h"


struct TeamSetLess{
    bool operator()(const TeamSet&, const TeamSet&) const;
    bool operator()(const TeamSet*&, const TeamSet*&) const;
};
