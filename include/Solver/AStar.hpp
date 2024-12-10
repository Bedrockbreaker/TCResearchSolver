#pragma once

#include <queue>
#include <unordered_map>

#include "Config.hpp"
#include "Graph.hpp"
#include "Hex.hpp"
#include "Solver.hpp"

namespace TCSolver::Solvers {

struct AStarState {
	Node node;
	int hCost;
	int pathLength;
	int tier;

	AStarState(const Node& node, int hCost, int pathLength, int tier);
	AStarState(const AStarState& other);
	AStarState(AStarState&& other) noexcept;

	bool operator>(const AStarState& other) const;

	AStarState& operator=(const AStarState& other);
	AStarState& operator=(AStarState&& other) noexcept;
};

class AStar : public Solvers::AbstractSolver {

	using PriorityQueue = std::priority_queue<
		AStarState,
		std::vector<AStarState>,
		std::greater<>
	>;

private:

	const std::unordered_map<short, Aspect>& aspects;

public:
	AStar(const Config& config);

	Graph Solve(const Graph& graph) override;
};

}