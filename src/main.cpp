#include "audio.h"
#include "debug.h"
#include "input.h"
#include "interrupts.h"
#include "perf.h"
#include "state.h"
#include "tty.h"
#include "util.h"

#include <span>

namespace config {
extern std::span<state::Mode *const> modes;
extern u32 the_startup_song;
} // namespace config

namespace debug {
extern tty::TtyMode tty_mode;
}

extern "C" [[noreturn]] void _exit(int) {
	debug::tty_mode.restore();
	debug::tty_mode.clear();
	debug::tty_mode.println("Crashed!");

	util::spin();
}

void initialise() {
	interrupts::initialise();
	audio::initialise();
}

int main() {
	debug::println("Initialising...");

	initialise();
	audio::play_song(config::the_startup_song);
	audio::set_bgm_volume(768);

	state::next_state = 3;
	state::current_state = state::next_state;

	config::modes[state::current_state]->restore();

	for (;;) {
		if (state::current_state != state::next_state) {
			util::wait_for_vsync();
			state::blacked_out = false;
			if (config::modes[state::current_state]->blackout()
				|| config::modes[state::next_state]->blackout())
			{
				state::blacked_out = true;
				util::set_screen_to_black();
			}
			config::modes[state::current_state]->suspend();
			state::last_state = state::current_state;
			state::current_state = state::next_state;
			util::wait_for_vsync();
			config::modes[state::current_state]->restore();
		}

		input::poll();
		config::modes[state::current_state]->update();
		for (auto mode : config::modes) {
			if (mode == nullptr) {
				continue;
			}
			mode->always_update();
		}

		perf::record_frame();
		util::wait_for_vsync();
		config::modes[state::current_state]->vsync_hook();
	}

	util::spin();
	return 0;
}
