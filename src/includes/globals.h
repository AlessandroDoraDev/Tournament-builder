#pragma once
#include <unordered_map>
#include "Rank.h"
#include <string>


extern const int N_PLAYERS;

extern const std::unordered_map<std::string, Rank> STRING_RANK_TO_ENUM_MAP;

extern const std::unordered_map<Rank, std::string> ENUM_RANK_TO_STRING_MAP;