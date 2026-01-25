#include "astar.h"
#include "problem.h"
#include "state.h"
#include <unordered_map>
#include <vector>

AStar::AStar(const Problem &problem, const Heuristic &h)
{

}

bool CompareNodes(const Node* a, const Node* b)
{
	return a->f() > b->f();
}

std::vector<State> AStar::solve()
{
	std::priority_queue<Node*, std::vector<Node*>, bool(*)(const Node*, const Node*)> frontier(CompareNodes);
	std::unordered_map<State, Node*, StateHash> closed;

	State startState = {.small = {1,1,1,3,2,2,3,3,0,2}}; 
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
			
			// Check if children are in closed set.
		}
	}


    return {};
}
