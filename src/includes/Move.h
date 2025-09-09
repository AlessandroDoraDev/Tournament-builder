#pragma once
#include <utility>
#include "matricesModule.h"
#include "MovePieceNBaseRankPair.h"
#include "MovePieceNRankPair.h"
#include "MoveArrT.h"

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
