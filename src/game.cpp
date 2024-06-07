#include "game.h"
#include "input.h"
#include "sprite.h"
#include "tiles.h"

#include <cstring>

extern "C" {
#include <tonc.h>

#include "ground1.h"
#include "king2.h"
}

#define let auto const

namespace game {

s32 constexpr PLAYER_WIDTH = 32;
size_t constexpr PLAYER_SPRITE_OFFSET = 0;
size_t constexpr GROUND_TILE_OFFSET = 1;

void Game::update() {
	if (input::get_button(input::Button::Left).is_down()) {
		this->player_position -= 2;
		this->player_facing_right = false;
	}
	if (input::get_button(input::Button::Right).is_down()) {
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
}

void Game::vsync_hook() {
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
	REG_BG0CNT =
		(u16)(BG_CBB(3) | BG_SBB(0) | BG_4BPP | BG_REG_32x32 | BG_PRIO(3));
	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
	std::memcpy(
		&tiles::SPRITE_CHARBLOCK[0][PLAYER_SPRITE_OFFSET],
		king2Tiles,
		sizeof(tiles::STile) * 16
	);
	tiles::SPRITE_PALETTE_MEMORY[0] = *(tiles::Palette const *)king2Pal;

	tiles::CHARBLOCKS[3][0] = tiles::EMPTY;
	std::memcpy(
		&tiles::CHARBLOCKS[3][GROUND_TILE_OFFSET],
		ground1Tiles,
		sizeof(tiles::STile) * 16
	);
	tiles::BG_PALETTE_MEMORY[0] = *(tiles::Palette const *)ground1Pal;
	tiles::BG_PALETTE_MEMORY[0].colours[0] = tiles::BLACK;
}

} // namespace game
