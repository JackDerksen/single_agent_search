#include "astar.h"
#include "ida_star.h"
#include "common.h"
#include "problem.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

// ---------------------------- I/O + Utilities ----------------------------

Problem ReadUserInput(std::size_t num_large) {
    // Read large disks
    std::vector<Disk> large(num_large);
    for (std::size_t i = 0; i < num_large; ++i) {
        int value;
        std::cin >> value;
        large[i] = static_cast<Disk>(value);
    }

    // Read small disks
    std::vector<Disk> small(num_large);
    for (std::size_t i = 0; i < num_large; ++i) {
        int value;
        std::cin >> value;
        small[i] = static_cast<Disk>(value);
    }

    const uint16_t groupSize = static_cast<uint16_t>(
        std::floor(std::sqrt(static_cast<double>(num_large))));

    return Problem(large, small, groupSize);
}

void PrintSolution(const std::vector<State>& solution) {
    std::cout << "Solution is" << std::endl;
    for (const State& s : solution) {
        std::cout << s << std::endl;
    }
}

enum class SolverKind {
    IDAStar,
    AStar,
};

struct SolveOptions {
    SolverKind solver = SolverKind::IDAStar;
    bool debug = false; // pass through to solver (debug printing)
};

// Keep solver selection logic in one place.
template <typename HeuristicT>
std::vector<State> SolveProblem(const Problem& p, HeuristicT& heuristic, const SolveOptions& opts) {
    if (opts.solver == SolverKind::IDAStar) {
        IDAStar solver(p, heuristic);
        return solver.solve(opts.debug);
    } else {
        AStar solver(p, heuristic);
        return solver.solve(opts.debug);
    }
}

// ---------------------------- Debug / Test mode ----------------------------

void RunSanityTests() {
    State s1;
    s1.small = {4, 5};

    State s2 = s1;

    State s3;
    s3.small = {4, 5, 6};

    StateHash hasher;
    std::cout << "Hash s1: " << hasher(s1) << std::endl;
    std::cout << "Hash s2: " << hasher(s2) << std::endl;
    std::cout << "Hash s3: " << hasher(s3) << std::endl;

    if (hasher(s1) != hasher(s2)) {
        std::cout << "Error: identical states have different hashes" << std::endl;
        assert(false);
    }
    if (hasher(s1) == hasher(s3)) {
        std::cout << "Error: different states have same hash" << std::endl;
        assert(false);
    }
    std::cout << "Printing s1: " << s1 << std::endl;

    // Test IsGoal
    State g1 = {{0, 1, 1, 1, 2, 2, 2, 3, 3, 3}, 0};
    assert(g1.IsGoal(3));

    State g2 = {{2, 2, 0, 1, 1}, 2};
    assert(g2.IsGoal(2));

    State g3 = {{1, 1, 1, 2, 2, 3, 2, 3, 3, 0}, 9};
    assert(!g3.IsGoal(3));
}

Problem GetDebugProblem() {
    // Default to a “harder” one so debug output is meaningful (as opposed to AB10 or AB17).
    return Problem(
        {1, 2, 3, 4, 5, 4, 3, 2, 1, 1, 2, 3, 4, 1, 2, 3, 4},
        {3, 2, 3, 2, 1, 0, 2, 1, 1, 3, 4, 1, 4, 2, 4, 3, 4},
        4);
}

int RunNormalMode(std::size_t num_large) {
    Problem p = ReadUserInput(num_large);

    // Default heuristic for stdin-driven mode.
    ZeroHeuristic heuristic;

    SolveOptions opts;
    opts.solver = SolverKind::IDAStar;
    opts.debug = false;

    std::vector<State> solution = SolveProblem(p, heuristic, opts);
    PrintSolution(solution);
    return 0;
}

int RunDebugMode() {
    // RunSanityTests();

    Problem p = GetDebugProblem();

    // Use a stronger heuristic for debug mode by default.
    HopHeuristic heuristic;

    SolveOptions opts;
    opts.solver = SolverKind::IDAStar;
    opts.debug = true;

    std::vector<State> solution = SolveProblem(p, heuristic, opts);
    PrintSolution(solution);
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <n>\n"
                  << "  n > 0: read instance from stdin (expects 2*n integers: large then small)\n"
                  << "  n = 0: run debug mode (hardcoded instances + debug output)\n";
        return 1;
    }

    const std::size_t n = static_cast<std::size_t>(std::stoi(argv[1]));
    if (n == 0) {
        return RunDebugMode();
    }
    return RunNormalMode(n);
}
