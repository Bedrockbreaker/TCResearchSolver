#include <algorithm>
#include <stdexcept>

#include "Hex.hpp"

using namespace TCSolver;

Hex::Hex(int i, int j, int k) : i(i), j(j), k(k) {
	if (i + j + k != 0) {
		throw std::invalid_argument("Sum of i, j, k must be 0");
	}
}

Hex::Hex(int i, int j): Hex(i, j, -i - j) {}

int Hex::Distance(const Hex& other) const {
	return std::max({
		std::abs(i - other.i),
		std::abs(j - other.j),
		std::abs(k - other.k)
	});
}

std::array<Hex, 6> Hex::Neighbours() const {
	return {
		Hex(i + 1, j, k - 1),
		Hex(i + 1, j - 1, k),
		Hex(i, j - 1, k + 1),
		Hex(i - 1, j, k + 1),
		Hex(i - 1, j + 1, k),
		Hex(i, j + 1, k - 1)
	};
}

std::string Hex::toString() const {
	return "Hex("
		+ std::to_string(i) + ", "
		+ std::to_string(j) + ", "
		+ std::to_string(k) + ")";
}

bool Hex::operator==(const Hex& other) const {
	// Hexes are uniquely determined by only 2 coordinates
	return i == other.i && j == other.j;
}