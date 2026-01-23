#ifndef STATE_H
#define STATE_H

#include "common.h"

/// Represents the state of the puzzle, with two stacks: large and small disks.
struct State {
  /// Stack of large disks, represented as a vector of integers (e.g., disk sizes).
  std::vector<int> large;
  /// Stack of small disks, represented as a vector of integers.
  std::vector<int> small;

  /// Default lightweight constructor.
  State() = default;

  /// Equality operator: compares both large and small stacks element-wise.
  bool operator==(const State &other) const;
};

/// Output operator for printing the State as space-separated small disk values.
std::ostream& operator<<(std::ostream& os, const State& s);

/// Hash function for State, order-sensitive and deterministic, based on small disk vector.
struct StateHash {
    /// Computes a hash value for the given State.
    std::size_t operator()(const State& s) const;
};

#endif // STATE_H
