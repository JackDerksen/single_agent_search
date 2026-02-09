#ifndef ASTAR_H
#define ASTAR_H

#include "common.h"
#include "problem.h"
#include "state.h"
#include "heuristic.h"
#include "node.h"
#include <queue>

// A* algorithm interface
class AStar {
	Problem problem;
	Heuristic & heuristic;
  public:
    std::vector<State> solve(bool debug);
    AStar(const Problem &problem, Heuristic &h);
    std::vector<State> solve();
};

#endif // ASTAR_H
