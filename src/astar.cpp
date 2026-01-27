#include "astar.h"
#include "problem.h"
#include "state.h"
#include "common.h"
#include <cstdint>
#include <execution>

AStar::AStar(const Problem &p, const Heuristic &h)
	  : problem(p)
{
}

bool CompareNodes(const Node* a, const Node* b) { return a->f() > b->f(); }

int WrapIndex(int i, int n) { return (i % n + n) % n; }

Node GenerateMoveNode(Node * parent, Move move, uint32_t largeValue)
{
	State state = {parent->state.small, 100000}; // 100000 so itll crash if index isnt set right

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
	}

	// Swap values
	state.small[parent->state.zeroIndex] = state.small[state.zeroIndex];
	state.small[state.zeroIndex] = 0;

	// TODO: Calculate Heuristic for state.
	Node node = {.state = state, .g = parent->g + 1, .h = 0, .parent = parent};
	return node;
}

uint32_t ExpandNode(Node * n, Node * outChildren, const std::vector<Disk> & large)
{
	// NOTE(rordon): it would be nice if we could track what the last move was
	// 				 and not add nodes that undo the last move to save time. 
	
	uint32_t largeValue = large[n->state.zeroIndex];
	
	// Add swap left and swap right states
	outChildren[0] = GenerateMoveNode(n, SWAP_RIGHT, largeValue);
	outChildren[1] = GenerateMoveNode(n, SWAP_LEFT,  largeValue);

	if (largeValue != 1)
	{
		// Add swap n left and right states
		outChildren[2] = GenerateMoveNode(n, SWAP_N_RIGHT, largeValue);
		outChildren[3] = GenerateMoveNode(n, SWAP_N_LEFT,  largeValue);
		return 4;
	}

	return 2;
}

std::vector<State> AStar::solve()
{
	std::priority_queue<Node*, std::vector<Node*>, bool(*)(const Node*, const Node*)> frontier(CompareNodes);
	std::unordered_map<State, Node*, StateHash> closed;
	std::vector<Node> nodes;
	nodes.reserve(1024);

	State startState = problem.small;
	nodes.push_back((Node){.state = startState, .g = 0, .h = 0, .parent = NULL});
	frontier.push(&nodes[0]);

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
			
			for (int i = 0; i < childCount; i++)
			{
				Node * child = &children[i];
				// Check if child is in closed set.
				auto it = closed.find(child->state); // hate this sm
				if (it != closed.end())
				{
					// Check if cost to node is less
					Node * existingChild = it->second;
					if (child->g < existingChild->g)
					{
						existingChild->g = child->g;
						existingChild->parent = n;
						frontier.push(existingChild);
					}
				}
				else
				{
					nodes.push_back(*child);
					frontier.push(&nodes.back());
				}
			}
		}
	}


    return {};
}
