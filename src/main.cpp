#include "debug.h"
#include "input.h"
#include "interrupts.h"
#include "perf.h"
#include "state.h"
#include "tty.h"
#include "util.h"

#include <span>

namespace config {
extern std::span<state::Mode *const> MODES;
extern u32 THE_STARTUP_SONG;
} // namespace config

namespace debug {
extern tty::TtyMode TTY_MODE;
}

extern "C" [[noreturn]] void _exit(int) {
	debug::TTY_MODE.restore();
	debug::TTY_MODE.clear();
	debug::TTY_MODE.println("Crashed!");

	util::spin();
}

void initialise() {
	interrupts::initialise();
}

int main() {
	debug::println("Initialising...");

	initialise();

	state::NEXT_STATE = 0;
	state::CURRENT_STATE = state::NEXT_STATE;

	config::MODES[state::CURRENT_STATE]->restore();

	for (;;) {
		if (state::CURRENT_STATE != state::NEXT_STATE) {
			util::wait_for_vsync();
			state::BLACKED_OUT = false;
			if (config::MODES[state::CURRENT_STATE]->blackout()
				|| config::MODES[state::NEXT_STATE]->blackout())
			{
				state::BLACKED_OUT = true;
				util::set_screen_to_black();
			}
			config::MODES[state::CURRENT_STATE]->suspend();
			state::LAST_STATE = state::CURRENT_STATE;
			state::CURRENT_STATE = state::NEXT_STATE;
			util::wait_for_vsync();
			config::MODES[state::CURRENT_STATE]->restore();
		}

		input::poll();
		config::MODES[state::CURRENT_STATE]->update();
		for (auto mode : config::MODES) {
			if (mode == nullptr) {
				continue;
			}
			mode->always_update();
		}

		perf::record_frame();
		util::wait_for_vsync();
		config::MODES[state::CURRENT_STATE]->vsync_hook();
	}

	util::spin();
	return 0;
}
