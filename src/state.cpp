#include "state.h"

#include <cstddef>

namespace state {

size_t current_state = 0;
size_t next_state = 0;
size_t last_state = 0;
bool blacked_out = true;

bool Mode::blackout() { return true; }

} // namespace state
