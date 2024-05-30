#pragma once

#include "sprite.h"
#include "state.h"

#include <cstring>
#include <functional>
#include <initializer_list>
#include <string_view>
#include <vector>

extern "C" {
#include <tonc_types.h>
}

namespace popup {

using sprite::HardwareSprite;

class PopupMenu : public state::Mode {
  protected:
	// TODO: Replace std::vector with something more platform appropriate
	std::vector<std::pair<std::string_view, std::function<void()>>> entries{};
	std::vector<bool> visible{};

	size_t const tile_source;
	size_t const sprite_tile_source;
	size_t const tile_map;

	s16 x = 10;
	s16 y = 6;
	u16 selection = 0;

	HardwareSprite cursor{
		.y = 76,
		.object_mode = sprite::ObjectMode::Normal,
		.x = 116,
		.horizontal_flip = true,
		.tile_index = 1,
		.palette = 15,
	};

  public:
	void update() override;
	void always_update() override;
	void suspend() override;
	void restore() override;
	void vsync_hook() override;

	virtual void b_hook();
	virtual void selection_hook();
	virtual void move_hook();

	PopupMenu &show(size_t);
	PopupMenu &hide(size_t);
	void set_position(s16, s16);

	virtual void load_tiles_and_palettes();
	virtual void load_sprites();

	PopupMenu()
		: PopupMenu({}) {}

	PopupMenu(
		std::initializer_list<std::pair<char const *, std::function<void()>>> l
	)
		: PopupMenu(l, 2, 4, 3) {}

	PopupMenu(
		std::initializer_list<std::pair<char const *, std::function<void()>>> l,
		size_t tile_source, size_t sprite_tile_source, size_t tile_map
	)
		: Mode()
		, visible(std::vector<bool>(l.size(), true))
		, tile_source(tile_source)
		, sprite_tile_source(sprite_tile_source)
		, tile_map(tile_map) {
		this->entries.reserve(l.size() - this->entries.capacity());
		for (auto &[s, f] : l) {
			this->entries.push_back({std::string_view{s}, f});
		}
	}
};

} // namespace popup
