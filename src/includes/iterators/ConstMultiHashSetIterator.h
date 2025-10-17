#pragma once
#include "absl/container/flat_hash_map.h"
#include "iterators/MultiHashSetIterator.h"

template<typename T_MultiHashSet>
class ConstMultiHashSetIterator{
public:
    using ValueType= T_MultiHashSet::ValueType;
    using NumerationType= T_MultiHashSet::SizeType;
    using ValuePointer= ValueType*;
    using ConstValuePointer= const ValueType*;
    using ValueReference= ValueType&;
    using ConstValueReference= const ValueType&; 
    using AbslIterator= absl::flat_hash_map<ValueType, NumerationType>::iterator;
    using AbslConstIterator= absl::flat_hash_map<ValueType, NumerationType>::const_iterator;
public:
    ConstMultiHashSetIterator(const AbslConstIterator&, const AbslConstIterator&); //always counting as if it was the end iterator
    ConstMultiHashSetIterator(const MultiHashSetIterator<T_MultiHashSet>&); //always counting as if it was the begin or end iterator
    ConstMultiHashSetIterator& operator++();
    ConstMultiHashSetIterator& operator++(int);
    ConstValuePointer operator->() const;
    ConstValueReference operator*() const;
    bool operator==(const ConstMultiHashSetIterator&) const;
    bool operator!=(const ConstMultiHashSetIterator&) const;
private:
    AbslConstIterator m_it;
    AbslConstIterator m_end;
    NumerationType m_current_elem_size;
};

template<typename T_MultiHashSet>
ConstMultiHashSetIterator<T_MultiHashSet>::
ConstMultiHashSetIterator(const AbslConstIterator& it, const AbslConstIterator& end)
: m_it(it), m_end(end){
    if(it==end){
        m_current_elem_size=-1;
    }else{
        m_current_elem_size=m_it->second;
    }
}

template<typename T_MultiHashSet>
ConstMultiHashSetIterator<T_MultiHashSet>::
ConstMultiHashSetIterator(const MultiHashSetIterator<T_MultiHashSet>& other)
: m_it(other.m_it), m_end(other.m_end), m_current_elem_size(other.m_current_elem_size){

}

template<typename T_MultiHashSet>
ConstMultiHashSetIterator<T_MultiHashSet>& ConstMultiHashSetIterator<T_MultiHashSet>::
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
ConstMultiHashSetIterator<T_MultiHashSet>& ConstMultiHashSetIterator<T_MultiHashSet>::
operator++(int){
    ConstMultiHashSetIterator<T_MultiHashSet> old_copy= *this;
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
ConstMultiHashSetIterator<T_MultiHashSet>::ConstValuePointer ConstMultiHashSetIterator<T_MultiHashSet>::
operator->() const{
    return &m_it->first;
}

template<typename T_MultiHashSet>
ConstMultiHashSetIterator<T_MultiHashSet>::ConstValueReference ConstMultiHashSetIterator<T_MultiHashSet>::
operator*() const{
    return m_it->first;
}

template<typename T_MultiHashSet>
bool ConstMultiHashSetIterator<T_MultiHashSet>::
operator==(const ConstMultiHashSetIterator& other) const{
    return this->m_it==other.m_it;
}

template<typename T_MultiHashSet>
bool ConstMultiHashSetIterator<T_MultiHashSet>::
operator!=(const ConstMultiHashSetIterator& other) const{
    return !(*this==other);
}