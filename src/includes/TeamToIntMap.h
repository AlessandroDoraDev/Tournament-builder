#pragma once
#include "absl/container/flat_hash_map.h"
#include "TeamSet.h"
#include "BaseID.h"

using TeamToIntMap= absl::flat_hash_map<TeamSet*, BaseID>;