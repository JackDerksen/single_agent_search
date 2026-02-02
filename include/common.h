#ifndef COMMON_H
#define COMMON_H

// Shared types, aliases, constants, etc.
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <iostream>
#include <ostream>
#include <functional>
#include <cassert>
#include <cstddef>
#include <algorithm>

// Disk type for small and large disks
using Disk = uint16_t;

typedef enum {
	SWAP_RIGHT,
	SWAP_LEFT,
	SWAP_N_RIGHT,
	SWAP_N_LEFT,
} Move;

#endif // COMMON_H
