#include "astar.h"
#include "common.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <n>" << std::endl;
        return 1;
    }
    size_t n = std::stoi(argv[1]);
    size_t num_large = n * n + 1;

    // Read large disks
    std::vector<Disk> large(num_large);
    for (size_t i = 0; i < num_large; ++i) {
        std::cin >> large[i];
    }

    // Read small disks
    std::vector<Disk> small(num_large);
    for (size_t i = 0; i < num_large; ++i) {
        std::cin >> small[i];
    }

    // Everything below is just a sanity test for StateHash (using dummy data).
    // We can delete this later, of course :)
    //
    // If any of you want to test this yourself, I did so with:
    // echo -e "1 2 3 4 5 6 7 8 9 10\n1 2 3 4 5 6 7 8 9 0" | ./bin/AB 3
    State s1;
    s1.small = {4,5};

    State s2 = s1;

    State s3;
    s3.small = {4,5,6};
    StateHash hasher;

    std::cout << "Hash s1: " << hasher(s1) << std::endl;
    std::cout << "Hash s2: " << hasher(s2) << std::endl;
    std::cout << "Hash s3: " << hasher(s3) << std::endl;

    if (hasher(s1) != hasher(s2)) {
        std::cout << "Error: identical states have different hashes" << std::endl;
        return 1;
    }
    if (hasher(s1) == hasher(s3)) {
        std::cout << "Error: different states have same hash" << std::endl;
        return 1;
    }
    std::cout << "Printing s1: " << s1 << std::endl;

    Problem problem(large);
    ZeroHeuristic h;
    AStar solver(problem, h);

    State start_state;
    start_state.small = small;
    auto solution = solver.solve(start_state);

    // print_solution(solution);
}
