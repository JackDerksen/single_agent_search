#include "state.h"

/// Implements equality operator by comparing the large and small vectors.
bool State::operator==(const State &other) const {
    return large == other.large && small == other.small;
}

/// Implements a printing operator for the State as space-separated small disk values (0 for uncovered).
std::ostream& operator<<(std::ostream& os, const State& s) {
    for (size_t i = 0; i < s.small.size(); ++i) {
        if (i > 0) os << " ";
        os << s.small[i];
    }
    return os;
}

/// Computes a deterministic hash for the State by hashing the small disk vector's elements in order.
/// Uses a simple polynomial hash (multiplier 31).
std::size_t StateHash::operator()(const State& s) const {
    std::size_t h = 0;
    for (int val : s.small) {
        h = h * 31 + std::hash<int>{}(val);
    }
    return h;
}
