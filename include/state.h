#ifndef STATE_H
#define STATE_H

#include "common.h"

// Representations of the puzzle state
struct State {
  std::vector<int> large;
  std::vector<int> small;

  bool operator==(const State &other) const;
};

#endif // STATE_H
