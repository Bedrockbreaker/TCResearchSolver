#include <iostream>

#include "AStar.hpp"
#include "SolverError.hpp"

using namespace TCSolver;
using namespace TCSolver::Solvers;

AStarState::AStarState(Hex position, Graph&& graph, int hCost, int pathLength, int tier)
	: position(position), graph(graph), hCost(hCost), pathLength(pathLength), tier(tier) {}

AStarState::AStarState(AStarState&& other) noexcept
	: position(other.position),
	graph(std::move(other.graph)),
	hCost(other.hCost),
	pathLength(other.pathLength),
	tier(other.tier) {}

AStarState::AStarState(const AStarState& other)
	: position(other.position),
	graph(other.graph),
	hCost(other.hCost),
	pathLength(other.pathLength),
	tier(other.tier) {}

bool AStarState::operator>(const AStarState& other) const {
	int fCost1 = pathLength + hCost;
	int fCost2 = other.pathLength + other.hCost;
	if (fCost1 != fCost2) return fCost1 > fCost2;
	return tier > other.tier;
}

AStarState& AStarState::operator=(const AStarState& other) {
	position = other.position;
	graph = other.graph;
	hCost = other.hCost;
	pathLength = other.pathLength;
	tier = other.tier;
	return *this;
}

AStarState& AStarState::operator=(AStarState&& other) noexcept {
	position = other.position;
	graph = std::move(other.graph);
	hCost = other.hCost;
	pathLength = other.pathLength;
	tier = other.tier;
	return *this;
}

Graph AStar::Solve(const Graph& startingGraph) {
	if (startingGraph.GetTerminals().size() != 2) {
		throw Error::SolverError("Graph must have exactly two terminals");
	}

	Graph::NodePtr start = startingGraph.GetTerminals()[0];
	Graph::NodePtr end = startingGraph.GetTerminals()[1];

	PriorityQueue openSet;
	std::unordered_map<Graph::NodePtr, int> gCosts;

	gCosts[start] = 0;

	openSet.push({
		start->getPosition(),
		std::move(Graph(startingGraph)),
		start->Distance(*end),
		0,
		start->getAspect()->getTier()
	});

	while (!openSet.empty()) {
		AStarState currentState = std::move(openSet.top());
		openSet.pop();

		Graph::NodePtr node = currentState.graph.at(currentState.position);
		const Graph& graph = currentState.graph;

		if (node == end) return graph;

		for (auto neighbor : graph.GetNeighbors(currentState.position)) {

			int gCost = gCosts[node] + 1;
			const auto it = gCosts.find(neighbor);
			int neighborGCost = it == gCosts.end()
				? std::numeric_limits<int>::max()
				: it->second;

			if (gCost >= neighborGCost) continue;

			gCosts.insert_or_assign(neighbor, gCost);

			Graph newGraph(graph);
			if (!newGraph.IsTerminal(neighbor)) {
				newGraph.Upsert(neighbor->getPosition(), neighbor->getAspect());
			}

			openSet.push({
				neighbor->getPosition(),
				std::move(newGraph),
				neighbor->Distance(*end),
				gCost,
				neighbor->getAspect()->getTier()
			});
		}
	}

	throw Error::SolverError("No path found");
}