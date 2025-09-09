#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <format>
#include <type_traits>
#include <algorithm>
#include <random>
#include <ranges>
#include "macros/myMacros.h"
#include "threadSafeTools.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/btree_set.h"

extern const int N_PLAYERS;

using BaseRank = uint8_t;

using BaseID = uint8_t;

enum class Rank: BaseRank{
    Empty = 0,
    Iron,
    Bronze,
    Silver,
    Gold,
    Platinum,
    Diamond,
    Ascendant,
    Immortal,
    Radiant,
    COUNT
};

const std::unordered_map<std::string, Rank> STRING_RANK_TO_ENUM_MAP={
    {"Empty", Rank::Empty},
    {"Iron", Rank::Iron},
    {"Bronze", Rank::Bronze},
    {"Silver", Rank::Silver},
    {"Gold", Rank::Gold},
    {"Platinum", Rank::Platinum},
    {"Diamond", Rank::Diamond},
    {"Ascendant", Rank::Ascendant},
    {"Immortal", Rank::Immortal},
    {"Radiant", Rank::Radiant}
};

const std::unordered_map<Rank, std::string> ENUM_RANK_TO_STRING_MAP={
    {Rank::Empty, "Empty"},
    {Rank::Iron, "Iron"},
    {Rank::Bronze, "Bronze"},
    {Rank::Silver, "Silver"},
    {Rank::Gold, "Gold"},
    {Rank::Platinum, "Platinum"},
    {Rank::Diamond, "Diamond"},
    {Rank::Ascendant, "Ascendant"},
    {Rank::Immortal, "Immortal"},
    {Rank::Radiant, "Radiant"}
};

struct Player {
    std::string name;
    Rank rank;
    friend std::ostream& operator<<(std::ostream& o, const Player& p);
    operator std::string() const{
        return name+" ("+ENUM_RANK_TO_STRING_MAP.at(rank)+")";
    }
};

using CSVRows = std::vector<std::vector<std::string>>;


using IDRankPair = std::pair<BaseID, BaseRank>;

using PlayerBag = std::vector<std::vector<IDRankPair>>;

using ConfigMatrix = std::vector<IDRankPair>; // each player rapresented by a number paired with the value of their rank

using TeamsMatrix = std::vector<Player>;
using PlayerList = std::vector<Player>;

template<typename T>
using PseudoMatrix = std::vector<T>;

struct IDRankPairLess{
    bool operator()(const IDRankPair&, const IDRankPair&) const;
};

struct RankLess{
    bool operator()(const Rank&, const Rank&) const;
};

//using TeamSet = std::multiset<Rank, RankLess>;
using TeamSet = absl::btree_multiset<Rank>;


struct TeamSetLess{
    bool operator()(const TeamSet&, const TeamSet&) const;
    bool operator()(const TeamSet*&, const TeamSet*&) const;
};


//using ConfigSet = std::multiset<TeamSet, TeamSetLess>;
using ConfigSet = absl::btree_multiset<TeamSet>;


struct VisitedConfigSetHash{
    size_t operator()(const ConfigSet&) const;
};


using MoveGenConfigSet = absl::btree_set<std::pair<int, TeamSet>>;

//using VisitedConfigSet= std::unordered_set<ConfigSet, VisitedConfigSetHash>;
using VisitedConfigSet = absl::flat_hash_set<MoveGenConfigSet>;

CSVRows readCSV(const std::string& filename);

PlayerList formatRowsToPlayerList(const CSVRows& rows);

//Rank stringRankToEnum(std::string name);

ConfigMatrix teamsToConfigMatrix(const TeamsMatrix& teams_matrix, std::unordered_map<BaseID, Player>& player_dict);

TeamsMatrix configMatrixToTeams(const ConfigMatrix& config_matrix, const std::unordered_map<BaseID, Player>& player_dict);

//stream operators to make the printMatrix function usable for all the types it's intended for
std::ostream& operator<<(std::ostream& os, const IDRankPair& p);

template<typename T>
void printMatrix(const PseudoMatrix<T>& matrix, int rows, int cols);

template<typename T>
void shuffleMatrixArray(PseudoMatrix<T>& matrix);



ConfigSet configMatrixToSet(const ConfigMatrix& config_matrix, int rows, int cols);

ConfigMatrix configSetToMatrix(const ConfigSet& config_set, PlayerBag player_bag); // need to know Matrix dims beforehand

PlayerBag makePlayerBag(const ConfigMatrix& player_list);


float configMatrixQuality(const ConfigMatrix&, int n_teams, int n_players);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// hpp-like part


template<typename T>
void printMatrix(const PseudoMatrix<T>& matrix, int rows, int cols){
    auto range= std::ranges::views::iota; //just aliasing the function
    float min=-1;
    float max=-1;
    for(int i: range(0, rows)){
        std::string follow=", ";
        std::cout<<"[";
        int avg=0;
        for(int j: range(0, cols)){
            if(j==cols-1){
                follow.clear();
            }
            const T& elem=matrix[i*cols+j];
            if constexpr (std::is_convertible<T, std::string>::value){
                std::cout<<std::format("{:<30}", ((std::string)elem+follow));
                if constexpr (std::is_same<T, Player>::value){
                    avg+=static_cast<int>(elem.rank);
                }
            }else{
                std::ostringstream os;
                os<<elem;
                std::cout<<std::format("{:<30}", (os.str()+follow));
            }
        }
        std::cout<<"]";
        if constexpr (std::is_same<T, Player>::value){
            float team_val=static_cast<float>(avg)/cols;
            std::cout<<" "<<team_val;
            if(min==-1 and max==-1){
                min=max=team_val;
            }else{
                if(min>team_val){
                    min=team_val;
                }
                if(max<team_val){
                    max=team_val;
                }
            }
        }
        std::cout<<"\n";
    }
    if constexpr (std::is_same<T, Player>::value){
        std::cout<<"Gap between strongest and weakest team: "<<max-min<<"\n";
    }
}


template<typename T>
void shuffleMatrixArray(PseudoMatrix<T>& matrix){
    using RandomGenerator=std::mt19937; //ugly name, not choosing 19937 for a particular reason rn so better hide it
    std::random_device rnd_dv; //slow random gen -> personal note
    RandomGenerator generator(rnd_dv()); //fast random gen -> personal note
    std::shuffle(matrix.begin(), matrix.end(), generator);
}