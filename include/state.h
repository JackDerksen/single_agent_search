#ifndef STATE_H
#define STATE_H

#include "common.h"

/// Represents the state of the puzzle, containing only the small disks that change.
struct State {
  /// Stack of small disks, represented as a vector of Disk IDs.
  std::vector<Disk> small;
  uint32_t zeroIndex = 0;

  /// Equality operator: compares both large and small stacks element-wise.
  bool operator==(const State &other) const;
  bool IsGoal(int n);
  uint32_t GetZeroIndex();
};

/// Output operator for printing the State as space-separated small disk values.
std::ostream& operator<<(std::ostream& os, const State& s);

/// Hash function for State, order-sensitive and deterministic, based on small disk vector.
struct StateHash {
    /// Computes a hash value for the given State.
    std::size_t operator()(const State& s) const;
};

#endif // STATE_H
