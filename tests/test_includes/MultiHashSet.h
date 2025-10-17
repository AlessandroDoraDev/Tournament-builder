#pragma once
#include "absl/container/flat_hash_map.h"
#include <string>
#include <functional>
#include <utility>
#include "iterators/MultiHashSetIterator.h"
#include "iterators/ConstMultiHashSetIterator.h"

template<typename T, typename NumerationType=size_t>
class MultiHashSet{
public:
    absl::flat_hash_map<T, NumerationType> m_map;
public:
    using ValueType= T;
    using SizeType= NumerationType;
    using Iterator= MultiHashSetIterator<MultiHashSet<T, NumerationType>>;
    using ConstIterator= ConstMultiHashSetIterator<MultiHashSet<T, NumerationType>>;
public:
    MultiHashSet(){
        m_total_size=0;
    }
    NumerationType multiset_insert(const T&);
    NumerationType multiset_insert(T&&);
    template<typename... Args>
    NumerationType multiset_emplace(Args&&...);
    NumerationType multiset_remove(const T&, int qty=1);
    Iterator multiset_begin();
    Iterator multiset_end();
    ConstIterator multiset_begin() const;
    ConstIterator multiset_end() const;
    ConstIterator multiset_cbegin() const;
    ConstIterator multiset_cend() const;
    friend bool operator==(const MultiHashSet& lhs, const MultiHashSet& rhs);
    template <typename H>
    friend H AbslHashValue(H h, const MultiHashSet& m);
    operator std::string() const;
    void set_to_string(std::function<std::string(const T&)>);
private:
    std::function<std::string(const T&)> m_to_string;
    NumerationType m_total_size;
};

template<typename T, typename NumerationType>
NumerationType MultiHashSet<T, NumerationType>::multiset_insert(T&& key){
    NumerationType res=1;
    if(m_map.contains(key)){
        res=++m_map[key];
        key.clear();
    }else{
        m_map.emplace(std::move(key), 1);
    }
    m_total_size++;
    return res;
}

template<typename T, typename NumerationType>
NumerationType MultiHashSet<T, NumerationType>::multiset_insert(const T& key){
    if(m_map.contains(key)){
        ++m_map[key];
    }else{
        m_map.emplace(key, 1);
    }
    m_total_size++;
    return m_map[key];
}


template<typename T, typename NumerationType>
template<typename... Args>
NumerationType MultiHashSet<T, NumerationType>::multiset_emplace(Args&&... args){
    T key= T(std::forward<Args>(args)...);
    NumerationType res=1;
    if(m_map.contains(key)){
        res=++m_map[key];
    }else{
        m_map.emplace(std::move(key), 1); //very stupid to emplace an already existing elem
    }
    m_total_size++;
    return res;
}


template<typename T, typename NumerationType>
NumerationType MultiHashSet<T, NumerationType>::multiset_remove(const T& key, int qty){
    NumerationType res=-1;
    if(!m_map.contains(key)){
        return res;
    }
    NumerationType& value=m_map[key];
    if(qty<value){
        value-=qty;
        m_total_size-=qty;
        res=value;
    }else{
        m_map.erase(key);
        m_total_size--;
    }
    return res;
}


template<typename T, typename NumerationType>
MultiHashSet<T, NumerationType>::Iterator MultiHashSet<T, NumerationType>::multiset_begin(){
    return MultiHashSet<T, NumerationType>::Iterator(m_map.begin(), m_map.end());
}

template<typename T, typename NumerationType>
MultiHashSet<T, NumerationType>::ConstIterator MultiHashSet<T, NumerationType>::multiset_begin() const{
    return MultiHashSet<T, NumerationType>::ConstIterator(m_map.begin(), m_map.end());
}

template<typename T, typename NumerationType>
MultiHashSet<T, NumerationType>::ConstIterator MultiHashSet<T, NumerationType>::multiset_cbegin() const{
    return MultiHashSet<T, NumerationType>::ConstIterator(m_map.cbegin(), m_map.cend());
}

template<typename T, typename NumerationType>
MultiHashSet<T, NumerationType>::Iterator MultiHashSet<T, NumerationType>::multiset_end(){
    return MultiHashSet<T, NumerationType>::Iterator(m_map.end(), m_map.end());
}

template<typename T, typename NumerationType>
MultiHashSet<T, NumerationType>::ConstIterator MultiHashSet<T, NumerationType>::multiset_end() const{
    return MultiHashSet<T, NumerationType>::ConstIterator(m_map.end(), m_map.end());
}

template<typename T, typename NumerationType>
MultiHashSet<T, NumerationType>::ConstIterator MultiHashSet<T, NumerationType>::multiset_cend() const{
    return MultiHashSet<T, NumerationType>::ConstIterator(m_map.cend(), m_map.cend());
}

template<typename T, typename NumerationType>
bool operator==(const MultiHashSet<T, NumerationType>& lhs, const MultiHashSet<T, NumerationType>& rhs){
    return lhs.m_map==rhs.m_map;
}

template<typename H, typename T, typename NumerationType>
H AbslHashValue(H h, const MultiHashSet<T, NumerationType>& m){
    using HashMapPtr_t= absl::flat_hash_map<T, NumerationType>*;
    HashMapPtr_t m_ptr= dynamic_cast<HashMapPtr_t>(&m);
    H::combine_unordered(std::move(h), m_ptr->begin(), m_ptr->end());
}

template<typename T, typename NumerationType>
void MultiHashSet<T, NumerationType>::set_to_string(std::function<std::string(const T&)> to_string){
    m_to_string=to_string;
}

template<typename T, typename NumerationType>
MultiHashSet<T, NumerationType>::operator std::string() const{

    std::function<std::string(const T&)> elem_to_string=
    m_to_string?
    m_to_string:
    [](const T& key){return (std::string)key;};

    return std::format("{{{}}}\nTotal size: {}\n",
    sequence_to_string(
        m_map.begin(),
        m_map.end(),
        [&elem_to_string](const std::pair<T, NumerationType>& entry){
            return std::format("{}: {}", elem_to_string(entry.first), static_cast<size_t>(entry.second));
        },
        ",\n"
    ), m_total_size);
}