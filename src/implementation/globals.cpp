#pragma once
#include "globals.h"


const std::unordered_map<std::string, Rank> STRING_RANK_TO_ENUM_MAP={
    {"Empty", Rank::Empty},
    {"Iron", Rank::Iron},
    {"Bronze", Rank::Bronze},
    {"Silver", Rank::Silver},
    {"Gold", Rank::Gold},
    {"Platinum", Rank::Platinum},
    {"Diamond", Rank::Diamond},
    {"Ascendant", Rank::Ascendant},
    {"Immortal", Rank::Immortal},
    {"Radiant", Rank::Radiant}
};

const std::unordered_map<Rank, std::string> ENUM_RANK_TO_STRING_MAP={
    {Rank::Empty, "Empty"},
    {Rank::Iron, "Iron"},
    {Rank::Bronze, "Bronze"},
    {Rank::Silver, "Silver"},
    {Rank::Gold, "Gold"},
    {Rank::Platinum, "Platinum"},
    {Rank::Diamond, "Diamond"},
    {Rank::Ascendant, "Ascendant"},
    {Rank::Immortal, "Immortal"},
    {Rank::Radiant, "Radiant"}
};