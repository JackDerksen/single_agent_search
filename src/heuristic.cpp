#include "heuristic.h"
#include "util.h"
#include <cmath>
#include <cstdint>

uint32_t MisplacedDiscCount(const State& s, uint32_t startIdx, uint32_t n)
{
	uint32_t misplaced = 0;
    for (uint32_t i = 0; i < n*n; i++)
    {
		uint32_t index = (startIdx + 1 + i) % (int)s.small.size();
		uint32_t expected = (i / n) + 1;
		if (s.small[index] != expected)
			misplaced++;
    }
	return misplaced;
}

int MisplacedDiscHeuristic::operator()(const State& s) const
{
	uint32_t n = floor(std::sqrt(s.small.size())) + 1;
	uint32_t smallestCount = 0;
	for (uint32_t i = 0; i < s.small.size(); i++)
	{
		uint32_t count = MisplacedDiscCount(s, i, n);
		if (count < smallestCount) { smallestCount = count; }
	}
    return smallestCount;
}

typedef struct
{
	size_t neighbors[4];
} DiskMoves;

void CreateHopMatrix(std::vector<std::vector<int>> & matrix, const Problem& p)
{ 
	size_t n = p.large.size(); // Number of disks.

	// Resize matrix for problem.
	matrix.resize(n);
	for (std::vector<int> & row : matrix) { row.resize(n); }

	std::vector<DiskMoves> moves(n);
	for (size_t i = 0; i < moves.size(); i++)
	{
		Disk large = p.large[i];
		DiskMoves m = {
			WrapIndex(i - 1, n),
			WrapIndex(i + 1, n),
			WrapIndex(i - large, n),
			WrapIndex(i + large, n)
		};
		moves[i] = m;
	}

	// Do a breadth first search to get distances to all other positions.
	for (size_t startPos = 0; startPos < moves.size(); startPos++)
	{
		std::queue<std::pair<int, int>> queue;
		std::vector<bool> visited(n, false);

		queue.push({startPos, 0});
		visited[startPos] = true;

		while (queue.empty() == false)
		{
			std::pair<int, int> current = queue.front();
			queue.pop();
			int curr = current.first;
			int cost = current.second;

			for (size_t neighbor : moves[curr].neighbors)
			{
				if (!visited[neighbor])
				{
					visited[neighbor] = true;
					queue.push({neighbor, cost + 1});
					matrix[startPos][neighbor] = cost + 1;
				}
			}
		}
	}
}

void GetGoalPositions(std::vector<int> & goals, Disk disk, int zeroIndex, int groupSize)
{
	assert(goals.size() == groupSize);

	int n = groupSize * groupSize + 1; // Number of disks
	int startIndex = groupSize * (disk - 1); // Start index of group (without rotation)
    for (int i = 0; i < groupSize; i++)
    {
		int index = (zeroIndex + 1 + startIndex + i) % n;
		goals[i] = index;
    }
}

int HopHeuristic::operator()(const State& s) const
{
	int n = s.small.size();
	int groups = floor(sqrt((double)s.small.size()));
	std::vector<int> goalPositions(groups);

	int h = std::numeric_limits<int>::max();
	// Calculate h for every goalstate rotation.
	for (int i = 0; i < n; i++)
	{
		int rotationH = 0;
		// Calculate least number of hops to a goal position for each disc.
		for (int start = 0; start < n; start++)
		{
			Disk disk = s.small[start];
			if (disk == 0) { continue; } // Skip empty space.
			GetGoalPositions(goalPositions, s.small[start], i, groups);
			int leastNumHops = std::numeric_limits<int>::max();
			for (int dest : goalPositions)
			{
				int hops = hopMatrix[start][dest];
				if (hops < leastNumHops) { leastNumHops = hops; }
			}
			rotationH += leastNumHops;
		}
		if (rotationH < h) { h = rotationH;  }
	}

	return h;
}

// Initialize the hop matrix.
void HopHeuristic::initialize(const Problem& p)
{
	CreateHopMatrix(hopMatrix, p);
}
