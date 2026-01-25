#ifndef NODE_H
#define NODE_H

#include "state.h"

// Search node (state, cost, parent)
struct Node {
  State state;
  int g; // Cost so far
  int h; // Heuristic
  int f() const { return g + h; }

  Node * parent;
};

#endif // NODE_H
