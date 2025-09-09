#pragma once
#include "matricesModule.h"
#include "threadSafeTools.h"
#include <map>
#include <initializer_list>
#include <algorithm>
#include <array>
#include <format>
#include "macros/myMacros.h"
#include "absl/container/flat_hash_set.h"

struct ValuedConfigSet{
    ConfigSet set;
    float quality;
};

using MarkedTConfigSet= std::vector<const TeamSet*>;

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


// def truncate_dict(d:dict, max_items=3):

// def find_optimal_team_config_rotation(orig_config_set: frozenset, rotation_size: int, visited_configs=None, pid: int= 0)-> tuple[frozenset, float]:

// def apply_rotation_to_config_set(config_set: frozenset, rotation: tuple, rotation_size: int)-> frozenset:

// def config_set_quality(config_set: frozenset)-> float:

// def find_optimal_team_config_recursive(orig_config_set: frozenset, orig_config_quality: float, rotation_size: int, visited_configs: ThreadSafeList, possible_moves:frozenset, best_quality:int, pid: int= 0)-> tuple[frozenset, float]:
using MovePieceNRankPair=std::pair<uint8_t, Rank>;
using MovePieceNBaseRankPair=std::pair<uint8_t, BaseRank>;

template<int rotation_size=2>
using MoveArrT= std::array<MovePieceNBaseRankPair, rotation_size>;


template<int rotation_size=2>
class Move{
public:
    Move(){}
    explicit Move(MoveArrT<rotation_size>&& moves): m_move(std::move(moves)){}
    Move(std::initializer_list<MovePieceNBaseRankPair> moves): m_move(std::move(moves)){}
    Move(std::initializer_list<MovePieceNRankPair> moves){
        int i=0;
        for(const MovePieceNRankPair& r: moves){
            m_move[i]={r.first, static_cast<BaseRank>(r.second)};
            i++;
        }
    }
    ConfigSet operator()(const ConfigSet&) const;
    
    template <typename H>
    friend H AbslHashValue(H h, const Move<rotation_size>& m) {
        return H::combine(std::move(h), m.m_move);
    }

    friend bool operator==(const Move<rotation_size>& m1, const Move<rotation_size>& m2) {
        return m1.m_move == m2.m_move;
    }


    operator std::string() const{
        std::string out="(";
        for (const MovePieceNBaseRankPair& move_piece : m_move) {
            out += std::format("({}, {})", move_piece.first, move_piece.second);
        }
        return out+")";
    }

    bool less(const Move<rotation_size>& m) const;
    //size_t hash(const Move<rotation_size>& m) const;
public:
    MoveArrT<rotation_size> m_move;
};


template<int rotation_size=2>
struct MovesSetLess{
    bool operator()(const Move<rotation_size>& m1, const Move<rotation_size>& m2) const;
};

template<int rotation_size=2>
using MovesSet = absl::flat_hash_set<Move<rotation_size>>;


template<int n_threads=1>
ConfigSet findOptimalTeamConfigViaRotationsMultiThreaded(ConfigMatrix& orig_config_matrix, int rotation_size, int n_teams);

template<int n_threads=1>
void findOptimalTeamConfigViaRotation(const ConfigSet& orig_config_set, const ConfigMatrix& orig_config_matrix, int rotation_size, MapForThreadMoles<n_threads>map_for_moles, int pid);


using TeamMap= std::map<Rank, int>;

struct TeamForMoveGen{
    int mark;
    const TeamSet* team_set_ptr; 
    TeamMap team_map;
};

struct TeamForMoveGenLess{
    bool operator()(TeamForMoveGen*& g1, TeamForMoveGen*& g2){
        return TeamSetLess{}(g1->team_set_ptr, g2->team_set_ptr);
    }
};

using ConfigForMoveGen= std::vector<TeamForMoveGen>;

template<int rotation_size=2>
FORCE_INLINE void updateBannedMoves(const std::array<ConfigForMoveGen::iterator, rotation_size>& teams_indexes, MovesSet<rotation_size>& banned_moves, VisitedConfigSet& visited_teams_groups);

template<int rotation_size=2>
void generateMoves(const MarkedTConfigSet&, MovesSet<rotation_size>&, const Move<rotation_size>&);

template<int rotation_size=2>
void generateMoves(const MarkedTConfigSet&, MovesSet<rotation_size>&);

template<int rotation_size=2>
void generateMoves(const std::array<ConfigForMoveGen::iterator, rotation_size>&, MovesSet<rotation_size>&, const ConfigForMoveGen&, MovesSet<rotation_size>&, VisitedConfigSet&);

// template<int rotation_size=2>
// void generateMovesFromGeneratedConfig(const MarkedTConfigSet&, MovesSet<rotation_size>&, const Move<rotation_size>&);


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// hpp part

template<int rotation_size>
bool Move<rotation_size>::less(const Move<rotation_size>& m) const{

    return std::lexicographical_compare(
        this->m_move.begin(), this->m_move.end(), 
        m.m_move.begin(), m.m_move.end(),
        [](const MovePieceNBaseRankPair& p1, const MovePieceNBaseRankPair& p2){
            std::string s1, s2;
            s1.push_back(p1.first);
            s1.push_back(p1.second);
            s2.push_back(p2.first);
            s2.push_back(p2.second);
            return s1<s2;
        });
}

template<int rotation_size>
bool MovesSetLess<rotation_size>::operator()(const Move<rotation_size>& m1, const Move<rotation_size>& m2) const{
    return m1.less(m2);
}

template<int n_threads>
ConfigSet findOptimalTeamConfigViaRotationsMultiThreaded(ConfigMatrix& orig_config_matrix, int rotation_size, int n_teams){
    auto range= std::ranges::views::iota;
    
    MapForThreadMoles<n_threads> map_for_moles;
    std::vector<std::thread> workers;
    workers.reserve(n_threads);
    for(int pid: range(0, n_threads)){
        shuffleMatrixArray(orig_config_matrix);
        ConfigSet orig_config_set= configMatrixToSet(orig_config_matrix, n_teams, N_PLAYERS);
        float orig_config_quality= configMatrixQuality(orig_config_matrix, n_teams, N_PLAYERS);
        map_for_moles.valued_sets_pp[pid].set=orig_config_set;
        map_for_moles.valued_sets_pp[pid].quality=orig_config_quality;
        workers.emplace_back(findOptimalTeamConfigViaRotation, orig_config_set, rotation_size, map_for_moles, pid);
    }
    for(const std::thread& worker: workers){
        worker.join();
    }
    return *std::min_element(
        map_for_moles.valued_sets_pp.begin(),
        map_for_moles.valued_sets_pp.end(),
        [](const ValuedConfigSet& set1, const ValuedConfigSet& set2){
            return set1.quality<set2.quality;
        }
    );
}

template<int n_threads>
void findOptimalTeamConfigViaRotation(const ConfigSet& orig_config_set, const ConfigMatrix& orig_config_matrix, int rotation_size, MapForThreadMoles<n_threads>map_for_moles, int pid=0){
    
    
    /*
    #determina tutte le possibili mosse
    possible_moves=frozenset(permutations(config_set_to_matrix(orig_config_set), rotation_size))
    #determina tutte le mosse inutili tra giocatori dello stesso rank
    same_rank_moves=set()
    config_organized_by_rank = {rank.value: [] for rank in Rank}
    for team in orig_config_set:
        for player in team:
            config_organized_by_rank[player[1]].append(player)
    for rank in config_organized_by_rank.keys():
        players_of_said_rank=config_organized_by_rank[rank]
        same_rank_moves.update(set(permutations(players_of_said_rank, rotation_size)))
    same_rank_moves=frozenset(same_rank_moves)
    possible_moves = possible_moves - same_rank_moves
    orig_config_quality=None
    if visited_configs is None:
        orig_config_quality = config_set_quality(orig_config_set)
        visited_configs=ThreadSafeList(ThreadSafeSet(), [[orig_config_set, orig_config_quality]]) #non importante che i set migliori siano in una threadsafe list, perché tanto tutti i thread scrivono in posizioni diverse
    else:
        orig_config_quality = visited_configs[1][pid][1]
    with print_lock:
        print(f'Process {pid}-> Best quality by far: {round(orig_config_quality, 2)}')
    res=find_optimal_team_config_recursive(orig_config_set, visited_configs[1][pid][0], rotation_size, visited_configs, possible_moves, best_quality=10, pid=pid)
    return res
    */
}

template<int rotation_size>
void generateMoves(
    const MarkedTConfigSet& config_set, 
    MovesSet<rotation_size>& moves_set, 
    const Move<rotation_size>& gen_move
        ){
    
    PROFILE_FUNCTION();
    if(moves_set.empty()){
        MovesSet<rotation_size>& empty_moves_set=moves_set;
        generateMoves(config_set, empty_moves_set);
    }else{
        // generateMovesFromGeneratedConfig(config_set, moves_set, gen_move);
    }
}

template<int rotation_size>
void generateMoves(const MarkedTConfigSet& config_set, MovesSet<rotation_size>& moves_set){
    
    PROFILE_FUNCTION();
    
    MovesSet<rotation_size> banned_moves;
    VisitedConfigSet visited_teams_groups;
    ConfigForMoveGen teams_list_p;
    teams_list_p.reserve(config_set.size());
    int i=0;
    for(const TeamSet* team_set: config_set){
        TeamMap team_mapped;
        for(const Rank& rank: *team_set){
            ++team_mapped[rank];
        }
        teams_list_p.emplace_back(i, team_set, std::move(team_mapped));
        i++;
    }
    std::array<ConfigForMoveGen::iterator, rotation_size> indexes;
    std::fill(indexes.begin(), indexes.end(), teams_list_p.begin());
    //basically n-dimensional for loop
    while(indexes[0]!=teams_list_p.end()){
        bool completely_same_team_rotation=
            std::all_of(indexes.begin(), indexes.end(), [&indexes](const ConfigForMoveGen::iterator& x){return x==indexes[0];});

        if(!completely_same_team_rotation){
            generateMoves(indexes, moves_set, teams_list_p, banned_moves, visited_teams_groups);
        }
        //incrementing similar to count
        int i=1;
        ++indexes[rotation_size-i];
        while(rotation_size-i!=0 && indexes[rotation_size-i]==teams_list_p.end()){
            indexes[rotation_size-i]=teams_list_p.begin();
            i++;
            ++indexes[rotation_size-i];
        }
    }
}


template<int rotation_size>
FORCE_INLINE void updateBannedMoves(
    const std::array<ConfigForMoveGen::iterator, rotation_size>& teams_indexes,
    MovesSet<rotation_size>& banned_moves, 
    VisitedConfigSet& visited_teams_groups){
    // auto range= std::ranges::views::iota;
    // Se c'è un elemento che non corrisponde per team allora fa parte della
    // mossa irrilevante. Gli elementi non corrispondenti sono SEMPRE sulla stessa
    // colonna se e solo se il resto degli elementi è condiviso e le liste sono
    // ordinate
    using ActualTeamSetIterator = decltype(teams_indexes[0]->team_set_ptr->begin());
    using MarkTeamSetItPair= std::pair<int, ActualTeamSetIterator>;
    using ColIterators= std::array<MarkTeamSetItPair, rotation_size>;
    MoveGenConfigSet team_group;
    ColIterators col_iterators;
    int i=0;
    for(const ConfigForMoveGen::iterator& team_ptr: teams_indexes){
        col_iterators[i].first=team_ptr->mark;
        col_iterators[i].second=team_ptr->team_set_ptr->begin();
        team_group.emplace(team_ptr->mark, *team_ptr->team_set_ptr);
        i++;
    }
    //codice sottostante commentato perché la logica dei visited teams non conta 34 mosse bannabili
    //secondo il resto dell'algoritmo.
    //static int roba = 0;
    ////dovrebbe controllare che la mossa non generi lo stesso
    //if(visited_teams_groups.contains(team_group)){
    //    std::cout << roba << "\n";
    //    roba++;
    //    return;
    //}
    //else {
    //    visited_teams_groups.insert(team_group);
    //}
    int mismatch_cnt=0;
    MoveArrT<rotation_size> move_arr;
    using ActualTeamSetIterator = decltype(teams_indexes[0]->team_set_ptr->end());
    ActualTeamSetIterator end=teams_indexes[0]->team_set_ptr->end(); //from first of teams take end;
    while(col_iterators[0].second!=end){

        bool all_match=std::all_of(col_iterators.begin(), col_iterators.end(), 
        [&col_iterators](const MarkTeamSetItPair& it){
            return *it.second==*col_iterators[0].second;
        });

        if(!all_match){
            mismatch_cnt++;
            if(mismatch_cnt==1){
                i=0;
                for(MarkTeamSetItPair& col_mark_it_pair: col_iterators){
                    move_arr[i].first= col_mark_it_pair.first;
                    move_arr[i].second= static_cast<BaseRank>(*col_mark_it_pair.second);
                    i++;
                }
            }
        }

        for(MarkTeamSetItPair& col_mark_it_pair: col_iterators){
            ++col_mark_it_pair.second;
        }
    }
    if(mismatch_cnt==1){
        MoveArrT<rotation_size> inverse_move_arr;
        std::copy(move_arr.rbegin(), move_arr.rend(), inverse_move_arr.begin());
        banned_moves.emplace(std::move(move_arr));
        banned_moves.emplace(std::move(inverse_move_arr));
    }

}

template<int rotation_size>
void generateMoves(
    const std::array<ConfigForMoveGen::iterator, rotation_size>& teams_indexes, 
    MovesSet<rotation_size>& moves_set, 
    const ConfigForMoveGen& teams_list_p, 
    MovesSet<rotation_size>& banned_moves, 
    VisitedConfigSet& visited_teams_groups){
    
    using ActualTeamSetIterator = decltype(teams_indexes[0]->team_set_ptr->begin());
    using TeamItTeamMovGenPair= std::pair<ActualTeamSetIterator, const TeamForMoveGen*>;
    
    updateBannedMoves(teams_indexes, banned_moves, visited_teams_groups);
    std::array<TeamItTeamMovGenPair, rotation_size> rank_indexes;
    std::transform(teams_indexes.begin(), teams_indexes.end(), rank_indexes.begin(),
        [](const ConfigForMoveGen::iterator& it) {
            return std::make_pair(it->team_set_ptr->begin(), &*it);
        });
    //basically n-dimensional for loop
    ActualTeamSetIterator end = rank_indexes[0].second->team_set_ptr->end();
    ActualTeamSetIterator& begin = rank_indexes[0].first;
    while (begin != end) {
        bool completely_same_rank_rotation=
            std::all_of(rank_indexes.begin(), rank_indexes.end(), [&rank_indexes](const TeamItTeamMovGenPair& x){return *x.first==*rank_indexes[0].first;});
        if(!completely_same_rank_rotation){
            /*
            //da escludere tutte le mosse che mi restituirebbero lo stesso set
            //di team.
            //L'unico caso in cui questo avviene è il caso in cui la mia mossa
            //influenza 1 elemento diverso tra tutti i team coinvolti che per
            //il resto degli elementi sono identici.
            //rotation_size lo considero come se non fosse mai più grande del
            //numero di team, poiché si sfocerebbe in logiche di rotazione
            //multidimensionali che impatterebbero in performance il programma
            //e che quindi non ha senso supportare in nessun caso.
            
            //Individuo se i team sono identici fino a len-1(basta comparison lexico)
            //e se gli elementi finali (quelli diversi) sono quelli della mossa
            //In sostanza è un calcolo solo, fatto una volta e che esclude
            //una mossa sola per ogni gruppo di team selezionato
            */
            Move<rotation_size> actual_move;
            std::transform(rank_indexes.begin(), rank_indexes.end(), actual_move.m_move.begin(), 
                [](TeamItTeamMovGenPair& pair){
                    return std::make_pair(pair.second->mark, static_cast<BaseRank>(*pair.first));
                });
            if(!banned_moves.contains(actual_move)){
                moves_set.emplace(std::move(actual_move));
            }
        }

        //incrementing similar to count
        int i=1;
        ++rank_indexes[rotation_size-i].first;
        while(rotation_size-i!=0 
                && rank_indexes[rotation_size-i].first==rank_indexes[rotation_size-i].second->team_set_ptr->end()){
            rank_indexes[rotation_size-i].first=rank_indexes[rotation_size-i].second->team_set_ptr->begin();
            i++;
            ++rank_indexes[rotation_size-i].first;
        }
    }
}


template<int rotation_size>
ConfigSet Move<rotation_size>::operator()(const ConfigSet&) const{

}