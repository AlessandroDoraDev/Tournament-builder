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

std::string formatDurationDetailed(std::chrono::nanoseconds ns) {
    using namespace std::chrono;

    auto secs = duration_cast<seconds>(ns);
    ns -= secs;

    auto millis = duration_cast<milliseconds>(ns);
    ns -= millis;

    auto micros = duration_cast<microseconds>(ns);
    ns -= micros;

    auto nanosecs = ns;

    // Format with fixed width for nice alignment
    return std::format("{} s {} ms {} us {} ns",
        secs.count(),
        millis.count(),
        micros.count(),
        nanosecs.count());
}


FORCE_INLINE void reorderTheMatrixToBeSnakeLike(ConfigMatrix& config_matrix, ConfigMatrix& out_matrix, int rows, int cols) {
    auto range = std::ranges::views::iota; //just aliasing the function

    out_matrix.resize(rows * cols);
    int i2 = 0;
    int j2 = 0;
    for (int i : range(0, rows)) {
        for (int j : range(0, cols)) {
            out_matrix[i2 * cols + j2] = config_matrix[i * cols + j];
            i2 = (i2 + 1) % rows;
            if (i2 == 0) {
                j2 = (j2 + 1) % rows;
            }
        }
    }
}
void strategicallyShuffleMatrixArray(ConfigMatrix& matrix, int shuffle_radius) {
    using RandomGenerator = std::mt19937; //ugly name, not choosing 19937 for a particular reason rn so better hide it
    std::random_device rnd_dv; //slow random gen -> personal note
    RandomGenerator generator(rnd_dv()); //fast random gen -> personal note
    std::vector<std::pair<int, int>>breaks; //begin, end
    BaseRank lastRankClass = matrix[0].second;
    int i = 1;
    for (const IDRankPair& elem : matrix) {
        if (lastRankClass != elem.second) {
            lastRankClass = elem.second;
            int begin = i - shuffle_radius;
            int end = i + shuffle_radius;
            begin = begin >= 0 ? begin : 0;
            end = end < matrix.size() ? end : matrix.size();
            breaks.emplace_back(begin, end);
            i++;
        }
    }
    for (std::pair<int, int> beginEndPair : breaks) {
        std::shuffle(
            matrix.begin() + beginEndPair.first,
            matrix.begin() + beginEndPair.second,
            generator
        );
    }
}


void findMatrixArrayBreaks(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks, int shuffle_radius) {
    BaseRank lastRankClass = matrix[0].second;
    int i = 1;
    for (const IDRankPair& elem : matrix) {
        if (lastRankClass != elem.second) {
            lastRankClass = elem.second;
            int begin = i - shuffle_radius;
            int end = i + shuffle_radius;
            begin = begin >= 0 ? begin : 0;
            end = end < matrix.size() ? end : matrix.size();
            breaks.emplace_back(begin, end);
        }
        i++;
    }
}

void findMatrixArrayBreaks2(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks, int shuffle_radius) {
    BaseRank lastRankClass = matrix[0].second;
    std::vector<int> pre_breaks;
    int i = 1;
    int last_begin, last_end;
    for (const IDRankPair& elem : matrix) {
        if (lastRankClass != elem.second) {
            lastRankClass = elem.second;
            pre_breaks.push_back(i);
        }
        i++;
    }
    for (i = 0; i < pre_breaks.size(); i++) {
        int j = pre_breaks[i];
        int begin, end;
        if (i == 0) {
            int next_j = pre_breaks[i + 1];
            begin = j - shuffle_radius;
            end = j + shuffle_radius;
            begin = begin >= 0 + shuffle_radius ? begin : (0 + j)/2 + 1;
            end = end < next_j - shuffle_radius ? end : (j + next_j)/2 + 1;
            last_end = 0 + shuffle_radius < j - shuffle_radius ? shuffle_radius : begin;
        }
        else if (i == pre_breaks.size() - 1) {
            int prev_j = pre_breaks[i - 1];
            begin = j - shuffle_radius;
            end = j + shuffle_radius;
            begin = begin >= prev_j + shuffle_radius ? begin : (prev_j + j)/2 + 1;
            end = end < matrix.size() - shuffle_radius ? end : (j + matrix.size())/2 + 1;
            last_begin = matrix.size() - shuffle_radius;
            last_begin = last_begin >= j + shuffle_radius ? last_begin : end;
        }
        else {
            int prev_j = pre_breaks[i - 1];
            int next_j = pre_breaks[i + 1];
            begin = j - shuffle_radius;
            end = j + shuffle_radius;
            begin = begin >= prev_j + shuffle_radius ? begin : (j + prev_j)/2 + 1;
            end = end < next_j - shuffle_radius ? end : (j + next_j)/2 + 1;
        }
        breaks.emplace_back(begin, end);
    }
    breaks.emplace_back(last_begin, last_end);
}

FORCE_INLINE void strategicallyShuffleMatrixArray(ConfigMatrix& matrix, std::vector<std::pair<int, int>>& breaks) {
    using RandomGenerator = std::mt19937; //ugly name, not choosing 19937 for a particular reason rn so better hide it
    static std::random_device rnd_dv; //slow random gen -> personal note
    static RandomGenerator generator(rnd_dv()); //fast random gen -> personal note
    static std::pair<int, int> last_beginEndPair = breaks.back();
    /*std::shuffle(
        breaks.begin(),
        breaks.end()-1,
        generator
    );*/
    for (std::pair<int, int> beginEndPair: breaks) {
        if (beginEndPair != last_beginEndPair) {
            std::shuffle(
                matrix.begin() + beginEndPair.first,
                matrix.begin() + beginEndPair.second,
                generator
            );
        }
        else {
            /*CircularRangeView<ConfigMatrix> r_matrix(matrix, last_beginEndPair.first, last_beginEndPair.second);
            std::shuffle(
                r_matrix.begin(),
                r_matrix.end(),
                generator
            );*/
        }
    }
    static int matrix_half_index = matrix.size() / 2;
    static int n_specular_swaps = matrix_half_index / 2;
    static std::uniform_int_distribution<> distribution(0, matrix_half_index-1);
    for (int i : std::ranges::views::iota(0, matrix_half_index)) {
        i = distribution(generator);
        int j = distribution(generator);
        std::swap(matrix[i], matrix[matrix.size()-1 - j]);
    }

}

ConfigMatrix simpleStrategy(ConfigMatrix& matrix, int n_teams, int n_players) {
    const int N_THREADS = 6;
    const int NUMBER_OF_TRIES = 5'000'000*N_THREADS;
    //const int NUMBER_OF_TRIES = 1'000 * N_THREADS;
    std::mutex print_mutex;
    std::mutex sol_mutex;
    std::mutex i_mutex;

    //absl::flat_hash_set<ConfigMatrix> sol_set;
    std::vector<ConfigMatrix> sol_set;
    sol_set.resize(NUMBER_OF_TRIES);
    //sol_set.reserve(NUMBER_OF_TRIES);
    int bit = 0xffff;
    std::vector<std::pair<int, int>>breaks;
    std::sort(matrix.begin(), matrix.end(),
        [](const IDRankPair& p1, const IDRankPair& p2) {
            return p1.second < p2.second;
        });
    findMatrixArrayBreaks2(matrix, breaks, 4);
    std::cout << "[";
    for (std::pair<int, int>& brk : breaks) {
        std::cout << "("<<brk.first<<", "<<brk.second<<") ";
    }
    std::cout << "]\n";
    std::vector<std::thread> threads;
    int i = 0;
    for (int k = 0; k < N_THREADS; k++) {
        threads.emplace_back([&](int k) {
            int j = 0;
            int actual_tries = NUMBER_OF_TRIES / N_THREADS;
            while (j < actual_tries) {
                ConfigMatrix copy = matrix;
                strategicallyShuffleMatrixArray(copy, breaks);
                reorderTheMatrixToBeSnakeLike(copy, sol_set[j* N_THREADS + k], n_teams, N_PLAYERS);
                j++;
            }
            }, k);
    }
    for (std::thread& t : threads) {
        t.join();
    }
    std::cout <<"                                        \r";
    return *std::min_element(sol_set.begin(), sol_set.end(),
        [n_teams, n_players](const ConfigMatrix& m, const ConfigMatrix& m2){
            return configMatrixQuality(m, n_teams, n_players)<configMatrixQuality(m2, n_teams, n_players);
        });
}

ConfigMatrix dumbStrategy(ConfigMatrix& matrix, int n_teams, int n_players) {
    const int N_THREADS = 1;
    const int NUMBER_OF_TRIES = 1'000'000 * N_THREADS;
    //const int NUMBER_OF_TRIES = 1'000 * N_THREADS;
    std::mutex print_mutex;
    std::mutex sol_mutex;
    std::mutex i_mutex;
    using RandomGenerator = std::mt19937; //ugly name, not choosing 19937 for a particular reason rn so better hide it
    std::random_device rnd_dv; //slow random gen -> personal note

    //absl::flat_hash_set<ConfigMatrix> sol_set;
    std::vector<ConfigMatrix> sol_set;
    sol_set.resize(NUMBER_OF_TRIES);
    //sol_set.reserve(NUMBER_OF_TRIES);
    int bit = 0xffff;
    std::sort(matrix.begin(), matrix.end(),
        [](const IDRankPair& p1, const IDRankPair& p2) {
            return p1.second < p2.second;
        });
    std::vector<std::thread> threads;
    std::array<std::pair<float, ConfigMatrix*>, N_THREADS> winner;
    for (std::pair<float, ConfigMatrix*>& w_pair : winner) {
        w_pair.first = 10.0;
        w_pair.second = nullptr;
    }
    int i = 0;
    for (int k = 0; k < N_THREADS; k++) {
        threads.emplace_back([&](int k, RandomGenerator gen) {
            int j = 0;
            int actual_tries = NUMBER_OF_TRIES / N_THREADS;
            while (j < actual_tries) {
                ConfigMatrix copy = matrix;
                std::shuffle(copy.begin(), copy.end(), gen);
                reorderTheMatrixToBeSnakeLike(copy, sol_set[j * N_THREADS + k], n_teams, N_PLAYERS);
                float current_quality = configMatrixQuality(sol_set[j * N_THREADS + k], n_teams, n_players);
                if (current_quality < winner[k].first) {
                    winner[k].first = current_quality;
                    winner[k].second = &sol_set[j * N_THREADS + k];
                }
                j++;
            }
            }, k, RandomGenerator(rnd_dv()));
    }
    for (std::thread& t : threads) {
        t.join();
    }
    std::cout << "                                        \r";
    return *std::min_element(winner.begin(), winner.end(),
        [n_teams, n_players](const std::pair<float, ConfigMatrix*>& m, const std::pair<float, ConfigMatrix*>& m2) {
            return m.first < m2.first;
        })->second;
}