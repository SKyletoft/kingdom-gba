#include "popup.h"

#include "input.h"
#include "tiles.h"
#include "tty.h"
#include "util.h"

#include "loading.h"

#include <cstring>
#include <ranges>

extern "C" {
#include <tonc.h>
#include <tonc_memdef.h>

#include "font.h"
}

namespace popup {

namespace r = std::ranges;
namespace v = std::views;
namespace rv = std::ranges::views;

using input::Button;
using tiles::BG_PALETTE_MEMORY;
using tiles::CHARBLOCKS;
using tiles::Colour;
using tiles::Palette;
using tiles::SCREENBLOCKS;
using tiles::ScreenEntry;
using tiles::SPRITE_CHARBLOCK;
using tiles::SPRITE_PALETTE_MEMORY;

constexpr size_t END_OF_ALPHABET = '~' - ' ' + 3;

static constexpr bool id(bool b) { return b; }

PopupMenu &PopupMenu::show(size_t i) {
	this->visible[i] = true;
	return *this;
}

PopupMenu &PopupMenu::hide(size_t i) {
	this->visible[i] = false;
	return *this;
}

void PopupMenu::b_hook() {}
void PopupMenu::selection_hook() {}
void PopupMenu::move_hook() {}

void PopupMenu::update() {
	if (input::get_button(Button::B).is_down()) {
		this->b_hook();
	}

	size_t const entry_count = (size_t)r::count_if(this->visible, id);

	if (input::get_button(Button::A) == input::InputState::Pressed) {
		this->cursor.x = (u8)((this->x * 8) % 240) + 8;

		size_t i = 0;
		for (; i < this->selection; ++i) {
			if (!this->visible[i]) {
				i++;
			}
		}

		this->selection_hook();
		this->entries[i].second();
	}
	if (input::get_button(Button::A) == input::InputState::Released) {
		this->cursor.x = (u8)((this->x * 8) % 240) + 5;
	}

	if (input::get_button(Button::Up) == input::InputState::Pressed) {
		if (this->selection == 0) {
			this->selection = (u16)entry_count;
		}
		this->move_hook();
		this->selection--;
	}

	if (input::get_button(Button::Down) == input::InputState::Pressed) {
		this->move_hook();
		this->selection = (u16)((this->selection + 1) % entry_count);
	}
}

void PopupMenu::always_update() {}

void PopupMenu::suspend() {
	REG_DISPCNT &= ~(u32)(DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D);
}

void PopupMenu::load_tiles_and_palettes() {
	CHARBLOCKS[this->tile_source][0] = tiles::EMPTY;
	CHARBLOCKS[this->tile_source][END_OF_ALPHABET] = tiles::STile{{
		0x11111111,
		0x11111111,
		0x11111111,
		0x11111111,
		0x11111111,
		0x11111111,
		0x11111111,
		0x11111111,
	}};
	std::memcpy(CHARBLOCKS[this->tile_source] + 1, fontTiles, fontTilesLen);

	BG_PALETTE_MEMORY[15] = loading::UI_PALETTE;
}

void PopupMenu::load_sprites() {
	SPRITE_CHARBLOCK[0][1] = tiles::STile{{
		0x00033000,
		0x00033300,
		0x33333330,
		0x33333333,
		0x33333334,
		0x44433340,
		0x00033400,
		0x00044000,
	}};

	SPRITE_PALETTE_MEMORY[15] = Palette{{
		tiles::TRANSPARENT,
		tiles::BLACK,
		tiles::BLACK,
		Colour(32, 10, 10),
		Colour(31, 15, 15),
	}};
}

void PopupMenu::restore() {
	assert(this->entries.size() == this->visible.size());

	this->selection = 0;

	util::wait_for_drawing_complete();
	REG_DISPCNT &= ~(u32)(DCNT_BG3 | DCNT_BG2);
	this->load_tiles_and_palettes();
	this->load_sprites();
	util::clear_layer(this->tile_map);

	size_t const menu_width = ([&]() {
		size_t longest = 0;
		for (auto const &[t, b] : rv::zip(this->entries, this->visible)) {
			if (b && t.first.size() > longest) {
				longest = t.first.size();
			}
		}
		return longest;
	})();
	size_t const menu_height = (size_t)r::count_if(this->visible, id);

	// TODO: Merge these loops
	for (auto const [y, x] : v::cartesian_product(
			 v::iota(0uz, menu_height + 2), v::iota(0uz, menu_width + 3)
		 ))
	{
		auto const y_ = (y + (size_t)this->y);
		auto const x_ = x + (size_t)this->x;
		SCREENBLOCKS[this->tile_map][y_ * 32 + x_] =
			ScreenEntry((u16)END_OF_ALPHABET, 0, 15);
	}

	for (auto [y, t] : rv::zip(this->entries, this->visible)
						   | rv::filter([](auto x) { return x.second; })
						   | rv::transform([](auto x) { return x.first; })
						   | v::enumerate)
	{
		auto [s, _] = t;
		for (auto const [x, c] : s | v::enumerate | v::take(s.size())) {
			auto const y_ = y + this->y;
			auto const x_ = x + 34 + this->x;
			// Indent by one for the cursor to fit
			SCREENBLOCKS[this->tile_map][y_ * 32 + x_] =
				ScreenEntry(tty::get_character_tile_index(c + 2), 0, 15);
		}
	}

	REG_BG3HOFS = 0;
	REG_BG3VOFS = 0;

	util::wait_for_drawing_complete();

	this->cursor.x = (u8)((this->x * 8) % 240) + 8;
	this->cursor.write_to_screen(0);

	REG_BG3CNT = (u16)(BG_CBB(this->tile_source) | BG_SBB(this->tile_map)
					   | BG_4BPP | BG_REG_32x32 | BG_PRIO(0));
	REG_DISPCNT |= DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D;
}

void PopupMenu::vsync_hook() {
	this->cursor.y = (u8)((this->y * 8) % 160 + 8 * (1 + this->selection));
	this->cursor.write_to_screen(0);
}

void PopupMenu::set_position(s16 x, s16 y) {
	this->x = x;
	this->y = y;
}

} // namespace popup
