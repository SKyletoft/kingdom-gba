#pragma once

#include "hexes.h"
#include "point.h"
#include "sprite.h"

#include <cstddef>

extern "C" {
#include <tonc_types.h>
}

namespace cursor_scroller {

using hexes::CubeCoord;
using point::Point;
using sprite::HexSprite;
using sprite::SpriteSize;

class CursorScroller {
  public:
	HexSprite cursor{
		.pos = CubeCoord::from_offset_xy({.col = 5, .row = 5}),
		.centre = Point<s16>{.x = 5, .y = -4},
		.size = SpriteSize::x16,
		.hardware_id = 0,
		.tile_index = 1,
		.hidden = true,
	};

	/// Up, Down, Left, Right
	std::array<s16, 4> directional_cooldowns{0, 0, 0, 0};
	size_t t = 0;
	s16 scroll_speed = 2;
	s16 cooldown = 12;

	void handle_input();
	bool move_cursor(Point<s32> const);
	Point<s16> recentre_camera(Point<s32> const) const;
	CubeCoord &pos();

	constexpr CursorScroller() {}
};

} // namespace cursor_scroller
