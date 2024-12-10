#pragma once

#include <array>
#include <string>

struct Hex {

	int i;
	int j;
	int k;

	Hex(int i, int j, int k);
	Hex(int i, int j);

	int Distance(const Hex& other) const;

	std::array<Hex, 6> Neighbours() const;

	std::string toString() const;
	bool operator==(const Hex& other) const;
};

namespace std {
	template<>
	struct hash<Hex> {
		size_t operator()(const Hex& pos) const {
			// Hexes are uniquely determined by only 2 coordinates
			return hash<int>()(pos.i) ^ (hash<int>()(pos.j) << 1);
		}
	};
};