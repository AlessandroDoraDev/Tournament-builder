#pragma once
#include "ConfigSet.h"

struct VisitedConfigSetHash{
    size_t operator()(const ConfigSet&) const;
};
