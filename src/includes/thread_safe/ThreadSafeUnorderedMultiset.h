#pragma once
#include <mutex>
#include <set>



template<typename T>
class ThreadSafeUnorderedMultiset{
public:
    void insert(const T&);
    bool contains(const T&);
private:
    std::mutex m_mutex;
    std::multiset<T> m_multiset;
};



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