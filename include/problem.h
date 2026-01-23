#ifndef PROBLEM_H
#define PROBLEM_H

#include "common.h"
#include "state.h"

// Game/problem rules (successors, testing the goal, etc.)
class Problem {
  public:
    explicit Problem(size_t n);

    bool is_goal(const State &s) const;
    std::vector<State> successors(const State &s) const;
};

#endif // PROBLEM_H
