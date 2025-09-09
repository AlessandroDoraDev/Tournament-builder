#pragma once

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