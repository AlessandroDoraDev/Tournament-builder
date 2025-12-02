#include "strat.h"
#include "globals.h"
#include <cstddef>
#include <algorithm>
#include "to_strings.h"
#include <format>
#include <chrono>
#include <thread>
#include "absl/container/flat_hash_set.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/btree_set.h"
#include "MoveArrT.h"
#include <cstdlib>
#include <unordered_set>
#include <unordered_map>
#include "Config.h"
#include <numeric>
#include <vector>

#define R_INT(r) static_cast<int>(r)

using Triplet= absl::btree_multiset<std::size_t>;

struct TripletHash{
    size_t operator()(Triplet const& t) const noexcept {return absl::Hash<Triplet>{}(t);}
};
struct TripletPtrHash{
    size_t operator()(Triplet* const& t) const noexcept {return absl::Hash<Triplet>{}(*t);}
};

struct TripletEq{
    bool operator()(Triplet const& t1, Triplet const& t2) const noexcept {return t1==t2;}
};
struct TripletPtrEq{
    bool operator()(Triplet* const& t1, Triplet* const& t2) const noexcept {return *t1==*t2;}
};

using IndexesArray= absl::FixedArray<std::size_t>;
using RIndexesArray= absl::FixedArray<std::size_t>;
using SymbolsArray= absl::FixedArray<char>;
using RankMoveSumArray= absl::FixedArray<int>;
#ifdef DEBUG
    using TripletPtrSet= std::unordered_set<Triplet*, TripletPtrHash, TripletPtrEq>;
    using TripletSet= std::unordered_set<Triplet, TripletHash, TripletEq>;
    using RankMoveSumMap= std::unordered_map<std::size_t, int>;
    using PntsMap= std::unordered_map<std::size_t, int>;
#else
    using TripletPtrSet= absl::flat_hash_set<Triplet*>; 
    using TripletSet= absl::flat_hash_set<Triplet>;
    using RankMoveSumMap= absl::flat_hash_map<std::size_t, int>;
    using PntsMap= absl::flat_hash_map<std::size_t, int>;
#endif


void incrementSelectedIndexesByOne(IndexesArray& selected_indexes, std::size_t roof){
    bool carry=true;
    std::size_t i=0;
    std::size_t end=selected_indexes.size();
    while(carry&&i<end){
        std::size_t curr_roof;
        std::size_t curr_starter;
        if((i&1)==0){
            curr_roof=roof;
            selected_indexes[i]++;
            curr_starter=0;
        }else{
            curr_roof=-1; //0 -1, bc 0 has to get counted too
            selected_indexes[i]--;
            curr_starter=roof-1;
        }
        if(selected_indexes[i]==curr_roof){
            selected_indexes[i]=curr_starter;
            i++;
        }else{
            carry=false;
        }
    }
}

void incrementSelectedRIndexesByOne(RIndexesArray& selected_r_indexes, Config& config, IndexesArray& selected_indexes){
    std::size_t i=0;
    bool carry=true;
    while(carry&&i<selected_r_indexes.size()){
        //++
        Rank curr_rank=config[selected_indexes[i]][selected_r_indexes[i]];
        do{
            selected_r_indexes[i]++;
        }while(selected_r_indexes[i]<config.n_cols
        &&curr_rank==config[selected_indexes[i]][selected_r_indexes[i]]);
        //
        if(selected_r_indexes[i]==config.n_cols){
            selected_r_indexes[i]=0;
            i++;
        }else{
            carry=false;
        }
    }
}

bool deduceAndApplyOnceMove(Config& config, IndexesArray& selected_indexes, const double ecc){
    const std::size_t& rotation_size=selected_indexes.size();
    SymbolsArray team_status(rotation_size);
    std::transform(selected_indexes.begin(), selected_indexes.end(), team_status.begin(),
    [&config](std::size_t index){
        int pnts= config[index].getPnts();
        if(pnts<config.at_least_pnts_per_team){
            return '<';
        }else if(pnts>config.at_least_pnts_per_team+1){
            return '>';
        }else{
            return '=';
        }
    });
    bool all_symbols_eq=std::all_of(team_status.begin(), team_status.end(), 
        [&team_status](char s){return s==team_status[0];}
    );

    if(all_symbols_eq){
        return false;
    }
    RankMoveSumArray orig_sum_array(rotation_size);
    RankMoveSumMap orig_sum_map;
    for(std::size_t i=0; i<rotation_size; i++){
        orig_sum_array[i]= config[selected_indexes[i]].getPnts()-config.at_least_pnts_per_team;
    }
    for(std::size_t i=0; i<rotation_size; i++){
        orig_sum_map[selected_indexes[i]]=orig_sum_array[i];
    }
    int orig_sum=std::accumulate(orig_sum_map.begin(), orig_sum_map.end(), 0,
    [](int acc, std::pair<std::size_t, int> dist){ 
        int ecc=dist.second>0?1:0;
        return acc+std::abs(dist.second-ecc);
    });
    
    RIndexesArray selected_r_indexes(rotation_size);
    std::fill(selected_r_indexes.begin(), selected_r_indexes.end(), 0);

    MoveArrT best_move(rotation_size);
    int best_pnts=orig_sum;

    auto is_end=[&config](std::size_t e){return e==0;}; //eventually iteration will reset to all zeros, so we can exit on all zeros since we don't need to check it.
    PntsMap orig_pnts;
    for(int i=0; i<rotation_size; i++){
        orig_pnts[selected_indexes[i]]=config[selected_indexes[i]].getPnts();
    }
    do{
        RankMoveSumMap sum_map=orig_sum_map;
        PntsMap pnts=orig_pnts;

        for(int i=0; i<rotation_size; i++){
            int next_i= i+1;
            if(next_i==rotation_size){
                next_i=0;
            }
            int curr_movement=R_INT(config[selected_indexes[i]][selected_r_indexes[i]]);
            sum_map[selected_indexes[i]]=sum_map[selected_indexes[i]]-curr_movement;
            sum_map[selected_indexes[next_i]]=sum_map[selected_indexes[next_i]]+curr_movement;
            pnts[selected_indexes[i]]-=curr_movement;
            pnts[selected_indexes[next_i]]+=curr_movement;
        }
        for(auto& pair: sum_map){
            int& sum=pair.second;
            if(sum>0){
                sum--;
            }
        }
        int move_pnts= std::accumulate(sum_map.begin(), sum_map.end(), 0,
            [](int acc, std::pair<std::size_t, int> dist){ return acc+std::abs(dist.second);});
        if(move_pnts<best_pnts){
            for(int i=0; i<rotation_size; i++){
                best_move[i]={selected_indexes[i], config[selected_indexes[i]][selected_r_indexes[i]]};
            }
            best_pnts=move_pnts;
        }
        incrementSelectedRIndexesByOne(selected_r_indexes, config, selected_indexes);
    }while(!std::all_of(selected_r_indexes.begin(), selected_r_indexes.end(), is_end));
    
    if(best_pnts==orig_sum){
        return false;
    }
    config.applyMove(best_move);
    return true;

}


void deduceAndApplyMoves(Config& config, std::size_t rotation_size){
    #define SELECTED_INDEXES_RANGE selected_indexes.begin(), selected_indexes.end()
    struct IndexesArrayAndTriplet{
        IndexesArray selected_indx;
        Triplet triplet;
    };
    std::vector<IndexesArrayAndTriplet> indexes_config_sequence;
    IndexesArray selected_indexes(rotation_size);
    
    const std::size_t& n= config.n_rows;
    const std::size_t r= selected_indexes.size();
    indexes_config_sequence.reserve(
        std::pow(n+r-1, r-1)
    );
    const std::size_t roof= config.n_rows;
    const double ecc=config.avg_rank-static_cast<int>(config.avg_rank) > 0?1:0;
    auto equalToRoofFunc=[roof](std::size_t index){return index==roof-1||index==-1;};
    auto allSelectedIndexesEqualFunc=[&selected_indexes](std::size_t i){return i==selected_indexes[0];};
    
    for(std::size_t i=0; i<selected_indexes.size(); i++){
        if((i&1)==0){
            selected_indexes[i]=0;
        }else{
            selected_indexes[i]=roof-1;
        }
    }
    std::size_t i=0;
    TripletSet explored_triplets;
    while(!std::all_of(selected_indexes.begin(), selected_indexes.end(), equalToRoofFunc)){
        Triplet t;
        t.insert(selected_indexes.begin(), selected_indexes.end());
        bool all_indexes_equal=std::all_of(
            selected_indexes.begin(), 
            selected_indexes.end(), 
            allSelectedIndexesEqualFunc);
        if(!explored_triplets.contains(t) && !all_indexes_equal){
            explored_triplets.emplace(t);
            indexes_config_sequence.emplace_back(selected_indexes, t);
        }
        incrementSelectedIndexesByOne(selected_indexes, roof);
    }
    TripletPtrSet explored_triplet_ptrs;
    std::vector<IndexesArrayAndTriplet>::iterator it=indexes_config_sequence.begin();
    std::vector<IndexesArrayAndTriplet>::iterator end=indexes_config_sequence.end();
    while(it!=end){
        if(!explored_triplet_ptrs.contains(&it->triplet)){
            explored_triplet_ptrs.emplace(&it->triplet);
            bool moveSucceded=deduceAndApplyOnceMove(config, it->selected_indx, ecc);
            if(moveSucceded){
                it=indexes_config_sequence.begin();
                explored_triplet_ptrs.clear();
                explored_triplet_ptrs.emplace(&it->triplet);
            }
        }
        ++it;
    }
}