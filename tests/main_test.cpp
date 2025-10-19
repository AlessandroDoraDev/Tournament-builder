#include <iostream>
#include <string>
#include <windows.h>
#include <ranges>
#include "globals.h"
#include "to_strings.h"

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
}
