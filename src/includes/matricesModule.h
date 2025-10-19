#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <format>
#include <type_traits>
#include <algorithm>
#include <random>
#include <ranges>
#include "macros/myMacros.h"
#include "PlayerList.h"
#include "CSVRows.h"
#include "PseudoMatrix.h"
#include "absl/container/flat_hash_set.h"
#include "absl/container/btree_set.h"


CSVRows readCSV(const std::string& filename);

PlayerList formatRowsToPlayerList(const CSVRows& rows);

template<typename T>
void printMatrix(const PseudoMatrix<T>& matrix, int rows, int cols);

template<typename T>
void shuffleMatrixArray(PseudoMatrix<T>& matrix);

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
// hpp-like part


template<typename T>
void printMatrix(const PseudoMatrix<T>& matrix, int rows, int cols){
    auto range= std::ranges::views::iota; //just aliasing the function
    float min=-1;
    float max=-1;
    for(int i: range(0, rows)){
        std::string follow=", ";
        std::cout<<"[";
        int avg=0;
        for(int j: range(0, cols)){
            if(j==cols-1){
                follow.clear();
            }
            const T& elem=matrix[i*cols+j];
            if constexpr (std::is_convertible<T, std::string>::value){
                std::cout<<std::format("{:<30}", ((std::string)elem+follow));
                if constexpr (std::is_same<T, Player>::value){
                    avg+=static_cast<int>(elem.rank);
                }
            }else{
                std::ostringstream os;
                os<<elem;
                std::cout<<std::format("{:<30}", (os.str()+follow));
            }
        }
        std::cout<<"]";
        if constexpr (std::is_same<T, Player>::value){
            float team_val=static_cast<float>(avg)/cols;
            std::cout<<" "<<team_val;
            if(min==-1 and max==-1){
                min=max=team_val;
            }else{
                if(min>team_val){
                    min=team_val;
                }
                if(max<team_val){
                    max=team_val;
                }
            }
        }
        std::cout<<"\n";
    }
    if constexpr (std::is_same<T, Player>::value){
        std::cout<<"Gap between strongest and weakest team: "<<max-min<<"\n";
    }
}


template<typename T>
void shuffleMatrixArray(PseudoMatrix<T>& matrix){
    using RandomGenerator=std::mt19937; //ugly name, not choosing 19937 for a particular reason rn so better hide it
    std::random_device rnd_dv; //slow random gen -> personal note
    RandomGenerator generator(rnd_dv()); //fast random gen -> personal note
    std::shuffle(matrix.begin(), matrix.end(), generator);
}