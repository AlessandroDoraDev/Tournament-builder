#pragma once
#include "thread_safe/ThreadSafeUnorderedSet.h"
#include "ValuedConfigSet.h"
#include "hash/VisitedConfigSetHash.h"
#include "ConfigSet.h"

template<int n_threads=1>
struct MapForThreadMoles{
    ThreadSafeUnorderedSet<ConfigSet, VisitedConfigSetHash> visited_configs;
    std::array<ValuedConfigSet, n_threads> valued_sets_per_process;
    std::array<ValuedConfigSet, n_threads>& valued_sets_pp=valued_sets_per_process;
    MapForThreadMoles(){
        for(ValuedConfigSet& val_set: valued_sets_pp){
            val_set.quality=10.0;
        }
    }
};