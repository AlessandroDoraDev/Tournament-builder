#pragma once
#include <utility>
#include "MovePieceNBaseRankPair.h"

template<int rotation_size=2>
using MoveArrT= std::array<MovePieceNBaseRankPair, rotation_size>;
