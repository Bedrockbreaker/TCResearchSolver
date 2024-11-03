#include <stdexcept>

#include "HexPosition.hpp"

HexPosition::HexPosition(int i, int j, int k) : i(i), j(j), k(k) {
	if (i + j + k != 0) {
		throw std::invalid_argument("Sum of i, j, k must be 0");
	}
}