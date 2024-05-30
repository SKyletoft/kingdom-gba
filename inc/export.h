#pragma once

#include <span>

extern "C" {
#include <tonc_types.h>
}

#define EXPORT(list, song)                                                     \
	std::span<state::Mode *const> modes{list};                                 \
	u32 the_startup_song = (u32)song;
