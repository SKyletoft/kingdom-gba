#include "state.h"
#include "export.h"

#include <span>
#include <array>

namespace config {

std::array<state::Mode *, 1> MODES_RAW = {
};
u32 DEFAULT_SONG = 0;

EXPORT(MODES_RAW, DEFAULT_SONG);

}
