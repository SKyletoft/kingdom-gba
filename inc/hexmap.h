#pragma once

#include "hexes.h"
#include "mdspan.h"
#include "point.h"
#include "scrolling_map.h"
#include "tiles.h"

namespace hexmap {

using hexes::OffsetXYCoord;
using point::Point;
using scrolling_map::Layer;
using tiles::Palette;
using tiles::ScreenEntry;
using tiles::STile;

class Hexmap : public scrolling_map::ScrollingMap {
  public:
	mdspan::Span2d<const u8> const map;

	std::pair<hexes::OffsetXYCoord, Point<s16>>
	grid_coord(Layer &layer, s16 x, s16 y);

	void load_tilesets(Layer &) override;
	void load_palettes(Layer &) override;
	ScreenEntry get_tile(Layer &, s16, s16) override;

	Hexmap(mdspan::Span2d<const u8> const map)
		: ScrollingMap(
			std::max((s16)0, (s16)(map.width * 3 * 8 - 240)),
			std::max((s16)0, (s16)(map.height * 16 - 155))
		)
		, map(map) {}
};

class SimpleHexmap : public Hexmap {
  private:
	std::span<const STile> tiles;
	std::span<const Palette> palettes;

  public:
	void load_tilesets(Layer &) override;
	void load_palettes(Layer &) override;

	SimpleHexmap(
		mdspan::Span2d<const u8> const map, std::span<const STile> const tiles,
		std::span<const Palette> const palettes
	)
		: Hexmap(map)
		, tiles(tiles)
		, palettes(palettes) {}
};

} // namespace hexmap
