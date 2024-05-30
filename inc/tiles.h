#pragma once

#include <algorithm>
#include <cmath>
#include <span>

extern "C" {
#include <tonc_memmap.h>
#include <tonc_types.h>
}

namespace tiles {

class Colour {
  private:
	u16 data;

	static constexpr u16 MASK = 0b11111;
	static constexpr u16 convert(u8 red, u8 green, u8 blue) {
		auto const clamp = [](u8 x) { return std::clamp(x, (u8)0, (u8)31); };
		red = clamp(red);
		green = clamp(green);
		blue = clamp(blue);

		const u16 be =
			(u16)(((blue & MASK) << 10) | ((green & MASK) << 5) | (red & MASK));

		return be;
	};

	struct RGB {
		u8 red;
		u8 green;
		u8 blue;
	};

  public:
	consteval static Colour from_24bit_colour(u8 red, u8 green, u8 blue) {
		auto const lerp = [](u8 x) {
			return (u8)std::round(std::lerp(0, 31, (double)x / 255.0));
		};
		return Colour(lerp(red), lerp(green), lerp(blue));
	}

	constexpr Colour(u8 red, u8 green, u8 blue)
		: data(convert(red, green, blue)) {}

	constexpr Colour(RGB rgb)
		: Colour(rgb.red, rgb.green, rgb.blue) {}

	constexpr Colour(u16 raw)
		: data(raw) {}

	constexpr Colour()
		: data(0) {}

	constexpr u16 raw() { return this->data; }
	constexpr RGB rgb() {
		return RGB{
			.red = (u8)(this->data & MASK),
			.green = (u8)((this->data >> 5) & MASK),
			.blue = (u8)((this->data >> 10) & MASK),
		};
	}
};
static_assert(sizeof(Colour) == sizeof(u16));

static constexpr Colour RED = Colour({
	.red = 0b11100,
	.green = 0,
	.blue = 0,
});
static constexpr Colour GREEN = Colour({
	.red = 0,
	.green = 0b11100,
	.blue = 0,
});
static constexpr Colour YELLOW = Colour({
	.red = 0b11111,
	.green = 0b11111,
	.blue = 0,
});
static constexpr Colour BLUE = Colour({
	.red = 0,
	.green = 0,
	.blue = 0b11100,
});
static constexpr Colour BLACK = Colour({
	.red = 0,
	.green = 0,
	.blue = 0,
});
static constexpr Colour WHITE = Colour({
	.red = 0b11111,
	.green = 0b11111,
	.blue = 0b11111,
});
static constexpr Colour TRANSPARENT = BLACK;

struct Palette {
	Colour colours[16];

	static constexpr Palette from_raw(const u16 raws[16]) {
		Palette pal{};
		for (size_t i = 0; i < 16; ++i) {
			pal.colours[i] = raws[i];
		}
		return pal;
	}
};

static_assert(sizeof(Palette[16]) == 0x0200);

const std::span<Palette, 16> BG_PALETTE_MEMORY{(Palette *)pal_bg_mem, 16};
const std::span<Palette, 16> SPRITE_PALETTE_MEMORY{(Palette *)pal_obj_mem, 16};

constexpr Palette BLACK_ON_BLACK = Palette{{
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
	tiles::BLACK,
}};

union ScreenEntry {
	u16 raw;
	struct __attribute((packed)) {
		u16 index : 10;
		u8 flips : 2;
		u8 palette : 4;
	};

	constexpr ScreenEntry(u16 index, u8 flips, u8 palette) {
		this->index = (u16)(index & 0b1111111111);
		this->flips = (u8)(flips & 0b11);
		this->palette = (u8)(palette & 0b1111);
	}
	constexpr ScreenEntry(const ScreenEntry &rhs)
		: raw(rhs.raw) {}
	constexpr ScreenEntry(volatile ScreenEntry &rhs)
		: raw(rhs.raw) {}
	constexpr ScreenEntry(const u16 &rhs)
		: raw(rhs) {}

	constexpr operator u16() const { return this->raw; }
	constexpr operator u16() { return this->raw; }

	constexpr void operator=(const ScreenEntry &rhs) volatile {
		this->raw = rhs.raw;
	}
	constexpr void operator=(const ScreenEntry &rhs) { this->raw = rhs.raw; }
	constexpr void operator=(volatile ScreenEntry &rhs) { this->raw = rhs.raw; }

	constexpr ScreenEntry with_palette(u8 new_pal) {
		ScreenEntry copy{this->raw};
		copy.palette = new_pal & 0b1111;
		return copy;
	}
};
static_assert(sizeof(ScreenEntry) == sizeof(u16));
static_assert(alignof(ScreenEntry) == alignof(u16));

/// 4 bpp tile
union STile {
	u32 index_octs[8];
	u16 index_quads[16];
	byte raw[32];
};
static_assert(sizeof(STile) == 32);

constexpr STile EMPTY{{0, 0, 0, 0, 0, 0, 0, 0}};

using Charblock = STile[512];
static_assert(sizeof(Charblock) == 16 * 1024);
static const std::span<Charblock> CHARBLOCKS{(Charblock *)tile_mem, 4};
static const std::span<Charblock> SPRITE_CHARBLOCK{
	(Charblock *)tile_mem + 4, 2
};

using Screenblock = ScreenEntry[1024];
static_assert(sizeof(Screenblock) == 2048);
static_assert(alignof(Screenblock) == alignof(ScreenEntry));

static const std::span<volatile Screenblock> SCREENBLOCKS{
	(Screenblock *)se_mem, 32
};
static_assert(sizeof(SCREENBLOCKS) <= 96 * 1024);

} // namespace tiles
