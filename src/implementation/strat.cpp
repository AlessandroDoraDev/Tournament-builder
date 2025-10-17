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
#include "Move.h"
#include <cstdlib>
#include <unordered_set>
#include <unordered_map>

#define R_INT(r) static_cast<int>(r)

using Triplet= absl::btree_multiset<std::size_t>;

struct TripletHash{
    size_t operator()(Triplet const& t) const noexcept {return absl::Hash<Triplet>{}(t);}
};

struct TripletEq{
    bool operator()(Triplet const& t1, Triplet const& t2) const noexcept {return t1==t2;}
};

//using TripletSet= absl::flat_hash_set<Triplet>;
using TripletSet= std::unordered_set<Triplet, TripletHash, TripletEq>;
using IndexesArray= std::array<std::size_t, ROTATION_SIZE>;
using SymbolsArray= std::array<char, ROTATION_SIZE>;
using RankMoveSumArray= std::array<int, ROTATION_SIZE>;
// using RankMoveSumMap= absl::flat_hash_map<std::size_t, int>;
using RankMoveSumMap= std::unordered_map<std::size_t, int>;
using PntsMap= std::unordered_map<std::size_t, int>;

// struct AllignedTeamsData{
//     std::size index;  
// };

void incrementSelectedIndexesByOne(IndexesArray& selected_indexes, std::size_t roof){
    bool carry=true;
    std::size_t i=0;
    std::size_t end=selected_indexes.size();
    while(carry&&i<end){
        selected_indexes[i]++;
        if(selected_indexes[i]==roof){
            selected_indexes[i]=0;
            i++;
        }else{
            carry=false;
        }
    }
}

void incrementSelectedRIndexesByOne(std::array<std::size_t, ROTATION_SIZE>& selected_r_indexes, Config& config, IndexesArray& selected_indexes){
    std::size_t i=0;
    bool carry=true;
    while(carry&&i<ROTATION_SIZE){
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

bool deduceAndApplyOnceMove(Config& config, IndexesArray& selected_indexes){
    double ecc=config.avg_rank-static_cast<int>(config.avg_rank) > 0?1:0;

    SymbolsArray team_status;
    std::array<std::size_t, ROTATION_SIZE> dst_array;
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
        [&team_status](char s){return s==team_status[0];});
    if(all_symbols_eq){
        return false;
    }
    RankMoveSumArray orig_sum_array;
    RankMoveSumMap orig_sum_map;
    for(int i=0; i<ROTATION_SIZE; i++){
        // if(team_status[i]=='>'){
        //     orig_sum_array[i]= config[selected_indexes[i]].getPnts()-(config.at_least_pnts_per_team+ecc);
        // }else{
        //     orig_sum_array[i]= config[selected_indexes[i]].getPnts()-config.at_least_pnts_per_team; 
        // }
        orig_sum_array[i]= config[selected_indexes[i]].getPnts()-config.at_least_pnts_per_team;
    }
    for(int i=0; i<ROTATION_SIZE; i++){
        orig_sum_map[selected_indexes[i]]=orig_sum_array[i];
    }
    int orig_sum=std::accumulate(orig_sum_map.begin(), orig_sum_map.end(), 0,
    [](int acc, std::pair<std::size_t, int> dist){ 
        int ecc=dist.second>0?1:0;
        return acc+std::abs(dist.second-ecc);
    });
    
    std::array<std::size_t, ROTATION_SIZE> selected_r_indexes;
    std::fill(selected_r_indexes.begin(), selected_r_indexes.end(), 0);

    MoveArrT<ROTATION_SIZE> best_move;
    int best_pnts=orig_sum;

    auto is_end=[&config](std::size_t e){return e==0;};
    PntsMap orig_pnts;
    for(int i=0; i<ROTATION_SIZE; i++){
        orig_pnts[selected_indexes[i]]=config[selected_indexes[i]].getPnts();
    }
    do{
        RankMoveSumMap sum_map=orig_sum_map;
        PntsMap pnts=orig_pnts;

        for(int i=0; i<ROTATION_SIZE; i++){
            int next_i= i+1;
            if(next_i==ROTATION_SIZE){
                next_i=0;
            }
            int curr_movement=R_INT(config[selected_indexes[i]][selected_r_indexes[i]]);
            sum_map[selected_indexes[i]]=sum_map[selected_indexes[i]]-curr_movement;
            sum_map[selected_indexes[next_i]]=sum_map[selected_indexes[next_i]]+curr_movement;
            pnts[selected_indexes[i]]-=curr_movement;
            pnts[selected_indexes[next_i]]+=curr_movement;
        }
        auto within= [](int a, int li, int ls){return a>=li&&a<=ls;};
        for(auto& pair: sum_map){
            // if(within(
            //     pnts[selected_indexes[i]]-sum_map[selected_indexes[i]], 
            //     config.at_least_pnts_per_team, 
            //     config.at_least_pnts_per_team+ecc)){
            //     sum_map[selected_indexes[i]]=0;
            // }
            int& sum=pair.second;
            if(sum>0){
                sum--;
            }
        }
        int move_pnts= std::accumulate(sum_map.begin(), sum_map.end(), 0,
            [](int acc, std::pair<std::size_t, int> dist){ return acc+std::abs(dist.second);});
        if(move_pnts<best_pnts){
            for(int i=0; i<ROTATION_SIZE; i++){
                best_move[i]={selected_indexes[i], config[selected_indexes[i]][selected_r_indexes[i]]};
            }
            best_pnts=move_pnts;
        }
        incrementSelectedRIndexesByOne(selected_r_indexes, config, selected_indexes);
    }while(!all_of(selected_r_indexes.begin(), selected_r_indexes.end(), is_end));
    
    if(best_pnts==orig_sum){
        return false;
    }
    config.applyMove(best_move);
    return true;

}

void deduceAndApplyMoves(Config& config){
    #define SELECTED_INDEXES_RANGE selected_indexes.begin(), selected_indexes.end()
    IndexesArray selected_indexes;
    const std::size_t roof= config.n_rows;
    auto equalToRoofFunc=[roof](std::size_t index){return index==roof-1;};
    auto allSelectedIndexesEqualFunc=[&selected_indexes](std::size_t i){return i==selected_indexes[0];};

    std::fill(SELECTED_INDEXES_RANGE, 0);
    TripletSet explored_triplets;
    // Triplet td;
    // std::array<std::size_t, ROTATION_SIZE> mv={0, 0, 10};
    // td.insert(mv.begin(), mv.end());
    while(!std::all_of(SELECTED_INDEXES_RANGE, equalToRoofFunc)){
        Triplet t;
        t.insert(selected_indexes.begin(), selected_indexes.end());
        // if(t==td){
        //     int i=0;
        // }
        bool all_indexes_equal=std::all_of(SELECTED_INDEXES_RANGE, allSelectedIndexesEqualFunc);
        if(!explored_triplets.contains(t) && !all_indexes_equal){
            explored_triplets.emplace(t);
            bool moveSucceded=deduceAndApplyOnceMove(config, selected_indexes);
            if(moveSucceded){
                std::fill(SELECTED_INDEXES_RANGE, 0);
                explored_triplets.clear();
                Triplet t;        
                t.insert(selected_indexes.begin(), selected_indexes.end());
                explored_triplets.emplace(t);
            }
        }
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        incrementSelectedIndexesByOne(selected_indexes, roof);
    }
}