#include "TeamArray.h"
#include <algorithm>

const Rank& TeamArray::operator[](std::size_t offset) const{
    return m_span[offset];
}

TeamArray::iterator TeamArray::begin() const{
    return m_span.begin();
}

TeamArray::iterator TeamArray::end() const{
    return m_span.end();
}


void TeamArray::replaceAt(Rank org_rank, Rank dst_rank){
    TeamArray::iterator begin=m_span.begin();
    TeamArray::iterator end=m_span.end();
    bool replaced=false;
    for(; begin!=end&&!replaced; ++begin){
        if(*begin==org_rank){
            replaced=true;
            *begin=dst_rank;
        }
    }
    std::sort(m_span.begin(), m_span.end());
    m_pnts+=static_cast<int>(dst_rank)-static_cast<int>(org_rank);
}

int TeamArray::getPnts(){
    return m_pnts;
}

void TeamArray::sort(){
    std::sort(m_span.begin(), m_span.end());
}