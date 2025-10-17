#pragma once
#include <utility>
#include "MovePieceNRankPair.h"

template<int rotation_size=2>
using MoveArrT= std::array<MovePieceNRankPair, rotation_size>;
