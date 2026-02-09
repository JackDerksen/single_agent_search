#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "state.h"
#include "problem.h"

// Heuristic interface
struct Heuristic {
    virtual int operator()(const State &s) const = 0;
    virtual void initialize(const Problem& p) {}
    virtual ~Heuristic() = default;
};

struct ZeroHeuristic : Heuristic {
    int operator()(const State&) const override { return 0; }
};

struct MisplacedDiscHeuristic: Heuristic {
    int operator()(const State&) const override;
};

struct HopHeuristic : Heuristic {
    std::vector<std::vector<int>> hopMatrix;
    std::vector<std::vector<Disk>> goalStates;

    int operator()(const State&) const override;
    void initialize(const Problem& p) override;
};

uint32_t MisplacedDiscCount(const State& s, uint32_t startIdx, uint32_t n);

#endif // HEURISTIC_H
