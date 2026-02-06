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

struct SlowHeuristic: Heuristic {
    int operator()(const State&) const override;
};

uint32_t MisplacedTileCount(const State& s, uint32_t startIdx, uint32_t n);

#endif // HEURISTIC_H
