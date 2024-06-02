#include "state.h"
#include "game.h"
#include "export.h"

#include <span>
#include <array>

namespace config {

game::Game THE_GAME{};

std::array<state::Mode *, 1> MODES_RAW = {
	&THE_GAME
};
u32 DEFAULT_SONG = 0;

EXPORT(MODES_RAW, DEFAULT_SONG);

}
