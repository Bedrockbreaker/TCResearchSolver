#pragma once

#include "Hex.hpp"
#include "Graph.hpp"

namespace TCSolver::AStar {

struct State {
public:
	int32_t aspectId;
	int32_t hCost;
	int32_t gCost;
	int32_t tier;
	Hex position;
	uint64_t placementMask;

	constexpr State() noexcept : position(Hex::ZERO), aspectId(0), hCost(0), gCost(0), tier(1), placementMask(0ULL) {}
	~State() = default;

	constexpr State(State&& other) noexcept = default;
	constexpr State& operator=(State&& other) noexcept = default;
	constexpr State(const State& other) noexcept = default;
	constexpr State& operator=(const State& other) noexcept = default;

	constexpr State(
		Hex position,
		int32_t aspectId,
		int32_t hCost,
		int32_t gCost,
		int32_t tier,
		uint64_t placementMask
	) noexcept :
		position(position),
		aspectId(aspectId),
		hCost(hCost),
		gCost(gCost),
		tier(tier),
		placementMask(placementMask) {}

	constexpr friend bool operator<(const State& lhs, const State& rhs) noexcept {
		int32_t fCost1 = lhs.gCost + lhs.hCost;
		int32_t fCost2 = rhs.gCost + rhs.hCost;
		return fCost1 != fCost2 ? fCost1 > fCost2 : lhs.tier > rhs.tier;
	}

	constexpr friend bool operator==(const State& lhs, const State& rhs) noexcept {
		return lhs.position == rhs.position
			&& lhs.aspectId == rhs.aspectId
			&& lhs.hCost == rhs.hCost
			&& lhs.gCost == rhs.gCost
			&& lhs.tier == rhs.tier
			&& lhs.placementMask == rhs.placementMask;
	}

	constexpr friend bool operator!=(const State& lhs, const State& rhs) noexcept { return !(lhs == rhs); }
};

bool Solve(const Graph& graph, Hex start, Hex end, std::vector<State>& path);

}

static inline uint64_t SplitMix64(uint64_t x) noexcept {
	x += 0x9E3779B97F4A7C15ULL;
	x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
	x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
	return x ^ (x >> 31);
}

namespace std {
template<>
struct hash<TCSolver::AStar::State> {
	size_t operator()(const TCSolver::AStar::State& state) const noexcept {
		uint64_t hash = 0x9E3779B97F4A7C15ULL;
		hash ^= SplitMix64(static_cast<uint64_t>(state.aspectId) | static_cast<uint64_t>(state.hCost) << 32);
		hash ^= SplitMix64(static_cast<uint64_t>(state.gCost) | static_cast<uint64_t>(state.tier) << 32);
		hash ^= std::hash<TCSolver::Hex>()(state.position);
		hash ^= SplitMix64(state.placementMask);
		return static_cast<size_t>(hash);
	}
};
}
