#include "problem.h"

Problem::Problem(const std::vector<Disk>& large_disks) : large(large_disks) {
}

bool Problem::is_goal(const State &s) const {
    return false;
}

std::vector<State> Problem::successors(const State &s) const {
    return {};
}
