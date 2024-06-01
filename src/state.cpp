#include "state.h"

#include <cstddef>

namespace state {

size_t CURRENT_STATE = 0;
size_t NEXT_STATE = 0;
size_t LAST_STATE = 0;
bool BLACKED_OUT = true;

bool Mode::blackout() { return true; }

} // namespace state
