#include "astar.h"
#include "problem.h"
#include "state.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

AStar::AStar(const Problem &p, const Heuristic &h)
	  : problem(p)
{
}

bool CompareNodes(const Node* a, const Node* b)
{
	return a->f() > b->f();
}

uint32_t ExpandNode(const Node * n, Node * outChildren, const std::vector<Disk> & large)
{
	// NOTE(rordon): it would be nice if we could track what the last move was
	// 				 and not add nodes that undo the last move to save time. 
	
	uint32_t largeValue = large[n->state.zeroIndex];
	if (largeValue == 1)
	{
		// Add swap left and right states
		return 2;
	}
	else
	{
		// Add swap n right and left states.
		return 4;
	}
}

std::vector<State> AStar::solve()
{
	std::priority_queue<Node*, std::vector<Node*>, bool(*)(const Node*, const Node*)> frontier(CompareNodes);
	std::unordered_map<State, Node*, StateHash> closed;

	State startState = problem.small;
	Node start = {.state = startState, .g = 0, .h = 0, .parent = NULL};
	frontier.push(&start);

	while (!frontier.empty())
	{
		Node * n = frontier.top();
		frontier.pop();
		if (n->state.IsGoal())
		{
			// return path.
		}
		else
		{
			closed[n->state] = n; 

			// Expand node
			Node children[4];
			uint16_t childCount = ExpandNode(n, children, problem.large);
			
			// Check if children are in closed set.
		}
	}


    return {};
}
