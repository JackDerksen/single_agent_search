#include "astar.h"

int main(int argc, char** argv) {
    size_t n = 3;
    Problem problem(n);
    ZeroHeuristic h;
    AStar solver(problem, h);

    State start_state;
    auto solution = solver.solve(start_state);
    // print_solution(solution);
}
