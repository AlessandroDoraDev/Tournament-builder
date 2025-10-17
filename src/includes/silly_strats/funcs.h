#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <algorithm>
#include <array>
#include <thread>
#include <utility>
#include <ranges>
#include <format>
#include <random>
#include <mutex>
#include "ConfigMatrix.h"
#include "CircularRangeView.h"
#include "macros/myMacros.h"
#include "matricesModule.h"

std::string formatDurationDetailed(std::chrono::nanoseconds ns);


FORCE_INLINE void reorderTheMatrixToBeSnakeLike(ConfigMatrix& config_matrix, ConfigMatrix& out_matrix, int rows, int cols);

void strategicallyShuffleMatrixArray(ConfigMatrix& matrix, int shuffle_radius);


void findMatrixArrayBreaks(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks, int shuffle_radius);

void findMatrixArrayBreaks2(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks, int shuffle_radius);

FORCE_INLINE void strategicallyShuffleMatrixArray(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks);

ConfigMatrix simpleStrategy(ConfigMatrix& matrix, int n_teams, int n_players);

ConfigMatrix dumbStrategy(ConfigMatrix& matrix, int n_teams, int n_players);

template<typename Container>
auto algoritmoGreedy(Container bravure, int giocatoriPerSquadra) -> std::vector<std::vector<typename Container::value_type>> {
    std::vector<typename Container::value_type> giocatoriOrdinati(bravure.begin(), bravure.end());
    
    std::sort(giocatoriOrdinati.begin(), giocatoriOrdinati.end(), std::greater<typename Container::value_type>());
    
    int numSquadre = giocatoriOrdinati.size() / giocatoriPerSquadra;
    std::vector<std::vector<typename Container::value_type>> squadre(numSquadre);
    std::vector<typename Container::value_type> punteggiSquadre(numSquadre, 0);
    
    for (const auto& bravura : giocatoriOrdinati) {
        int minIdx = -1;
        typename Container::value_type minPunteggio = (std::numeric_limits<typename Container::value_type>::max)();
        
        for (int i = 0; i < numSquadre; i++) {
            if (squadre[i].size() < giocatoriPerSquadra && punteggiSquadre[i] < minPunteggio) {
                minIdx = i;
                minPunteggio = punteggiSquadre[i];
            }
        }
        
        if (minIdx != -1) {
            squadre[minIdx].push_back(bravura);
            punteggiSquadre[minIdx] += bravura;
        }
    }
    
    return squadre;
}


template<typename T>
void stampaMatrice(const std::vector<std::vector<T>>& matrice) {
    for (std::size_t i = 0; i < matrice.size(); ++i) {
        std::cout << "Squadra " << (i + 1) << ": [";
        for (std::size_t j = 0; j < matrice[i].size(); ++j) {
            std::cout << matrice[i][j];
            if (j < matrice[i].size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
}