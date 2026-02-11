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

static inline bool IsInverseMove(Move a, Move b)
{
	// Returns true iff b immediately undoes a
	return (a == SWAP_RIGHT   && b == SWAP_LEFT) ||
		   (a == SWAP_LEFT    && b == SWAP_RIGHT) ||
		   (a == SWAP_N_RIGHT && b == SWAP_N_LEFT) ||
		   (a == SWAP_N_LEFT  && b == SWAP_N_RIGHT);
}

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

	Node node;
	node.state = state;
	node.g = parent->g + 1;
	node.h = heuristic(state);
	node.lastMove = move;
	node.parent = parent;
	return node;
}

uint32_t ExpandNode(Node * n, Node * outChildren, const std::vector<Disk> & large, const Heuristic & h)
{
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

uint32_t ExpandNodeNoUndo(Node * n, Node * outChildren, const std::vector<Disk> & large, const Heuristic & h, Move lastMove, bool hasLastMove)
{
	uint32_t largeValue = large[n->state.zeroIndex];

	Move candidates[4] = { SWAP_RIGHT, SWAP_LEFT, SWAP_N_RIGHT, SWAP_N_LEFT };
	uint32_t candidateCount = (largeValue != 1) ? 4u : 2u;

	uint32_t outCount = 0;
	for (uint32_t i = 0; i < candidateCount; i++)
	{
		Move m = candidates[i];
		if (hasLastMove && IsInverseMove(lastMove, m))
			continue;

		outChildren[outCount++] = GenerateMoveNode(n, m, largeValue, h);
	}

	return outCount;
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

	Move lastMove = SWAP_RIGHT;
	bool hasLastMove = false;
	if (path.size() >= 2)
	{
		const Node &parent = path[path.size() - 2];
		const Node &curr = path[path.size() - 1];

		int diff = (int)curr.state.zeroIndex - (int)parent.state.zeroIndex;
		int numDisks = (int)parent.state.small.size();
		diff = WrapIndex(diff, numDisks);

		// Determine which move was taken from parent -> current by examining the zeroIndex delta.
		// This is enough to identify the immediate inverse move to skip
		int largeValue = (int)problem.large[parent.state.zeroIndex];
		if (diff == WrapIndex( largeValue, numDisks)) { lastMove = SWAP_RIGHT; hasLastMove = true; }
		else if (diff == WrapIndex(-largeValue, numDisks)) { lastMove = SWAP_LEFT; hasLastMove = true; }
		else if (diff == WrapIndex( 1, numDisks)) { lastMove = SWAP_N_RIGHT; hasLastMove = true; }
		else if (diff == WrapIndex(-1, numDisks)) { lastMove = SWAP_N_LEFT; hasLastMove = true; }
	}

	uint16_t childCount = (uint16_t)ExpandNodeNoUndo(&current, children, problem.large, heuristic, lastMove, hasLastMove);

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

	Node startNode;
	startNode.state = startState;
	startNode.g = 0;
	startNode.h = heuristic(startState);
	startNode.lastMove = SWAP_RIGHT;
	startNode.parent = NULL;

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
