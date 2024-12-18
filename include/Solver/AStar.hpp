#pragma once

#include <queue>
#include <unordered_map>

#include "Config.hpp"
#include "Graph.hpp"
#include "Hex.hpp"
#include "Solver.hpp"

namespace TCSolver::Solvers {

struct AStarState {
	Hex position;
	Graph graph;
	int hCost;
	int pathLength;
	int tier;

	AStarState(Hex position, Graph&& graph, int hCost, int pathLength, int tier);
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

public:
	AStar() = default;

	Graph Solve(const Graph& graph) override;
};

}