#include "state.h"

bool State::operator==(const State &other) const {
    return large == other.large && small == other.small;
}
