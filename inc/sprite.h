#pragma once

#include "hexes.h"
#include "point.h"
#include <cstddef>
#include <span>

extern "C" {
#include <tonc.h>
#include <tonc_types.h>
}

namespace sprite {

using hexes::CubeCoord;
using hexes::Direction;
using point::Point;

enum class ColourMode : u8 { BPP4 = 0, BPP8 = 1 };
enum class ObjectMode : u8 {
	Normal = 0b00,
	Affine = 0b01,
	Hidden = 0b10,
	Large = 0b11
};
enum class GraphicsMode : u8 {
	Normal = 0b00,
	Alpha = 0b01,
	Mask = 0b10,
};
enum class SpriteSize : u8 {
	x8 = 0b0000,
	x16 = 0b0001,
	x32 = 0b0010,
	x64 = 0b0011,
	w16h8 = 0b0100,
	w32h8 = 0b0101,
	w32h16 = 0b0110,
	w64h32 = 0b0111,
	w8h16 = 0b1000,
	w8h32 = 0b1001,
	w16h32 = 0b1010,
	w32h64 = 0b1011
};

struct alignas(8) __attribute((packed)) HardwareSprite {
	// attr0
	u8 y : 8 = 0;
	ObjectMode object_mode : 2 = ObjectMode::Hidden;
	GraphicsMode graphics_mode : 2 = GraphicsMode::Normal;
	bool mosaic : 1 = 0;
	ColourMode colour_mode : 1 = ColourMode::BPP4;
	u8 shape : 2 = 0;

	// attr1
	u16 x : 9 = 0;
	u8 _pad0 : 3 = 0;
	bool horizontal_flip : 1 = false;
	bool vertical_flip : 1 = false;
	u8 size : 2 = 0;

	// attr2
	u16 tile_index : 10 = 0;
	u8 prio : 2 = 0;
	u8 palette : 4 = 0;

	// padding
	u16 _pad1 = 0;

	void write_to_screen(size_t hardware_sprite_id);
	static void hide(size_t hardware_sprite_id);

	constexpr HardwareSprite &set_size(SpriteSize const new_size) {
		this->shape = (u8)(((u8)new_size & 0b1100) >> 2);
		this->size = (u8)((u8)new_size & 0b11);
		return *this;
	}
};
static_assert(sizeof(HardwareSprite) == sizeof(OBJ_ATTR));
static_assert(alignof(HardwareSprite) >= alignof(OBJ_ATTR));

static constexpr HardwareSprite X8{
	.shape = 0b00,
	.size = 0b00,
};
static constexpr HardwareSprite X16{
	.shape = 0b00,
	.size = 0b01,
};
static constexpr HardwareSprite X32{
	.shape = 0b00,
	.size = 0b10,
};
static constexpr HardwareSprite X64{
	.shape = 0b00,
	.size = 0b11,
};

struct HexSprite {
	CubeCoord pos{};
	Point<s16> animation{};
	Point<s16> centre{};
	SpriteSize size = SpriteSize::x8;
	u8 hardware_id = 0;
	bool horizontal_flip = false;
	bool vertical_flip = false;
	u16 tile_index = 0;
	u8 prio = 0;
	u8 palette = 0;
	ObjectMode object_mode = ObjectMode::Normal;
	ColourMode colour_mode = ColourMode::BPP4;
	bool hidden = false;

	constexpr HexSprite &translate(Direction d) {
		this->pos += d;
		return *this;
	}

	constexpr HexSprite &translate(CubeCoord vec) {
		this->pos += vec;
		return *this;
	}

	constexpr HexSprite &move_to(Direction dir) {
		return this->move_to(this->pos + dir);
	}

	constexpr HexSprite &move_to(CubeCoord vec) {
		auto from = this->pos.to_pixel_space();
		this->pos = vec;
		auto to = this->pos.to_pixel_space();
		this->animation += (from - to).into<s16>();
		return *this;
	}

	void hide() const;
	void move();
	void render(Point<s16> const camera_offset) const;
};

// Due to fun literal language bugs, this is read only and you have to use the
// `Sprite::write_to_screen` function to write
//
// https://timsong-cpp.github.io/lwg-issues/3813
static const std::span<const HardwareSprite> SPRITE_MEM{
	(HardwareSprite *)MEM_OAM, 128
};

} // namespace sprite
