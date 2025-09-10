#pragma once
#include "TeamForMoveGen.h"
#include "less/TeamSetLess.h"

struct TeamForMoveGenLess{
    bool operator()(TeamForMoveGen*& g1, TeamForMoveGen*& g2){
        return TeamSetLess{}(g1->team_set_ptr, g2->team_set_ptr);
    }
};