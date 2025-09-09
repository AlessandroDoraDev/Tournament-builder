#pragma once
#include <utility>
#include "TeamSet.h"
#include "absl/container/btree_set.h"

using MoveGenConfigSet = absl::btree_set<std::pair<int, TeamSet>>;
