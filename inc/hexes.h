#pragma once

#include "point.h"

#include <array>
#include <compare>
#include <cstddef>
#include <cstdlib>
#include <optional>

extern "C" {
#include <tonc_types.h>
}

namespace hexes {

struct CubeCoord;
struct OffsetXYCoord;
struct AxialCoord;

// Note: Used for indexing, mirror any changes in CUBE_DIRECTION_VECTORS
enum class Direction { R = 0, UR, UL, L, DR, DL };

struct CubeCoord {
	s16 const q = 0;
	s16 const r = 0;
	s16 const s = 0;

  private:
	constexpr CubeCoord(s16 q, s16 r, s16 s)
		: q(q)
		, r(r)
		, s(s) {}

	constexpr static bool check_invariant(s16 q, s16 r, s16 s) {
		return (q + r + s) == 0;
	}

  public:
	// Operators

	CubeCoord &operator=(CubeCoord vec);

	constexpr std::strong_ordering
	operator<=>(CubeCoord const &) const = default;

	constexpr CubeCoord operator+(CubeCoord vec) const {
		return this->add(vec);
	}
	constexpr CubeCoord &operator+=(CubeCoord v) { return *this = *this + v; }
	constexpr CubeCoord operator+(Direction) const;
	constexpr CubeCoord &operator+=(Direction d) { return *this = *this + d; }

	constexpr CubeCoord operator*(s16 factor) const {
		return this->scale(factor);
	}
	constexpr CubeCoord &operator*=(s16 factor) {
		return *this = *this * factor;
	}

	constexpr CubeCoord operator-(CubeCoord vec) const {
		return this->subtract(vec);
	}
	constexpr CubeCoord &operator-=(CubeCoord vec) {
		return *this = *this - vec;
	}

	constexpr CubeCoord add(CubeCoord const vec) const {
		return CubeCoord(this->q + vec.q, this->r + vec.r, this->s + vec.s);
	}
	constexpr CubeCoord subtract(CubeCoord vec) const {
		return CubeCoord(this->q - vec.q, this->r - vec.r, this->s - vec.s);
	}
	constexpr CubeCoord scale(s16 factor) const {
		return CubeCoord(this->q * factor, this->r * factor, this->s * factor);
	}

	// Methods

	constexpr s16 distance(CubeCoord hex) const {
		CubeCoord vec = *this - hex;
		auto dist = (std::abs(vec.q) + std::abs(vec.r) + std::abs(vec.s)) / 2;
		return (s16)dist;
	}

	constexpr CubeCoord neighbour(Direction dir) const;

	constexpr bool is_odd() const;

	// Factories

	constexpr CubeCoord() {}
	constexpr CubeCoord(CubeCoord const &vec)
		: q(vec.q)
		, r(vec.r)
		, s(vec.s) {}

	static constexpr std::optional<CubeCoord> from_qrs(s16 q, s16 r, s16 s) {
		if (check_invariant(q, r, s)) {
			return CubeCoord(q, r, s);
		}
		return std::nullopt;
	}

	static constexpr CubeCoord unsafe_from_qrs_unchecked(s16 q, s16 r, s16 s) {
		return CubeCoord(q, r, s);
	}

	constexpr OffsetXYCoord to_offset_xy() const;
	static constexpr CubeCoord from_offset_xy(OffsetXYCoord);

	constexpr AxialCoord to_axial_coord() const;
	static constexpr CubeCoord from_axial_coord(AxialCoord);

	constexpr point::Point<s32> to_pixel_space() const;
};

struct OffsetXYCoord {
	s16 col;
	s16 row;

	constexpr CubeCoord to_cube_coord() const;
	static constexpr OffsetXYCoord from_cube_coord(CubeCoord);

	constexpr AxialCoord to_axial_coord() const;
	static constexpr OffsetXYCoord from_axial_coord(AxialCoord);

	constexpr point::Point<s32> to_point() const {
		return {.x = this->col, .y = this->row};
	};
};

struct AxialCoord {
	s16 q;
	s16 r;

	constexpr CubeCoord to_cube_coord() const;
	static constexpr AxialCoord from_cube_coord(CubeCoord);

	constexpr OffsetXYCoord to_offset_xy() const;
	static constexpr AxialCoord from_offset_xy(OffsetXYCoord);
};

constexpr OffsetXYCoord CubeCoord::to_offset_xy() const {
	auto hex = *this;
	auto col = hex.q + (hex.r - (hex.r & 1)) / 2;
	auto row = hex.r;
	return OffsetXYCoord((s16)col, (s16)row);
}

constexpr CubeCoord CubeCoord::from_offset_xy(OffsetXYCoord rhs) {
	return rhs.to_cube_coord();
}

constexpr point::Point<s32> CubeCoord::to_pixel_space() const {
	auto const xy = this->to_offset_xy();
	return point::Point{
		.x = xy.col * 24 + 12 * (xy.row & 1),
		.y = xy.row * 16,
	};
}

constexpr AxialCoord CubeCoord::to_axial_coord() const {
	auto hex = *this;
	s16 q = hex.q;
	s16 r = hex.r;
	return AxialCoord(q, r);
}

constexpr CubeCoord CubeCoord::from_axial_coord(AxialCoord rhs) {
	return rhs.to_cube_coord();
}

constexpr CubeCoord OffsetXYCoord::to_cube_coord() const {
	auto hex = *this;
	auto q = hex.col - (hex.row - (hex.row & 1)) / 2;
	auto r = hex.row;
	auto s = -q - r;
	return CubeCoord::unsafe_from_qrs_unchecked((s16)q, (s16)r, (s16)s);
}

constexpr OffsetXYCoord OffsetXYCoord::from_cube_coord(CubeCoord rhs) {
	return rhs.to_offset_xy();
}

constexpr AxialCoord OffsetXYCoord::to_axial_coord() const {
	return this->to_cube_coord().to_axial_coord();
}

constexpr OffsetXYCoord OffsetXYCoord::from_axial_coord(AxialCoord rhs) {
	return rhs.to_offset_xy();
}

constexpr CubeCoord AxialCoord::to_cube_coord() const {
	auto hex = *this;
	s16 q = hex.q;
	s16 r = hex.r;
	s16 s = (s16)-q - (s16)r;
	return CubeCoord::unsafe_from_qrs_unchecked(q, r, s);
}

constexpr AxialCoord AxialCoord::from_cube_coord(CubeCoord rhs) {
	return rhs.to_axial_coord();
}

constexpr OffsetXYCoord AxialCoord::to_offset_xy() const {
	return this->to_cube_coord().to_offset_xy();
}

constexpr AxialCoord AxialCoord::from_offset_xy(OffsetXYCoord rhs) {
	return rhs.to_axial_coord();
}

static constexpr std::array<CubeCoord, 6> CUBE_DIRECTION_VECTORS = {
	CubeCoord::unsafe_from_qrs_unchecked(1, 0, -1),
	CubeCoord::unsafe_from_qrs_unchecked(1, -1, 0),
	CubeCoord::unsafe_from_qrs_unchecked(0, -1, 1),
	CubeCoord::unsafe_from_qrs_unchecked(-1, 0, 1),
	CubeCoord::unsafe_from_qrs_unchecked(0, 1, -1),
	CubeCoord::unsafe_from_qrs_unchecked(-1, 1, 0),
};

constexpr bool CubeCoord::is_odd() const {
	return this->to_offset_xy().row & 1;
}

constexpr CubeCoord CubeCoord::neighbour(Direction dir) const {
	return CUBE_DIRECTION_VECTORS[(size_t)dir];
};

constexpr CubeCoord CubeCoord::operator+(Direction dir) const {
	return *this + this->neighbour(dir);
}

} // namespace hexes

// Hash impl by ChatGPT
namespace std {
template <> struct hash<hexes::CubeCoord> {
	size_t operator()(const hexes::CubeCoord &obj) const {
		size_t hashValue = 0;
		// Use std::hash_combine for combining hash values
		hash_combine(hashValue, obj.q);
		hash_combine(hashValue, obj.r);
		hash_combine(hashValue, obj.s);
		return hashValue;
	}

	// Hash combining function
	template <typename T> void hash_combine(size_t &seed, const T &v) const {
		seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
};
} // namespace std
