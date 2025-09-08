#pragma once
#include <mutex>
#include <set>


/**
 * Need set and multiset to be thread-safe. No other resource is shared
 * 
 */
template<typename T, typename Hash>
class ThreadSafeUnorderedSet{
public:
    bool insert(const T&);
    bool contains(const T&);
private:
    std::mutex m_mutex;
    std::unordered_set<T, Hash> m_set;
};

template<typename T>
class ThreadSafeUnorderedMultiset{
public:
    void insert(const T&);
    bool contains(const T&);
private:
    std::mutex m_mutex;
    std::multiset<T> m_multiset;
};

template<typename T, typename Hash>
bool ThreadSafeUnorderedSet<T, Hash>::insert(const T& elem){
    std::lock_guard lock(m_mutex);
    std::pair<std::set::iterator, bool> pair=
        m_set.insert(elem);
    return pair.second;
}

template<typename T, typename Hash>
bool ThreadSafeUnorderedSet<T, Hash>::contains(const T& elem){
    std::lock_guard lock(m_mutex);
    return m_set.contains(elem);
}

template<typename T>
void ThreadSafeUnorderedMultiset<T>::insert(const T& elem){
    std::lock_guard lock(m_mutex);
    m_multiset.insert(elem);
}

template<typename T>
bool ThreadSafeUnorderedMultiset<T>::contains(const T& elem){
    std::lock_guard lock(m_mutex);
    return m_multiset.contains(elem);
}