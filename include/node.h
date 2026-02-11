#ifndef NODE_H
#define NODE_H

#include "state.h"
#include "common.h"

// Search node (state, cost, parent)
struct Node {
  State state;
  int g; // Cost so far
  int h; // Heuristic
  int f() const { return g + h; }

  // The move used to reach this node from its parent.
  // This lets the search avoid generating the immediate inverse move (undoing progress)
  Move lastMove = SWAP_RIGHT;

  Node * parent;
};

#endif // NODE_H
