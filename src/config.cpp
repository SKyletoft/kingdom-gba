#include "state.h"

#include <span>
#include <array>

namespace config {

std::array<state::Mode *, 0> MODES_RAW = {};
std::span<state::Mode *const> MODES{MODES_RAW};
u32 THE_STARTUP_SONG = 0;

}
