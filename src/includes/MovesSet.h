#pragma once
#include "Move.h"
#include "absl/container/flat_hash_set.h"

template<int rotation_size=2>
using MovesSet = absl::flat_hash_set<Move<rotation_size>>;
