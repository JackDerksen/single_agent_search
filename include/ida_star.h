#ifndef IDA_STAR_H
#define IDA_STAR_H

#include "common.h"
#include "problem.h"
#include "state.h"
#include "heuristic.h"

#include <cstdint>
#include <vector>

/**
 * NOTE: I generated these doc comments with Claude.
 *
 * Iterative Deepening A* (IDA*) solver.
 *
 * IDA* performs a series of depth-first searches, each with an increasing
 * cost bound on f(n) = g(n) + h(n). It uses far less memory than A* because
 * it does not maintain a global priority queue / closed set. Instead, it
 * explores paths in a DFS manner and prunes any branch whose f-cost exceeds
 * the current threshold.
 *
 * High-level algorithm:
 *  1) threshold = h(start)
 *  2) Repeat:
 *      - Run a bounded DFS that only expands nodes with f <= threshold
 *      - If goal found: return the path
 *      - Otherwise: set threshold to the minimum f-cost that exceeded the
 *        previous threshold, and repeat
 */
class IDAStar {
  Problem problem;
  Heuristic &heuristic;

  // Stats collected across a single bounded DFS iteration (one threshold pass).
  struct IterationStats {
    uint64_t expanded = 0; // number of nodes where we generated successors
    uint64_t pruned = 0;   // number of nodes cut off because f > threshold
  };

public:
  /**
   * Construct an IDA* solver for a problem instance and heuristic.
   * The heuristic is initialized with the problem (same as A* does).
   */
  IDAStar(const Problem &problem, Heuristic &h);

  /**
   * Solve the problem.
   *
   * If `debug` is true, the solver prints per-iteration IDA* progress to stdout.
   * IDA* runs a sequence of depth-first searches, each with an f-cost bound
   * (threshold) where f = g + h.
   *
   * Expected debug output (conceptually) per iteration:
   *  - iteration index (0, 1, 2, ...)
   *  - current threshold (the f-cost bound used for this DFS pass)
   *  - nodes expanded in this iteration (how many states were actually expanded
   *    with f <= threshold; pruned nodes do not count as expanded)
   *  - next threshold candidate (the minimum f-cost that exceeded the current
   *    threshold, which becomes the threshold for the next iteration)
   *
   * With `HopHeuristic`, you should see thresholds increase in small steps on
   * harder instances, along with the number of expanded nodes per iteration.
   *
   * @returns A vector of states from start to goal (inclusive). Empty if no solution.
   */
  std::vector<State> solve(bool debug);

  /// Convenience overload: run without debug output.
  std::vector<State> solve();

private:
  /**
   * Depth-first search bounded by `threshold`.
   *
   * This is the "core" of IDA*:
   *  - Compute f = g + h
   *  - If f > threshold: prune and report f as a candidate for next threshold
   *  - If at goal: success
   *  - Else: expand successors and recurse
   *
   * @param current     Current state being explored.
   * @param g           Cost so far from start to `current`.
   * @param threshold   Current IDA* f-cost bound.
   * @param path        The states from start to current (used for solution reconstruction).
   * @param outSolution Filled with the solution path when goal is found.
   * @param debug       Whether to collect/print debug stats (implementation-defined).
   * @param stats       Per-iteration counters (expanded/pruned) for this threshold pass.
   *
   * @returns
   *  - If goal found: returns -1 (a sentinel indicating success)
   *  - Otherwise: returns the minimum f-cost that exceeded `threshold` encountered
   *    in this subtree (used as the next threshold).
   */
  int dfsBounded(const State &current,
                 int g,
                 int threshold,
                 std::vector<State> &path,
                 std::vector<State> &outSolution,
                 bool debug,
                 IterationStats &stats);

  /**
   * True if `s` is already present on the current DFS path.
   * Used to prevent trivial cycles without storing a global closed set.
   */
  bool isOnPath(const State &s, const std::vector<State> &path) const;
};

#endif // IDA_STAR_H
