#include "heuristic.h"
#include <cmath>
#include <cstdint>

uint32_t MisplacedDiscCount(const State& s, uint32_t startIdx, uint32_t n)
{
	uint32_t misplaced = 0;
    for (uint32_t i = 0; i < n*n; i++)
    {
		uint32_t index = (startIdx+ 1 + i) % (int)s.small.size();
		uint32_t expected = (i / n) + 1;
		if (s.small[index] != expected)
			misplaced++;
    }
	return misplaced;
}

uint32_t CalculateHops(size_t startPos, size_t destPos, const Problem& p)
{ 
	// TODO
	return 0;
}

int MisplacedDiscHeuristic::operator()(const State& s) const
{
	uint32_t n = floor(std::sqrt(s.small.size())) + 1;
	uint32_t largestCount = 0;
	for (uint32_t i = 0; i < s.small.size(); i++)
	{
		uint32_t count = MisplacedDiscCount(s, i, n);
		if (count > largestCount) { largestCount = count; }
	}
    return largestCount;
}

int HopHeuristic::operator()(const State& s) const
{
	// for state in goalstates 
	//	for each disc
	//		get nearest goal position
	//		get num hops to nearest position
	//		add to sum
	//	store sum for goalstate
	// return lowest sum for goalstates
	return 0;
}

// Initialize the hop matrix and generate goal states.
void HopHeuristic::initialize(const Problem& p)
{
	size_t numDisks = (p.n * p.n) + 1;

	// TODO: Generate goal states.

	hopMatrix.resize(numDisks);
	for (std::vector<int> & row : hopMatrix)
	{
		row.resize(numDisks);
	}

	for (size_t i = 0; i < hopMatrix.size(); i++)
	{
		for (size_t j = 0; j< hopMatrix.size(); j++)
		{
			CalculateHops(i, j, p);
		}
	}
}
