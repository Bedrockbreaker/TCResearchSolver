#pragma once

#include <bit>
#include <queue>

#include "Graph.hpp"
#include "Solver.hpp"

namespace TCSolver::DreyfusWagner {

struct State {
public:
	int32_t cost;
	int32_t aspectId;
	Hex position;
	uint64_t placementMask;

	constexpr friend bool operator<(const State& lhs, const State& rhs) noexcept { return lhs.cost > rhs.cost; }
};

bool Solve(const Graph& graph);

void Dijkstra(
	const Graph& graph,
	const std::unordered_set<Hex>& initialPositions,
	std::unordered_map<uint128_t, std::unordered_map<uint128_t, int32_t>>& dp,
	std::unordered_map<uint128_t, uint128_t>& parents,
	std::unordered_set<uint128_t>& allNodes
);

struct MaskPriority {
	constexpr bool operator()(const uint64_t& lhs, const uint64_t& rhs) const {
		// Return lhs if has fewer set bits
		return std::popcount(lhs) > std::popcount(rhs);
	}
};

typedef std::priority_queue<uint64_t, std::vector<uint64_t>, MaskPriority> TerminalSubsets_t;

constexpr TerminalSubsets_t GetTerminalSubsets(uint64_t terminalMask) noexcept;

}
