#include "astar.h"
#include "common.h"
#include "problem.h"

Problem ReadUserInput(size_t n)
{
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

	return Problem(large, small, n);
}

void PrintSolution(std::vector<State> solution)
{
    std::cout << "The solution is:" << std::endl;
    for (State s : solution) { std::cout << s << std::endl; }
}

void Tests()
{
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
        assert(false);
    }
    if (hasher(s1) == hasher(s3)) {
        std::cout << "Error: different states have same hash" << std::endl;
        assert(false);
    }
    std::cout << "Printing s1: " << s1 << std::endl;

    // Test IsGoal Function
    State g1 = { {0, 1, 1, 1, 2, 2, 2, 3, 3, 3}, 0};
    assert(g1.IsGoal(3));

    State g2 = { {2, 2, 0, 1, 1}, 2};
    assert(g2.IsGoal(2));

    State g3 = { {1, 1, 1, 2, 2, 3, 2, 3, 3, 0}, 9};
    assert(!g3.IsGoal(3));
}

int main(int argc, char** argv) {
	
    if (argc < 2)
	{
        std::cerr << "Usage: " << argv[0] << " <n>" << std::endl;
		// TODO: Different compiler flags for building with test input vs stdin.
        // return 1;
    }

	// Read problem from stdin.
    // size_t n = std::stoi(argv[1]);
	// Problem p = ReadUserInput(n);
	
	// Problem from the example solver.
	Problem p = Problem({1,3,4,2,4,3,2,1,1,2,3,4,3,1,4,2,5}, {1,1,1,1,2,2,2,2,3,4,3,3,4,0,4,3,4}, 4);
	
	// NOTE(rordon): Don't try this problem until we have a better heuristic. This one gets up to over 300,000,000 nodes with BFS.
	//Problem p = Problem({1,3,3,4,1,2,1,2,5,4,2,3,3,2,4,1,4}, {0,2,4,1,3,4,2,3,1,4,3,2,1,3,4,1,2}, 4);

    ZeroHeuristic zeroHeuristic;
    AStar solver(p, zeroHeuristic);

    auto solution = solver.solve();
    PrintSolution(solution);
}
