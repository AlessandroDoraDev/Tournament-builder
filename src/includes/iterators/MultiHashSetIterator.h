#pragma once
#include "absl/container/flat_hash_map.h"

template<typename T_MultiHashSet>
class ConstMultiHashSetIterator;

template<typename T_MultiHashSet>
class MultiHashSetIterator{
public:
    using ValueType= T_MultiHashSet::ValueType;
    using NumerationType= T_MultiHashSet::SizeType;
    using ValuePointer= ValueType*;
    using ConstValuePointer= const ValueType*;
    using ValueReference= ValueType&;
    using ConstValueReference= const ValueType&; 
    using AbslIterator= absl::flat_hash_map<ValueType, NumerationType>::iterator;
    using AbslConstIterator= absl::flat_hash_map<ValueType, NumerationType>::const_iterator;
    friend class ConstMultiHashSetIterator<T_MultiHashSet>;
public:
    MultiHashSetIterator(const AbslIterator&, const AbslIterator&); //always counting as if it was the end iterator
    MultiHashSetIterator& operator++();
    MultiHashSetIterator& operator++(int);
    ValuePointer operator->();
    ConstValueReference operator*();
    bool operator==(const MultiHashSetIterator&) const;
    bool operator!=(const MultiHashSetIterator&) const;
private:
    AbslIterator m_it;
    AbslIterator m_end;
    NumerationType m_current_elem_size;
};

template<typename T_MultiHashSet>
MultiHashSetIterator<T_MultiHashSet>::
MultiHashSetIterator(const AbslIterator& it, const AbslIterator& end)
: m_it(it), m_end(end){
    if(it==end){
        m_current_elem_size=-1;
    }else{
        m_current_elem_size=m_it->second;
    }
}

template<typename T_MultiHashSet>
MultiHashSetIterator<T_MultiHashSet>& MultiHashSetIterator<T_MultiHashSet>::
operator++(){
    m_current_elem_size--;
    if(m_current_elem_size==0){
        ++m_it;
        if(m_it!=m_end){
            m_current_elem_size=m_it->second;
        }else{
            m_current_elem_size=-1;
        }
    }
    return *this;
}

template<typename T_MultiHashSet>
MultiHashSetIterator<T_MultiHashSet>& MultiHashSetIterator<T_MultiHashSet>::
operator++(int){
    MultiHashSetIterator<T_MultiHashSet> old_copy= *this;
    m_current_elem_size--;
    if(m_current_elem_size==0){
        ++m_it;
        if(m_it!=m_end){
            m_current_elem_size=m_it->second;
        }else{
            m_current_elem_size=-1;
        }
    }
    return old_copy;
}

template<typename T_MultiHashSet>
MultiHashSetIterator<T_MultiHashSet>::ValuePointer MultiHashSetIterator<T_MultiHashSet>::
operator->(){
    return &m_it->first;
}

template<typename T_MultiHashSet>
MultiHashSetIterator<T_MultiHashSet>::ConstValueReference MultiHashSetIterator<T_MultiHashSet>::
operator*(){
    return m_it->first;
}

template<typename T_MultiHashSet>
bool MultiHashSetIterator<T_MultiHashSet>::
operator==(const MultiHashSetIterator& other) const{
    return this->m_it==other.m_it;
}

template<typename T_MultiHashSet>
bool MultiHashSetIterator<T_MultiHashSet>::
operator!=(const MultiHashSetIterator& other) const{
    return !(*this==other);
}