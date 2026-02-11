#include "astar.h"
#include "problem.h"
#include "state.h"
#include "common.h"
#include "node.h"
#include <cstdint>
#include <limits>
#include <unordered_set>

AStar::AStar(const Problem &p, Heuristic &h): problem(p), heuristic(h)
{
	heuristic.initialize(problem);
}

int WrapIndex(int i, int n) { return (i % n + n) % n; }

Node GenerateMoveNode(Node * parent, Move move, uint32_t largeValue, const Heuristic & heuristic)
{
	State state = {parent->state.small};

	uint32_t numDisks = parent->state.small.size();
	switch (move)
	{
		case SWAP_RIGHT:
			state.zeroIndex = WrapIndex(parent->state.zeroIndex + largeValue, numDisks);
			break;
		case SWAP_LEFT:
			state.zeroIndex = WrapIndex(parent->state.zeroIndex - largeValue, numDisks);
			break;
		case SWAP_N_RIGHT:
			state.zeroIndex = WrapIndex(parent->state.zeroIndex + 1, numDisks);
			break;
		case SWAP_N_LEFT:
			state.zeroIndex = WrapIndex(parent->state.zeroIndex - 1, numDisks);
			break;
		default: assert(false);
	}

	// Swap values
	state.small[parent->state.zeroIndex] = state.small[state.zeroIndex];
	state.small[state.zeroIndex] = 0;

	Node node = {state,  parent->g + 1, heuristic(state), parent};
	return node;
}

uint32_t ExpandNode(Node * n, Node * outChildren, const std::vector<Disk> & large, const Heuristic & h)
{
	// NOTE(rordon): it would be nice if we could track what the last move was
	// 				 and not add nodes that undo the last move to save time.

	uint32_t largeValue = large[n->state.zeroIndex];

	// Add swap left and swap right states
	outChildren[0] = GenerateMoveNode(n, SWAP_RIGHT, largeValue, h);
	outChildren[1] = GenerateMoveNode(n, SWAP_LEFT,  largeValue, h);

	if (largeValue != 1)
	{
		// Add swap n left and right states
		outChildren[2] = GenerateMoveNode(n, SWAP_N_RIGHT, largeValue, h);
		outChildren[3] = GenerateMoveNode(n, SWAP_N_LEFT,  largeValue, h);
		return 4;
	}

	return 2;
}

static std::vector<State> ReconstructSolutionFromPath(const std::vector<Node> &path)
{
	std::vector<State> solution;
	solution.reserve(path.size());
	for (const Node &n : path) { solution.push_back(n.state); }
	return solution;
}

// Returns either:
// - found==true and solution populated, or
// - found==false and the next f-bound to try (minNextBound)
struct IDAResult
{
	bool found = false;
	int minNextBound = std::numeric_limits<int>::max();
	std::vector<State> solution;
};

static IDAResult IDAStarDfs(
	const Problem &problem,
	const Heuristic &heuristic,
	std::vector<Node> &path,
	std::unordered_set<State, StateHash> &onPath,
	int bound,
	bool debug,
	uint64_t &expanded)
{
	Node &current = path.back();

	// f = g + h
	int f = current.g + current.h;
	if (f > bound)
	{
		IDAResult r;
		r.found = false;
		r.minNextBound = f;
		return r;
	}

	if (current.state.IsGoal(problem.n))
	{
		IDAResult r;
		r.found = true;
		r.solution = ReconstructSolutionFromPath(path);
		return r;
	}

	expanded++;

	Node children[4];
	uint16_t childCount = ExpandNode(&current, children, problem.large, heuristic);

	int minNext = std::numeric_limits<int>::max();

	for (uint16_t i = 0; i < childCount; i++)
	{
		Node child = children[i];

		// Avoid cycles along current DFS path
		if (onPath.find(child.state) != onPath.end())
			continue;

		// Push
		path.push_back(child);
		onPath.insert(child.state);

		IDAResult r = IDAStarDfs(problem, heuristic, path, onPath, bound, debug, expanded);
		if (r.found)
			return r;

		if (r.minNextBound < minNext)
			minNext = r.minNextBound;

		// Pop
		onPath.erase(child.state);
		path.pop_back();
	}

	IDAResult out;
	out.found = false;
	out.minNextBound = minNext;
	return out;
}

// IDA* keeps memory bounded (O(depth)) while still leveraging the existing heuristic (as an admissible f-bound)
std::vector<State> AStar::solve(bool debug)
{
	State startState = problem.smallState;

	assert(startState.small.size() > 0);

	Node startNode{startState, 0, heuristic(startState), NULL};

	std::vector<Node> path;
	path.reserve(1024);
	path.push_back(startNode);

	std::unordered_set<State, StateHash> onPath;
	onPath.insert(startState);

	int bound = startNode.g + startNode.h;

	uint64_t expanded = 0;
	while (true)
	{
		IDAResult r = IDAStarDfs(problem, heuristic, path, onPath, bound, debug, expanded);
		if (r.found)
		{
			if (debug)
			{
				std::cout << "Expanded nodes: " << expanded << std::endl;
				std::cout << "Moves to solve: " << r.solution.size() << std::endl;
			}
			return r.solution;
		}

		// No solution within this bound, so increase to the smallest f that exceeded it
		if (r.minNextBound == std::numeric_limits<int>::max())
		{
			// Search space exhausted (should not happen for a solvable instance)
			return {};
		}

		bound = r.minNextBound;

		// The DFS fully unwinds back to the root before returning, so path
		// should be back to just the start node.
		assert(path.size() == 1);
		assert(onPath.size() == 1);
	}
}
