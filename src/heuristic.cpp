#include "heuristic.h"
#include <cmath>
#include <cstdint>

uint32_t MisplacedTileCount(const State& s, uint32_t startIdx, uint32_t n)
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

int SlowHeuristic::operator()(const State& s) const
{
	uint32_t n = floor(std::sqrt(s.small.size())) + 1;
	uint32_t largestCount = 0;
	for (uint32_t i = 0; i < s.small.size(); i++)
	{
		uint32_t count = MisplacedTileCount(s, i, n);
		if (count > largestCount) { largestCount = count; }
	}
    return largestCount;
}
