#pragma once

#include "point.h"
#include "tiles.h"

extern "C" {
#include <tonc_types.h>
}

namespace scrolling_map {

using point::Point;
using tiles::ScreenEntry;
using tiles::STile;

size_t get_screenblock_offset_from_tiles(s16 x, s16 y);
size_t get_screenblock_offset_from_camera(s16 x, s16 y);

struct Layer {
	const size_t tile_source;
	const size_t tile_map;

	const s16 min_x = 0;
	const s16 min_y = 0;
	const s16 max_x = 40 * 3 * 8;
	const s16 max_y = 30 * 4 * 8;

	Point<s16> pos;
	s16 last_load_at_x = -100;
	s16 last_load_at_y = -100;
	bool updated_x = false;
	bool updated_y = false;
	size_t update_top = 0;
	size_t update_left = 0;

	bool operator==(Layer const &) const = default;
	bool operator!=(Layer const &) const = default;

	void move_in_bounds(s16, s16);
};

class ScrollingMap {
  public:
	Layer layer0;
	Layer layer1;

	void update_tile(ScreenEntry volatile *const, Layer &, s16, s16);
	void
	update_tile_from_camera(ScreenEntry volatile *const, Layer &, s16, s16);
	void update_layer(Layer &);
	void update_layer_partial(Layer &);
	void recolour(Point<s32>, Point<s32>, Layer &);
	void load_map(Layer &);
	void move_in_bounds(Point<s16>);
	void move_in_bounds(s16, s16);
	void update_camera();
	ScreenEntry get_tile_from_camera(Layer &, s16, s16);

	virtual void load_tilesets(Layer &) = 0;
	virtual void load_palettes(Layer &) = 0;
	virtual ScreenEntry get_tile(Layer &, s16, s16) = 0;

	ScrollingMap(s16 width, s16 height)
		: layer0(Layer{
			.tile_source = 3,
			.tile_map = 0,
			.max_x = width,
			.max_y = height,
			.pos = Point<s16>{.x = 0}
		})
		, layer1(Layer{
			  .tile_source = 3,
			  .tile_map = 1,
			  .min_x = 4,
			  .max_x = (s16)(width + 4),
			  .max_y = height,
			  .pos = Point<s16>{.x = 4}
		  }) {}

	ScrollingMap(
		s16 width, s16 height, size_t bg0_tile_source, size_t bg0_tile_map,
		size_t bg1_tile_source, size_t bg1_tile_map
	)
		: layer0(Layer{
			.tile_source = bg0_tile_source,
			.tile_map = bg0_tile_map,
			.max_x = width,
			.max_y = height,
			.pos = Point<s16>{.x = 0}
		})
		, layer1(Layer{
			  .tile_source = bg1_tile_source,
			  .tile_map = bg1_tile_map,
			  .min_x = 4,
			  .max_x = (s16)(width + 4),
			  .max_y = height,
			  .pos = Point<s16>{.x = 4}
		  }) {}

	virtual ~ScrollingMap(){};
};

} // namespace scrolling_map
