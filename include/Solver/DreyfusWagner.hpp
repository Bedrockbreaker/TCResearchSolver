#pragma once

#include <bit>
#include <queue>

#include "Graph.hpp"

namespace TCSolver::DreyfusWagner {

struct State {
public:
	int32_t cost;
	int32_t aspectId;
	Hex position;
	uint64_t placementMask;

	constexpr friend bool operator<(const State& lhs, const State& rhs) noexcept { return lhs.cost > rhs.cost; }
};

std::vector<State> Solve(const Graph& graph);

void Dijkstra(
	const Graph& graph,
	const std::unordered_set<uint16_t>& initialNodes,
	std::unordered_map<uint64_t, std::unordered_map<uint16_t, int32_t>>& dp,
	std::unordered_set<uint16_t>& allNodes
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
