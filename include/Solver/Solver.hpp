#pragma once

#include "Graph.hpp"

namespace TCSolver::Solver {
static constexpr uint16_t GetMask(Hex position, int32_t aspectId) noexcept {
	return
		static_cast<uint16_t>((aspectId & 0xFF) << 8) // HACK: This assumes that aspectId is less than 256
		| static_cast<uint16_t>(Graph::HEX_ENCODINGS_PACKED[position]);
};
}
