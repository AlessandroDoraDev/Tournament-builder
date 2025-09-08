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
#include "matricesModule.h"
#include "core.h"

template <typename Container>
class CircularRangeView {
public:
    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using difference_type = std::ptrdiff_t;

    CircularRangeView(Container& c, size_type start, size_type end)
        : container(c), start_idx(start), end_idx(end)
    {
        assert(!container.empty());
        assert(start < container.size());
        assert(end < container.size());
    }

    struct iterator {
        using iterator_category = std::random_access_iterator_tag;
        using value_type = typename Container::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator(Container* cont, size_type pos, size_type start, size_type end, bool is_end = false)
            : container(cont), index(pos), start_index(start), end_index(end), is_end_iterator(is_end) {
        }

        reference operator*() const { return (*container)[index]; }
        pointer operator->() const { return &(*container)[index]; }

        iterator& operator++() {
            if (index == end_index) {
                is_end_iterator = true;
            }
            else {
                index = (index + 1) % container->size();
            }
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator& operator--() {
            if (is_end_iterator) {
                // Going back from end() iterator: move to end index
                is_end_iterator = false;
                index = end_index;
            }
            else {
                if (index == start_index) {
                    // Can't go before begin (makes iterator invalid)
                    // Let's wrap backward from start to end (circular)
                    index = end_index;
                }
                else {
                    index = (index + container->size() - 1) % container->size();
                }
            }
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        iterator operator+(difference_type n) const {
            if (is_end_iterator) return *this; // no move past end
            size_type sz = container->size();
            size_type dist_to_end = distance_to_end();
            if (n >= dist_to_end) {
                // Move past or to end: return end iterator
                return iterator(container, end_index, start_index, end_index, true);
            }
            else {
                size_type new_index = (index + n) % sz;
                return iterator(container, new_index, start_index, end_index, false);
            }
        }

        iterator& operator+=(difference_type n) {
            *this = *this + n;
            return *this;
        }

        iterator operator-(difference_type n) const {
            return *this + (-n);
        }

        iterator& operator-=(difference_type n) {
            *this = *this - n;
            return *this;
        }

        difference_type operator-(const iterator& other) const {
            assert(container == other.container);
            // Calculate distance from other to *this in circular range
            difference_type dist_this = distance_from_start(index);
            difference_type dist_other = distance_from_start(other.index);
            return dist_this - dist_other;
        }

        reference operator[](difference_type n) const {
            return *(*this + n);
        }

        bool operator==(const iterator& other) const {
            if (is_end_iterator && other.is_end_iterator)
                return true;
            return container == other.container && index == other.index && is_end_iterator == other.is_end_iterator;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        bool operator<(const iterator& other) const {
            return distance_from_start(index) < distance_from_start(other.index);
        }

        bool operator>(const iterator& other) const {
            return other < *this;
        }

        bool operator<=(const iterator& other) const {
            return !(other < *this);
        }

        bool operator>=(const iterator& other) const {
            return !(*this < other);
        }

    private:
        Container* container;
        size_type index;
        size_type start_index;
        size_type end_index;
        bool is_end_iterator = false;

        size_type distance_from_start(size_type idx) const {
            size_type sz = container->size();
            if (end_index >= start_index) {
                // simple case: linear segment
                return idx >= start_index ? idx - start_index : sz; // out-of-range idx => large number
            }
            else {
                // wrap case
                if (idx >= start_index)
                    return idx - start_index;
                else
                    return sz - start_index + idx;
            }
        }

        difference_type distance_to_end() const {
            size_type sz = container->size();
            if (end_index >= index)
                return end_index - index;
            else
                return sz - index + end_index;
        }
    };

    iterator begin() { return iterator(&container, start_idx, start_idx, end_idx, false); }
    iterator end() { return iterator(&container, end_idx, start_idx, end_idx, true); }

private:
    Container& container;
    size_type start_idx;
    size_type end_idx;
};

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