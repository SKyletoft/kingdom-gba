#pragma once

#include <tuple>

extern "C" {
#include <tonc_memmap.h>
#include <tonc_types.h>
}

namespace perf {

void record_frame();

// Total, Rolling, in 1024ths of a scanline
std::tuple<u32, u32> get_perf_data();

void count_frame();

std::size_t get_frame();

} // namespace perf
