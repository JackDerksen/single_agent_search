#ifndef PROBLEM_H
#define PROBLEM_H

#include "common.h"
#include "state.h"

// Game/problem rules (successors, testing the goal, etc.)
struct Problem {
    std::vector<Disk> large;  // Read-only large disks
    std::vector<Disk> small;  // Read-only large disks
    explicit Problem(const std::vector<Disk>& large_disks, const std::vector<Disk>& small_disks);
};

#endif // PROBLEM_H
