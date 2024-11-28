#pragma once

#include <array>

struct Hex {

	int i;
	int j;
	int k;

	Hex(int i, int j, int k);
	Hex(int i, int j);

	int Distance(const Hex& other) const;

	std::array<Hex, 6> Neighbours() const;

	bool operator==(const Hex& other) const;
};

struct HexHash {
	std::size_t operator()(const Hex& pos) const;
};