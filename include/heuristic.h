#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "state.h"

// Heuristic interface
struct Heuristic {
    virtual int operator()(const State &s) const = 0;
    virtual ~Heuristic() = default;
};

struct ZeroHeuristic : Heuristic {
    int operator()(const State&) const override { return 0; }
};

#endif // HEURISTIC_H
