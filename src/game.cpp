#include "game.h"
#include "input.h"
#include "sprite.h"
#include "tiles.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <ranges>

extern "C" {
#include <tonc.h>
#include <tonc_memmap.h>
#include <tonc_video.h>

#include "background1.h"
#include "ground1.h"
#include "king2.h"
}

#define let auto const
#define fn auto constexpr

namespace rv = std::ranges::views;
namespace r = std::ranges;
namespace v = std::views;

namespace game {

using input::Button;
using tiles::ScreenEntry;

s32 constexpr PLAYER_WIDTH = 32;
size_t constexpr PLAYER_SPRITE_OFFSET = 0;
size_t constexpr GROUND_TILE_OFFSET = 8;
size_t constexpr CHARBLOCK = 3;
size_t constexpr SCREENBLOCK = 0;

/// x: Tile space
fn load_background_raw(Game &game, s32 const world_x, size_t const x)->void {
	size_t constexpr BOTTOM_EDGE = SCREEN_HEIGHT / 8;
	size_t constexpr BACKGROUND_WIDTH = SCREEN_WIDTH * 2 / 8;
	let ground_tile_offset = (size_t(world_x) / 8uz) % BACKGROUND_WIDTH;

	for (let y : rv::iota(0uz, BOTTOM_EDGE)) {
		let bg_map_pos = y * 32 + x;
		let bg_img_pos = y * 32 + ground_tile_offset;
		auto se = ((ScreenEntry const *)background1Map)[bg_img_pos];
		se.index += GROUND_TILE_OFFSET;
		tiles::SCREENBLOCKS[SCREENBLOCK][bg_map_pos] = se;
	}
}

/// world_x: Pixel space
fn load_background(Game &game, s32 const world_x)->void {
	let x_signed = world_x - game.camera_position;
	assert(x_signed >= 0);
	assert(x_signed <= SCREEN_WIDTH);
	let x = size_t(x_signed / 8);
	load_background_raw(game, world_x, x);
}

void Game::update() {
	let old_cam = this->camera_position;
	if (input::get_button(Button::Left).is_down()) {
		this->player_position -= 2;
		this->player_facing_right = false;
	}
	if (input::get_button(Button::Right).is_down()) {
		this->player_position += 2;
		this->player_facing_right = true;
	}

	let pos_diff = this->player_position - this->camera_position;
	if (pos_diff > SCREEN_WIDTH - PLAYER_WIDTH * 4) {
		this->camera_position++;
		if (pos_diff > SCREEN_WIDTH - PLAYER_WIDTH * 3) {
			this->camera_position++;
		}
	}
	if (pos_diff < PLAYER_WIDTH * 3) {
		this->camera_position--;
		if (pos_diff < PLAYER_WIDTH * 2) {
			this->camera_position--;
		}
	}

	switch (old_cam - this->camera_position) {
	case -2:
	case -1: {
		load_background_raw(
			*this,
			this->camera_position,
			size_t(this->camera_position / 8) % 32uz
		);
	} break;
	case 2:
	case 1: {
		load_background_raw(
			*this,
			this->camera_position + SCREEN_WIDTH,
			size_t(this->camera_position / 8) % 32uz
		);
	} break;
	default:
		break;
	}
}

void Game::vsync_hook() {
	REG_BG0HOFS = u16(this->camera_position);
	sprite::HardwareSprite{
		.y = 70,
		.object_mode = sprite::ObjectMode::Normal,
		.horizontal_flip = this->player_facing_right,
	}
		.set_x(this->player_position - this->camera_position)
		.set_size(sprite::SpriteSize::X32)
		.write_to_screen(0);
}

void Game::restore() {
	REG_BG0CNT = (u16)(0 | BG_CBB(CHARBLOCK) | BG_SBB(SCREENBLOCK) | BG_4BPP
			   | BG_REG_32x32 | BG_PRIO(3));
	REG_DISPCNT = 0 | DCNT_MODE0
		      | DCNT_BG0
		      // | DCNT_BG1
		      | DCNT_OBJ | DCNT_OBJ_1D;
	std::memcpy(
		(void *)&tiles::SPRITE_CHARBLOCK[0][PLAYER_SPRITE_OFFSET],
		(void *)king2Tiles,
		sizeof(tiles::STile) * 16
	);
	tiles::SPRITE_PALETTE_MEMORY[0] = *(tiles::Palette const *)king2Pal;

	tiles::CHARBLOCKS[CHARBLOCK][0] = tiles::EMPTY;
	std::memcpy(
		(void *)&tiles::CHARBLOCKS[CHARBLOCK][GROUND_TILE_OFFSET],
		(void *)background1Tiles,
		background1TilesLen
	);
	tiles::BG_PALETTE_MEMORY[0] = *(tiles::Palette const *)background1Pal;
	tiles::BG_PALETTE_MEMORY[0].colours[0] = tiles::BLACK;

	for (let x : rv::iota(
			     this->camera_position,
			     // this->camera_position + 256
			     SCREEN_WIDTH + this->camera_position
		     ) | v::stride(8))
	{
		load_background(*this, x);
	}
}

} // namespace game
