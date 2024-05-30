#include "scrolling_map.h"

#include "input.h"
#include "state.h"
#include "tiles.h"
#include "util.h"

#include <algorithm>
#include <cstring>

extern "C" {
#include <tonc.h>
#include <tonc_memmap.h>
}

#define memcpy_(dest, src) std::memcpy((void *)dest, src, sizeof(src))

namespace scrolling_map {

size_t get_screenblock_offset_from_tiles(s16 x, s16 y) {
	while (x < 0) {
		x += 32;
	}
	while (y < 0) {
		y += 32;
	}
	return ((((size_t)y) % 32) * 32) + (((size_t)x) % 32);
}

size_t get_screenblock_offset_from_camera(s16 x, s16 y) {
	return get_screenblock_offset_from_tiles(x / 8, y / 8);
}

void Layer::move_in_bounds(s16 x, s16 y) {
	this->pos.x = std::clamp((s16)(this->pos.x + x), this->min_x, this->max_x);
	this->pos.y = std::clamp((s16)(this->pos.y + y), this->min_y, this->max_y);
}

void ScrollingMap::move_in_bounds(Point<s16> p) {
	this->move_in_bounds(p.x, p.y);
}

void ScrollingMap::move_in_bounds(s16 x, s16 y) {
	if (x == 0 && y == 0) {
		return;
	}
	this->layer0.move_in_bounds(x, y);
	this->layer1.move_in_bounds(x, y);
}

ScreenEntry ScrollingMap::get_tile_from_camera(Layer &layer, s16 x, s16 y) {
	return this->get_tile(layer, x / 8, y / 8);
}

void ScrollingMap::update_tile_from_camera(
	ScreenEntry volatile *const base, Layer &layer, s16 x, s16 y
) {
	size_t const idx = get_screenblock_offset_from_camera(x, y);
	ScreenEntry const tile = this->get_tile_from_camera(layer, x, y);
	base[idx] = tile;
}

void ScrollingMap::update_tile(
	ScreenEntry volatile *const base, Layer &layer, s16 x, s16 y
) {
	size_t const idx = get_screenblock_offset_from_tiles(x, y);
	ScreenEntry const tile = this->get_tile(layer, x, y);
	base[idx] = tile;
}

void ScrollingMap::recolour(Point<s32> from, Point<s32> to, Layer &layer) {
	ScreenEntry volatile *const base = tiles::SCREENBLOCKS[layer.tile_map];

	for (s16 x = (s16)from.x; x <= to.x; x += 8) {
		for (s16 y = (s16)from.y; y <= to.y; y += 8) {
			this->update_tile(
				base,
				layer,
				(s16)((layer.pos.x + x) / 8),
				(s16)((layer.pos.y + y) / 8)
			);
		}
	}
}

void ScrollingMap::load_map(Layer &layer) {
	// Yes, load one row too many
	this->recolour({0, 0}, {30 * 8, 20 * 8}, layer);
}

void ScrollingMap::update_layer_partial(Layer &layer) {
	ScreenEntry volatile *const base = tiles::SCREENBLOCKS[layer.tile_map];

	s16 const dy = layer.pos.y - layer.last_load_at_y;
	if (dy != 0 || layer.update_top != 0) {
		s16 const diff = dy < 0 ? -4 : 164;
		s16 const cam_y = (s16)((layer.pos.y + diff) / 8);
		s16 const base_x = layer.pos.x / 8;

		layer.update_top = (layer.update_top + 1) % 4;
		auto const [start, end] = std::array<std::pair<s16, s16>, 4>{
			std::pair{-1, 8},
			std::pair{8, 16},
			std::pair{16, 24},
			std::pair{24, 31},
		}[layer.update_top];

		for (s16 d_cam_x = start; d_cam_x < end; ++d_cam_x) {
			this->update_tile(base, layer, base_x + d_cam_x, cam_y);
		}
		if (layer.update_top == 0) {
			layer.last_load_at_y = layer.pos.y;
		}
	}

	s16 const dx = layer.pos.x - layer.last_load_at_x;
	if (dx != 0 || layer.update_left != 0) {
		s16 const diff = dx < 0 ? -4 : 244;
		s16 const cam_x = (s16)((layer.pos.x + diff) / 8);
		s16 const base_y = layer.pos.y / 8;

		layer.update_left = (layer.update_left + 1) % 4;
		auto const [start, end] = std::array<std::pair<s16, s16>, 4>{
			std::pair{-1, 5},
			std::pair{5, 10},
			std::pair{10, 15},
			std::pair{15, 21},
		}[layer.update_left];

		for (s16 d_cam_y = start; d_cam_y < end; ++d_cam_y) {
			this->update_tile(base, layer, cam_x, base_y + d_cam_y);
		}
		if (layer.update_left == 0) {
			layer.last_load_at_x = layer.pos.x;
		}
	}
}

void ScrollingMap::update_layer(Layer &layer) {
	ScreenEntry volatile *const base = tiles::SCREENBLOCKS[layer.tile_map];

	s16 const dy = layer.pos.y - layer.last_load_at_y;
	if (abs(dy) > 4) {
		s16 const diff = dy < 0 ? -4 : 164;
		s16 const cam_y = (s16)((layer.pos.y + diff) / 8);
		s16 const base_x = layer.pos.x / 8;

		for (s16 d_cam_x = -1; d_cam_x < 31; ++d_cam_x) {
			this->update_tile(base, layer, base_x + d_cam_x, cam_y);
		}
		layer.updated_y = true;
	}

	s16 const dx = layer.pos.x - layer.last_load_at_x;
	if (abs(dx) > 4) {
		s16 const diff = dx < 0 ? -4 : 244;
		s16 const cam_x = (s16)((layer.pos.x + diff) / 8);
		s16 const base_y = layer.pos.y / 8;

		for (s16 d_cam_y = -1; d_cam_y < 21; ++d_cam_y) {
			this->update_tile(base, layer, cam_x, base_y + d_cam_y);
		}
		layer.updated_x = true;
	}

	if (layer.updated_x) {
		layer.updated_x = false;
		layer.last_load_at_x = layer.pos.x;
	}
	if (layer.updated_y) {
		layer.updated_y = false;
		layer.last_load_at_y = layer.pos.y;
	}
}

void ScrollingMap::update_camera() {
	REG_BG0HOFS = (u16)this->layer0.pos.x;
	REG_BG0VOFS = (u16)this->layer0.pos.y;
	REG_BG1HOFS = (u16)this->layer1.pos.x;
	REG_BG1VOFS = (u16)this->layer1.pos.y;
}

} // namespace scrolling_map
