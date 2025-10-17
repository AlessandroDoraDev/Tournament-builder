#include <iostream>
#include <string>
#include <windows.h>
#include <ranges>
#include "globals.h"
#include "to_strings.h"
//#include "test_includes/MultiHashSetL2.h"
#include "BaseID.h"
//#include "TeamMap.h"

void my_assert(bool expression, std::string text_if_failure){
    if(!expression){
        std::cout<<text_if_failure<<"\n";
    }
}

template<typename ConType, typename ElemType>
inline bool my_contains(ConType arr, ElemType elem){
    return std::find(arr.begin(), arr.end(), elem)!=arr.end();
}

//void test_MultiHashSet();

void main_test(){
    std::cout<<"Esecuzione test\n";
    //MultiHashSetL2 m;
    //test_MultiHashSet();
}


// void test_MultiHashSet(){
//     ConfigSetOnMaps multiset1;
//     MultiHashSet<std::string, BaseID> multiset;
//     std::string s="ciao";
//     std::string s2="ciaoo";
//     multiset.multiset_insert(std::move(s));
//     multiset.multiset_insert(s2);
//     multiset.multiset_emplace("ciao");
//     multiset.multiset_insert(s);
//     multiset.multiset_insert("ciaoooo");
//     std::cout<<"String was: "<<s<<"\n";
//     MultiHashSet<std::string, BaseID>::ConstIterator it=multiset.multiset_begin();
//     MultiHashSet<std::string, BaseID>::ConstIterator end=multiset.multiset_end();
//     for(; it!=end; ++it){
//         std::cout<<*it<<"!!!\n";
//     }
//     std::cout<<(std::string)multiset;
// }