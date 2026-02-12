#include "heuristic.h"
#include "util.h"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

// For a valid state, `small.size()` should be `n*n + 1` (including the zero).
// Recover `n` robustly from `small.size()`. Returns 0 if it can't be inferred.
uint32_t InferGroupSizeFromState(const State& s) {
    const std::size_t total = s.small.size();
    if (total == 0) return 0;

    const double nApprox = std::sqrt(static_cast<double>(total - 1));
    const uint32_t n = static_cast<uint32_t>(std::lround(nApprox));
    if (static_cast<std::size_t>(n) * static_cast<std::size_t>(n) + 1 != total) {
        return 0;
    }
    return n;
}

// Count how many discs are misplaced if we treat `startIdx` as the reference
// index (ie. the position of zero is at `startIdx` for the purpose of rotation).
uint32_t MisplacedDiscCountForRotation(const State& s, uint32_t startIdx, uint32_t groupSize) {
    const std::size_t total = s.small.size();
    assert(total > 0);
    assert(groupSize > 0);

    const uint32_t goalCount = groupSize * groupSize; // number of non-zero discs
    uint32_t misplaced = 0;

    for (uint32_t i = 0; i < goalCount; ++i) {
        const uint32_t index = static_cast<uint32_t>((static_cast<std::size_t>(startIdx) + 1 + i) % total);
        const uint32_t expected = (i / groupSize) + 1;
        if (s.small[index] != expected) {
            ++misplaced;
        }
    }

    return misplaced;
}

struct DiskMoves {
    std::size_t neighbors[4];
};

void CreateHopMatrix(std::vector<std::vector<int>>& matrix, const Problem& p) {
    const std::size_t n = p.large.size(); // number of positions/disks
    matrix.assign(n, std::vector<int>(n, 0));

    std::vector<DiskMoves> moves(n);
    for (std::size_t i = 0; i < moves.size(); ++i) {
        const Disk large = p.large[i];
        moves[i] = DiskMoves{
            static_cast<std::size_t>(WrapIndex(static_cast<int>(i) - 1, static_cast<int>(n))),
            static_cast<std::size_t>(WrapIndex(static_cast<int>(i) + 1, static_cast<int>(n))),
            static_cast<std::size_t>(WrapIndex(static_cast<int>(i) - static_cast<int>(large), static_cast<int>(n))),
            static_cast<std::size_t>(WrapIndex(static_cast<int>(i) + static_cast<int>(large), static_cast<int>(n))),
        };
    }

    // BFS from each start position to compute shortest hop distances.
    for (std::size_t startPos = 0; startPos < moves.size(); ++startPos) {
        std::queue<std::pair<std::size_t, int>> q;
        std::vector<bool> visited(n, false);

        q.push({startPos, 0});
        visited[startPos] = true;
        matrix[startPos][startPos] = 0;

        while (!q.empty()) {
            const auto current = q.front();
            q.pop();

            const std::size_t curr = current.first;
            const int cost = current.second;

            for (std::size_t neighbor : moves[curr].neighbors) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    matrix[startPos][neighbor] = cost + 1;
                    q.push({neighbor, cost + 1});
                }
            }
        }
    }
}

void GetGoalPositions(std::vector<int>& goals, Disk disk, int zeroIndex, int groupSize) {
    assert(static_cast<int>(goals.size()) == groupSize);
    assert(groupSize > 0);

    const int total = groupSize * groupSize + 1;                     // total positions
    const int startIndex = groupSize * (static_cast<int>(disk) - 1); // start of disk's goal group (unrotated)

    for (int i = 0; i < groupSize; ++i) {
        const int index = (zeroIndex + 1 + startIndex + i) % total;
        goals[i] = index;
    }
}

} // namespace

uint32_t MisplacedDiscCount(const State& s, uint32_t startIdx, uint32_t n) {
    // Public helper
    return MisplacedDiscCountForRotation(s, startIdx, n);
}

int MisplacedDiscHeuristic::operator()(const State& s) const {
    const uint32_t groupSize = InferGroupSizeFromState(s);
    assert(groupSize != 0);

    uint32_t best = std::numeric_limits<uint32_t>::max();
    for (uint32_t i = 0; i < s.small.size(); ++i) {
        const uint32_t count = MisplacedDiscCountForRotation(s, i, groupSize);
        if (count < best) best = count;
    }
    return static_cast<int>(best);
}

int HopHeuristic::operator()(const State& s) const {
    const int total = static_cast<int>(s.small.size());
    const uint32_t groupSizeU = InferGroupSizeFromState(s);
    assert(groupSizeU != 0);

    const int groupSize = static_cast<int>(groupSizeU);
    std::vector<int> goalPositions(groupSize);

    int bestH = std::numeric_limits<int>::max();

    // Calculate h for every goal-state rotation.
    for (int zeroRotation = 0; zeroRotation < total; ++zeroRotation) {
        int rotationH = 0;

        // For each disc, add the min hops to any of its goal positions.
        for (int start = 0; start < total; ++start) {
            const Disk disk = s.small[static_cast<std::size_t>(start)];
            if (disk == 0) continue;

            GetGoalPositions(goalPositions, disk, zeroRotation, groupSize);

            int leastNumHops = std::numeric_limits<int>::max();
            for (int dest : goalPositions) {
                const int hops = hopMatrix[static_cast<std::size_t>(start)][static_cast<std::size_t>(dest)];
                if (hops < leastNumHops) leastNumHops = hops;
            }

            rotationH += leastNumHops;
        }

        if (rotationH < bestH) bestH = rotationH;
    }

    return bestH;
}

void HopHeuristic::initialize(const Problem& p) {
    CreateHopMatrix(hopMatrix, p);
}
