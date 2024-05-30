#include "util.h"
#include "tiles.h"

extern "C" {
#include <maxmod.h>
#include <tonc_bios.h>
#include <tonc_memmap.h>
}

namespace util {

// An infinite loop without a sideeffect (like setting a global
// variable) is UB and may be optimised away by the compiler.
// Returning from main might actually be fine and handled by the
// runtime, but let's not rely on that until we've checked it out
// with gdb
vu32 x;
[[noreturn]] void spin() {
	for (u32 i = 0;; ++i) {
		x = i;
		wait_for_vsync();
	}
}

void sleep(u32 frames) {
	for (; frames != 0; --frames) {
		wait_for_vsync();
	}
}

void clear_layer(size_t screenblock) {
	const tiles::ScreenEntry empty{0, 0, 0};
	std::span<u16> tile_map{(u16 *)tiles::SCREENBLOCKS[screenblock], 1024};
	for (u16 &tile : tile_map) {
		tile = empty;
	}
}

void set_screen_to_black() {
	for (auto &palette : tiles::BG_PALETTE_MEMORY) {
		palette = tiles::BLACK_ON_BLACK;
	}
	REG_DISPCNT = 0;
}

void wait_for_drawing_start() {
	while (REG_VCOUNT >= 160)
		;
}

void wait_for_drawing_complete() { VBlankIntrWait(); }

void wait_for_vsync() { wait_for_drawing_complete(); }

// Progress in the range 0-255
u8 lerp(u8 from, u8 to, s32 progress) {
	s32 base = to;
	s32 range = from - to;
	return (u8)(range * progress / 255 + base);
}

} // namespace util
