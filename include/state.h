#ifndef STATE_H
#define STATE_H

#include "common.h"

#include <cstdint>
#include <ostream>
#include <vector>

/// Represents the state of the puzzle, containing only the small disks that change.
struct State {
  /// Stack of small disks, represented as a vector of Disk IDs.
  std::vector<Disk> small;
  std::uint32_t zeroIndex = 0;

  /// Compare the small disk layout.
  bool operator==(const State &other) const;

  /// Returns true if this state is a goal state for the given group size `n`.
  bool IsGoal(int n) const;

  /// Finds the index of the empty space (0) in `small`.
  /// Precondition: `small` contains exactly one 0.
  std::uint32_t GetZeroIndex() const;
};

/// Output operator for printing the State as space-separated small disk values.
std::ostream& operator<<(std::ostream& os, const State& s);

/// Hash function for State, order-sensitive and deterministic, based on small disk vector.
struct StateHash {
    /// Computes a hash value for the given State.
    std::size_t operator()(const State& s) const;
};

#endif // STATE_H
