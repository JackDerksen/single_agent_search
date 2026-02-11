#ifndef ASTAR_H
#define ASTAR_H

#include "common.h"
#include "problem.h"
#include "state.h"
#include "heuristic.h"

// Actually updated to IDA*
class AStar {
	Problem problem;
	Heuristic & heuristic;
  public:
    // Solve the problem. If debug flag is set, print stats
    std::vector<State> solve(bool debug);

    // Convenience overload
    std::vector<State> solve() { return solve(false); }

    AStar(const Problem &problem, Heuristic &h);
};

#endif // ASTAR_H
