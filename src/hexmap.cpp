#include "hexmap.h"

#include "debug.h"
#include "hexes.h"
#include "tiles.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>

extern "C" {
#include "complete.h"
}

#define memcpy_(dest, src) std::memcpy((void *)dest, src, sizeof(src))

namespace hexmap {

using tiles::BG_PALETTE_MEMORY;
using tiles::CHARBLOCKS;
using tiles::Palette;

constexpr u16 TILE_SIZE = 9;

void Hexmap::load_tilesets(Layer &layer) {
	debug::printf("Loading tilesets: 0x%X\n", layer.tile_source);
	memcpy_(&CHARBLOCKS[layer.tile_source][1], completeTiles);
	CHARBLOCKS[layer.tile_source][0] = STile{.index_octs = {0}};
}

void Hexmap::load_palettes(Layer &) {
	BG_PALETTE_MEMORY[2] = *(Palette *)completePal;
}

std::pair<hexes::OffsetXYCoord, Point<s16>>
Hexmap::grid_coord(Layer &layer, s16 x, s16 y) {
	size_t tile_x;
	size_t tile_y;

	if (layer != this->layer0) {
		x -= 2;
		y -= 2;

		tile_x = (size_t)x / 3;
		tile_y = (size_t)(y / 4) * 2 + 1;
	} else {
		tile_x = (size_t)x / 3;
		tile_y = (size_t)(y / 4) * 2;
	}
	return {hexes::OffsetXYCoord{(s16)tile_x, (s16)tile_y}, {x, y}};
}

ScreenEntry Hexmap::get_tile(Layer &layer, s16 x, s16 y) {
	auto [offset, x_y] = this->grid_coord(layer, x, y);
	x = x_y.x;
	y = x_y.y;

	u16 index = 0;
	u8 palette = 2;
	if (offset.col < (s16)this->map.width - (offset.row & 1)
		&& offset.row < (s16)this->map.height)
	{
		u8 tile_type = this->map[(size_t)offset.row, (size_t)offset.col];
		u16 tile_index = (u16)((y % 4) * 3 + (x % 3));
		u16 hex_index = tile_type * TILE_SIZE;
		index = (u16)(tile_index + hex_index + 1);

		if (tile_index > TILE_SIZE - 1) {
			index = 0;
		}
	}

	return ScreenEntry(index, 0, palette);
}

void SimpleHexmap::load_tilesets(Layer &layer) {
	std::memcpy(
		CHARBLOCKS[layer.tile_source],
		this->tiles.data(),
		this->tiles.size_bytes()
	);
}

void SimpleHexmap::load_palettes(Layer &layer) {
	std::memcpy(
		BG_PALETTE_MEMORY.data(),
		this->palettes.data(),
		this->palettes.size_bytes()
	);
}

} // namespace hexmap
