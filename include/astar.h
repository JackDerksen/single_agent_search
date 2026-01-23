#ifndef ASTAR_H
#define ASTAR_H

#include "common.h"
#include "problem.h"
#include "state.h"
#include "heuristic.h"

// A* algorithm interface
class AStar {
  public:
    AStar(const Problem &problem, const Heuristic &h);

    std::vector<State> solve(const State &start);
};

#endif // ASTAR_H
