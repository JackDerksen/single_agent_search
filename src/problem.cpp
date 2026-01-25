#include "problem.h"

Problem::Problem(const std::vector<Disk>& large_disks, const std::vector<Disk>& small_disks, uint16_t n): large(large_disks), n(n)
{
	smallState.small = small_disks;
	smallState.zeroIndex = smallState.GetZeroIndex();
}
