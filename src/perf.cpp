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

size_t vblank_count = 0;
size_t main_loop_count = 0;
u32 total_finish_line = 0;
std::array<u32, 16> last_16_finishes = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void record_frame() {
	u16 scanline = REG_VCOUNT;
	last_16_finishes[main_loop_count % 16] = scanline * 1024;
	total_finish_line += scanline;

	main_loop_count++;
}

std::tuple<u32, u32> get_perf_data() {
	u32 total = total_finish_line / main_loop_count;
	u32 last_16 =
		*std::ranges::fold_left_first(last_16_finishes, std::plus{}) / 16;

	return std::tuple{total, last_16};
}

void count_frame() { vblank_count++; }

size_t get_frame() { return vblank_count; }

} // namespace perf
