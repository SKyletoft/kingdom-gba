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

	state::next_state = 3;
	state::current_state = state::next_state;

	config::MODES[state::current_state]->restore();

	for (;;) {
		if (state::current_state != state::next_state) {
			util::wait_for_vsync();
			state::blacked_out = false;
			if (config::MODES[state::current_state]->blackout()
				|| config::MODES[state::next_state]->blackout())
			{
				state::blacked_out = true;
				util::set_screen_to_black();
			}
			config::MODES[state::current_state]->suspend();
			state::last_state = state::current_state;
			state::current_state = state::next_state;
			util::wait_for_vsync();
			config::MODES[state::current_state]->restore();
		}

		input::poll();
		config::MODES[state::current_state]->update();
		for (auto mode : config::MODES) {
			if (mode == nullptr) {
				continue;
			}
			mode->always_update();
		}

		perf::record_frame();
		util::wait_for_vsync();
		config::MODES[state::current_state]->vsync_hook();
	}

	util::spin();
	return 0;
}
