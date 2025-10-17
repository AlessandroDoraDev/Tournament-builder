#pragma once
#include <iostream>

template<typename T>
class Test_TypeClass{
public:
    Test_TypeClass(const T& t){
        std::cout<<"Passed as lvalue\n";
    }
    Test_TypeClass(T&& t){
        std::cout<<"Passed as rvalue\n";
    }
};