#pragma once
#include <unordered_map>
#include "Rank.h"
#include <string>



inline constexpr int N_PLAYERS=5;

inline constexpr int ROTATION_SIZE=3;

inline constexpr std::string_view TEST_FILE_DIR = "../../../../assets/CorrettoOlly.csv";

extern const std::unordered_map<std::string, Rank> STRING_RANK_TO_ENUM_MAP;

extern const std::unordered_map<Rank, std::string> ENUM_RANK_TO_STRING_MAP;