#include "problem.h"

Problem::Problem(size_t n) {
}

bool Problem::is_goal(const State &s) const {
    return false;
}

std::vector<State> Problem::successors(const State &s) const {
    return {};
}
