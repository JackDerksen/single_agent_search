#include "astar.h"
#include "problem.h"
#include "state.h"
#include "common.h"
#include "util.h"
#include <cstdint>

AStar::AStar(const Problem &p, Heuristic &h): problem(p), heuristic(h)
{
	heuristic.initialize(problem);
}

bool CompareNodes(const Node* a, const Node* b) { return a->f() > b->f(); }

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

std::vector<State> AStar::solve(bool debug)
{
	std::priority_queue<Node*, std::vector<Node*>, bool(*)(const Node*, const Node*)> frontier(CompareNodes);
	std::unordered_map<State, Node*, StateHash> closed;
	std::deque<Node> nodes;

	State startState = problem.smallState;
	nodes.push_back(Node{startState, 0, heuristic(startState), NULL});
	frontier.push(&nodes[0]);

	while (!frontier.empty())
	{
		Node * n = frontier.top();
		frontier.pop();
		if (n->state.IsGoal(problem.n))
		{
			// return path.
			std::vector<State> solution;
			Node* current = n;
			solution.push_back(current->state);
			while (current->parent != NULL)
			{
				current = current->parent;
				solution.push_back(current->state);
			}
			std::reverse(solution.begin(), solution.end());
			if (debug)
			{
				std::cout << "Number of nodes: " <<nodes.size() << std::endl;
				std::cout << "Moves to solve: " << solution.size() << std::endl;
			}
			return solution;
		}
		else
		{
			closed[n->state] = n; 

			// Expand node
			Node children[4];
			uint16_t childCount = ExpandNode(n, children, problem.large, heuristic);
			
			for (int i = 0; i < childCount; i++)
			{
				Node * child = &children[i];
				// Check if child is in closed set.
				auto it = closed.find(child->state);
				if (it != closed.end())
				{
					// Check if cost to new node is less
					Node * existingChild = it->second;
					if (child->g < existingChild->g)
					{
						// Update existing node with new cost and parent.
						existingChild->g = child->g;
						existingChild->parent = n;
						// Reopen node
						frontier.push(existingChild);
					}
				}
				else
				{
					// New node, store and add to frontier.
					nodes.push_back(*child);
					frontier.push(&nodes.back());
				}
			}
		}
	}

    return {};
}
