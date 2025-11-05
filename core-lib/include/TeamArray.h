#pragma once
#include <span>
#include "Rank.h"
#include <cstddef>
#include <numeric>

//using TeamArray= std::span<Rank>;

class TeamArray{
public:
    using ValueType= Rank;
    using iterator= std::span<Rank>::iterator;
public:
    friend class Config;
public:
    template<typename IteratorType>
    TeamArray(IteratorType begin, IteratorType end): m_span(begin, end){
        m_pnts=std::accumulate(
            m_span.begin(),
            m_span.end(),
            0,
            [](int acc, const Rank& r){return acc+static_cast<int>(r);}
        );
    }
    TeamArray(std::span<Rank> span): m_span(std::move(span)){
        m_pnts=std::accumulate(
            m_span.begin(),
            m_span.end(),
            0,
            [](int acc, const Rank& r){return acc+static_cast<int>(r);}
        );
    } //non explicit on purpose
    const Rank& operator[](std::size_t) const;
    std::span<Rank>::iterator begin() const;
    std::span<Rank>::iterator end() const;
    void replaceAt(Rank org_rank, Rank dst_rank);
    int getPnts();

    void sort();
private:
    std::span<Rank> m_span;
    int m_pnts;
};