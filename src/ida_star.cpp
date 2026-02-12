#include "ida_star.h"

#include "common.h"
#include "move.h"
#include "util.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>

/*
NOTE: I generated these doc comments with Claude.

Iterative Deepening A* (IDA*) implementation.

IDA* does repeated depth-first searches with an f-cost bound (threshold).
Each DFS prunes any node whose f = g + h exceeds the current threshold.

Algorithm overview:
1) threshold = h(start)  (since g(start) = 0)
2) Repeat:
   a) Run a DFS that only explores nodes with f <= threshold
   b) If goal found: return the path
   c) Else: set threshold to the minimum f-value that exceeded the old threshold
            during the DFS, and repeat

Debug stats:
- When `solve(true)` is used, we print per-iteration information:
  - iteration number
  - current threshold
  - nodes expanded (states for which we generated successors)
  - prunes (states cut off because f > threshold)
- We also print a final summary once solved.
*/

namespace {

// Sentinel return value used to signal success (goal found).
constexpr int kFound = -1;

// Apply a move to a state to generate a successor state.
// This mirrors the logic in `astar.cpp` (GenerateMoveNode / ExpandNode).
State ApplyMove(const Problem &problem, const State &parentState, Move move) {
    State s = parentState;

    const uint32_t numDisks = static_cast<uint32_t>(s.small.size());
    const uint32_t largeValue = static_cast<uint32_t>(problem.large[s.zeroIndex]);

    switch (move) {
        case SWAP_RIGHT:
            s.zeroIndex = static_cast<uint32_t>(
                WrapIndex(static_cast<int>(s.zeroIndex + largeValue), static_cast<int>(numDisks)));
            break;
        case SWAP_LEFT:
            s.zeroIndex = static_cast<uint32_t>(
                WrapIndex(static_cast<int>(s.zeroIndex) - static_cast<int>(largeValue), static_cast<int>(numDisks)));
            break;
        case SWAP_N_RIGHT:
            s.zeroIndex =
                static_cast<uint32_t>(WrapIndex(static_cast<int>(s.zeroIndex + 1), static_cast<int>(numDisks)));
            break;
        case SWAP_N_LEFT:
            s.zeroIndex = static_cast<uint32_t>(WrapIndex(static_cast<int>(s.zeroIndex) - 1, static_cast<int>(numDisks)));
            break;
        default:
            assert(false);
    }

    // Perform the swap to move the zero to the new index.
    s.small[parentState.zeroIndex] = s.small[s.zeroIndex];
    s.small[s.zeroIndex] = 0;

    return s;
}

// Enumerate legal moves from `state`.
// Matches `ExpandNode` in `astar.cpp`:
// - Always allow SWAP_LEFT and SWAP_RIGHT.
// - Additionally allow SWAP_N_LEFT and SWAP_N_RIGHT only if the large disk value != 1.
uint32_t GetMoves(const Problem &problem, const State &state, Move outMoves[4]) {
    const uint32_t largeValue = static_cast<uint32_t>(problem.large[state.zeroIndex]);

    outMoves[0] = SWAP_RIGHT;
    outMoves[1] = SWAP_LEFT;

    if (largeValue != 1) {
        outMoves[2] = SWAP_N_RIGHT;
        outMoves[3] = SWAP_N_LEFT;
        return 4;
    }

    return 2;
}

} // namespace

IDAStar::IDAStar(const Problem &p, Heuristic &h) : problem(p), heuristic(h) {
    heuristic.initialize(problem);
}

std::vector<State> IDAStar::solve(bool debug) {
    // `path` is the current DFS path from start to the current node.
    // If we reach the goal, this path is exactly the solution we return.
    std::vector<State> path;
    path.reserve(1024);

    const State start = problem.smallState;
    path.push_back(start);

    // Initial threshold is f(start) = g(0) + h(start) = h(start).
    int threshold = heuristic(start);

    // Debug counters across all iterations.
    uint64_t totalExpanded = 0;
    uint64_t totalPruned = 0;
    uint32_t iteration = 0;

    // Iteratively increase the threshold until a solution is found.
    while (true) {
        IterationStats stats;

        std::vector<State> solution;
        solution.reserve(path.size());

        // Run bounded DFS from the start.
        int t = dfsBounded(/*current=*/start,
                           /*g=*/0,
                           /*threshold=*/threshold,
                           /*path=*/path,
                           /*outSolution=*/solution,
                           /*debug=*/debug,
                           /*stats=*/stats);

        if (debug) {
            std::cout << "[IDA*] iter=" << iteration
                      << " threshold=" << threshold
                      << " expanded=" << stats.expanded
                      << " pruned=" << stats.pruned
                      << std::endl;
        }

        totalExpanded += stats.expanded;
        totalPruned += stats.pruned;

        if (t == kFound) {
            if (debug) {
                std::cout << "[IDA*] solved"
                          << " iters=" << (iteration + 1)
                          << " final_threshold=" << threshold
                          << " total_expanded=" << totalExpanded
                          << " total_pruned=" << totalPruned
                          << std::endl;
            }
            // Found: `solution` is start->goal inclusive.
            return solution;
        }

        if (t == std::numeric_limits<int>::max()) {
            if (debug) {
                std::cout << "[IDA*] no solution"
                          << " iters=" << (iteration + 1)
                          << " last_threshold=" << threshold
                          << " total_expanded=" << totalExpanded
                          << " total_pruned=" << totalPruned
                          << std::endl;
            }
            // No candidate for a higher threshold => no solution reachable.
            return {};
        }

        // Raise the threshold to the smallest f-cost that exceeded the old bound.
        threshold = t;
        ++iteration;
    }
}

std::vector<State> IDAStar::solve() {
    return solve(false);
}

int IDAStar::dfsBounded(const State &current,
                        int g,
                        int threshold,
                        std::vector<State> &path,
                        std::vector<State> &outSolution,
                        bool debug,
                        IterationStats &stats) {
    (void)debug; // debug printing is handled at the iteration level

    // Compute f = g + h and prune if it exceeds the threshold.
    const int h = heuristic(current);
    const int f = g + h;
    if (f > threshold) {
        // Pruned by the current bound.
        ++stats.pruned;

        // Return the f-cost that caused the cutoff; caller uses min of these
        // to determine the next threshold.
        return f;
    }

    // Goal test.
    if (current.IsGoal(problem.n)) {
        // Success: copy the current path (which already includes `current`)
        // into `outSolution`.
        outSolution = path;
        return kFound;
    }

    int minExcess = std::numeric_limits<int>::max();

    // We're about to generate successors from this node.
    ++stats.expanded;

    // Enumerate legal moves and DFS into children.
    Move moves[4];
    const uint32_t moveCount = GetMoves(problem, current, moves);

    for (uint32_t i = 0; i < moveCount; ++i) {
        const Move m = moves[i];
        State child = ApplyMove(problem, current, m);

        // Avoid cycles by not revisiting a state already on the current path.
        if (isOnPath(child, path)) {
            continue;
        }

        // Extend path with this child.
        path.push_back(child);

        // Recurse. Each action costs 1, so g increases by 1.
        int t = dfsBounded(/*current=*/child,
                           /*g=*/g + 1,
                           /*threshold=*/threshold,
                           /*path=*/path,
                           /*outSolution=*/outSolution,
                           /*debug=*/debug,
                           /*stats=*/stats);

        if (t == kFound) {
            return kFound;
        }

        if (t < minExcess) {
            minExcess = t;
        }

        // Backtrack.
        path.pop_back();
    }

    return minExcess;
}

bool IDAStar::isOnPath(const State &s, const std::vector<State> &path) const {
    for (const State &ancestor : path) {
        if (ancestor == s) {
            return true;
        }
    }
    return false;
}
