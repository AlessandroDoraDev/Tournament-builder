#pragma once
#include "TeamMap.h"
#include "TeamSet.h"

struct TeamForMoveGen{
    int mark;
    const TeamSet* team_set_ptr; 
    TeamMap team_map;
};