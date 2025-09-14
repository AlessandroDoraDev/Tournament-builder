#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <algorithm>
#include <array>
#include <thread>
#include <utility>
#include <ranges>
#include <format>
#include <random>
#include <mutex>
#include "ConfigMatrix.h"
#include "CircularRangeView.h"
#include "macros/myMacros.h"
#include "matricesModule.h"

std::string formatDurationDetailed(std::chrono::nanoseconds ns);


FORCE_INLINE void reorderTheMatrixToBeSnakeLike(ConfigMatrix& config_matrix, ConfigMatrix& out_matrix, int rows, int cols);

void strategicallyShuffleMatrixArray(ConfigMatrix& matrix, int shuffle_radius);


void findMatrixArrayBreaks(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks, int shuffle_radius);

void findMatrixArrayBreaks2(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks, int shuffle_radius);

FORCE_INLINE void strategicallyShuffleMatrixArray(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks);

ConfigMatrix simpleStrategy(ConfigMatrix& matrix, int n_teams, int n_players);

ConfigMatrix dumbStrategy(ConfigMatrix& matrix, int n_teams, int n_players);