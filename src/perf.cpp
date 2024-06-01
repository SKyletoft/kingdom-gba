#include "perf.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <tuple>

extern "C" {
#include <tonc_memmap.h>
#include <tonc_types.h>
}

namespace perf {

size_t VBLANK_COUNT = 0;
size_t MAIN_LOOP_COUNT = 0;
u32 TOTAL_FINISH_LINE = 0;
std::array<u32, 16> LAST_16_FINISHES = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void record_frame() {
	u16 scanline = REG_VCOUNT;
	LAST_16_FINISHES[MAIN_LOOP_COUNT % 16] = scanline * 1024;
	TOTAL_FINISH_LINE += scanline;

	MAIN_LOOP_COUNT++;
}

std::tuple<u32, u32> get_perf_data() {
	u32 total = TOTAL_FINISH_LINE / MAIN_LOOP_COUNT;
	u32 last_16 =
		*std::ranges::fold_left_first(LAST_16_FINISHES, std::plus{}) / 16;

	return std::tuple{total, last_16};
}

void count_frame() { VBLANK_COUNT++; }

size_t get_frame() { return VBLANK_COUNT; }

} // namespace perf
