#pragma once

#include <array>
#include <assert.h>
#include <cstdint>
#include <format>
#include <string>

namespace TCSolver {

/**
 * Hex coordinate using a cubic system, with the straight axis being the vertical.
 */
struct Hex {
public:
	static const Hex ZERO;
	static const std::array<Hex, 6> DIRECTIONS;
	static const Hex NORTH;
	static const Hex SOUTH;
	static const Hex NORTHEAST;
	static const Hex NORTHWEST;
	static const Hex SOUTHEAST;
	static const Hex SOUTHWEST;

	static constexpr int32_t Distance(const Hex& a, const Hex& b) noexcept {
		return std::max(
			std::abs(a.i + a.j - b.i - b.j),
			std::max(
				std::abs(a.i - b.i),
				std::abs(a.j - b.j)
			)
		);
	}

	int32_t i;
	int32_t j;

	constexpr Hex() noexcept : i(0), j(0) {}
	constexpr Hex(int32_t i, int32_t j) noexcept : i(i), j(j) {}

	constexpr ~Hex() = default;

	constexpr Hex(const Hex& other) noexcept = default;
	constexpr Hex& operator=(const Hex& other) noexcept = default;
	constexpr Hex(Hex&& other) noexcept = default;
	constexpr Hex& operator=(Hex&& other) noexcept = default;

	constexpr void NeighboringPositions(std::array<Hex, 6>& out) const noexcept {
		for (size_t i = 0; i < DIRECTIONS.size(); ++i) out[i] = *this + DIRECTIONS[i];
	}

	constexpr std::array<Hex, 6> GetNeighboringPositions() const noexcept {
		std::array<Hex, 6> out;
		for (size_t i = 0; i < DIRECTIONS.size(); ++i) out[i] = *this + DIRECTIONS[i];
		return out;
	}

	std::string to_string() const { return std::format("Hex({}, {}, {})", i, j, -i - j); }

	constexpr Hex& operator+=(const Hex& other) noexcept { i += other.i; j += other.j; return *this; }
	constexpr friend Hex operator+(const Hex& lhs, const Hex& rhs) noexcept { return Hex(lhs) += rhs; }

	constexpr Hex& operator-=(const Hex& other) noexcept { i -= other.i; j -= other.j; return *this; }
	constexpr friend Hex operator-(const Hex& lhs, const Hex& rhs) noexcept { return Hex(lhs) -= rhs; }

	constexpr friend bool operator==(const Hex& lhs, const Hex& rhs) noexcept
		{ return lhs.i == rhs.i && lhs.j == rhs.j; }
	constexpr friend bool operator!=(const Hex& lhs, const Hex& rhs) noexcept { return !(lhs == rhs); }
};

inline constexpr Hex Hex::ZERO = Hex(0, 0);
inline constexpr std::array<Hex, 6> Hex::DIRECTIONS = {
	Hex(+1,  0), Hex(+1, -1), Hex(0, -1),
	Hex(-1,  0), Hex(-1, +1), Hex(0, +1)
};
inline constexpr Hex Hex::NORTH = Hex::DIRECTIONS[1];
inline constexpr Hex Hex::SOUTH = Hex::DIRECTIONS[0];
inline constexpr Hex Hex::NORTHEAST = Hex::DIRECTIONS[3];
inline constexpr Hex Hex::NORTHWEST = Hex::DIRECTIONS[4];
inline constexpr Hex Hex::SOUTHEAST = Hex::DIRECTIONS[5];
inline constexpr Hex Hex::SOUTHWEST = Hex::DIRECTIONS[2];

}

namespace std {
	template<>
	struct hash<TCSolver::Hex> {
		size_t operator()(const TCSolver::Hex& pos) const noexcept {
			uint64_t a = static_cast<uint64_t>(pos.i);
			uint64_t b = static_cast<uint64_t>(pos.j);
			return a ^ (b + 0x9E3779B97F4A7C15ULL + (a << 6) + (a >> 2));
		}
	};
};
