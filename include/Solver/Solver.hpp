#pragma once

#include "Graph.hpp"

struct uint128_t {
public:
	uint64_t high;
	uint64_t low;

	constexpr uint128_t() : high(0), low(0) {};
	constexpr uint128_t(uint64_t high, uint64_t low) : high(high), low(low) {};

	friend bool operator==(const uint128_t& lhs, const uint128_t& rhs) noexcept
		{ return lhs.high == rhs.high && lhs.low == rhs.low; }
	friend bool operator!=(const uint128_t& lhs, const uint128_t& rhs) noexcept { return !(lhs == rhs); }
};

namespace TCSolver::Solver {

constexpr uint128_t GetMask(uint64_t position, int32_t aspectId) noexcept {
	return {position, static_cast<uint64_t>(aspectId)};
}

constexpr uint128_t GetMask(Hex position, int32_t aspectId) noexcept {
	return {Graph::HEX_ENCODINGS[position], static_cast<uint64_t>(aspectId)};
}

constexpr std::pair<Hex, int32_t> GetNodeFromMask(uint128_t mask) noexcept {
	return {Graph::HEX_DECODINGS.at(mask.high), static_cast<int32_t>(mask.low)};
}

constexpr std::pair<uint64_t, int32_t> SeparateMask(uint128_t mask) noexcept {
	return {mask.high, static_cast<int32_t>(mask.low)};
}

inline uint64_t SplitMix64(uint64_t x) noexcept {
	x += 0x9E3779B97F4A7C15ULL;
	x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
	x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
	return x ^ (x >> 31);
}
}

namespace std {
template<> struct hash<uint128_t> {
	size_t operator()(const uint128_t& uint128) const noexcept {
		uint64_t hash = 0x9E3779B97F4A7C15ULL;
		hash ^= TCSolver::Solver::SplitMix64(uint128.high);
		hash ^= TCSolver::Solver::SplitMix64(uint128.low);
		return static_cast<size_t>(hash);
	}
};
}
