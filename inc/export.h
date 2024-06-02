#pragma once

#include <span>

extern "C" {
#include <tonc_types.h>
}

#define EXPORT(list, song)                                                     \
	std::span<state::Mode *const> MODES{list};                                 \
	u32 THE_STARTUP_SONG = (u32)song;
