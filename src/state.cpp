#include "state.h"
#include "common.h"

#include <cassert>
#include <cstdint>
#include <stdexcept>

/// Implements equality operator by comparing the small vectors only.
bool State::operator==(const State& other) const { return small == other.small; }

bool State::IsGoal(int n) const
{
    assert(zeroIndex < small.size());
    assert(small[zeroIndex] == 0);

    const int total = static_cast<int>(small.size());
    const int goalCount = n * n; // number of non-zero discs

    for (int i = 0; i < goalCount; ++i)
    {
        const int index = (static_cast<int>(zeroIndex) + 1 + i) % total;
        const int expected = (i / n) + 1;
        if (small[static_cast<std::size_t>(index)] != static_cast<Disk>(expected))
            return false;
    }
    return true;
}

std::uint32_t State::GetZeroIndex() const
{
    for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(small.size()); ++i)
    {
        if (small[i] == 0) { return i; }
    }

    // This is a programmer error: the representation requires exactly one 0.
    throw std::runtime_error("Invalid State: small disk state has no empty space (0).");
}

/// Implements a printing operator for the State as space-separated small disk values (0 for uncovered).
std::ostream& operator<<(std::ostream& os, const State& s) {
    for (size_t i = 0; i < s.small.size(); ++i) {
        if (i > 0) os << " ";
        os << static_cast<int>(s.small[i]);
    }
    return os;
}

/// Computes a deterministic hash for the State by hashing the small disk vector's elements in order.
/// Uses a simple polynomial hash (multiplier 31).
std::size_t StateHash::operator()(const State& s) const {
    std::size_t h = 0;
    for (Disk val : s.small) {
        h = h * 31 + std::hash<Disk>{}(val);
    }
    return h;
}
