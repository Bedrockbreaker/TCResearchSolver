#include "Solver/AStar.hpp"
#include "SolverError.hpp"

using namespace TCSolver;
using namespace TCSolver::Solvers;

AStarState::AStarState(const Node& node, int hCost, int pathLength, int tier)
	: node(node), hCost(hCost), pathLength(pathLength), tier(tier) {}

AStarState::AStarState(AStarState&& other) noexcept
	: node(std::move(other.node)),
	hCost(other.hCost),
	pathLength(other.pathLength),
	tier(other.tier) {}

AStarState::AStarState(const AStarState& other)
	: node(other.node),
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
	node = other.node;
	hCost = other.hCost;
	pathLength = other.pathLength;
	tier = other.tier;
	return *this;
}

AStarState& AStarState::operator=(AStarState&& other) noexcept {
	node = std::move(other.node);
	hCost = other.hCost;
	pathLength = other.pathLength;
	tier = other.tier;
	return *this;
}

AStar::AStar(const Config& config) : aspects(config.GetAspects()) {}

Graph AStar::Solve(const Graph& graph) {
	if (graph.GetTerminals().size() != 2) {
		throw Error::SolverError("Graph must have exactly two terminals");
	}

	const Node& start = *(graph.GetTerminals()[0]);
	const Node& end = *(graph.GetTerminals()[1]);

	PriorityQueue openSet;
	std::unordered_map<Node, int> gCosts;
	std::unordered_map<const Node*, const Node*> parents;

	gCosts[start] = 0;

	openSet.push({start, start.Distance(end), 0, start.getAspect()->getTier()});

	while (!openSet.empty()) {
		AStarState currentState = openSet.top();
		openSet.pop();

		const Node& node = currentState.node;

		if (node == end) {
			std::vector<Node> path;
			
			for (
				const Node* nodePointer = &(gCosts.find(node)->first);
				nodePointer != nullptr;
				nodePointer = parents[nodePointer]
			) {
				path.push_back(*nodePointer);
			}

			Graph solution = graph;
			solution.MergeTerminals(path);
			return solution;
		}

		for (const Node& neighbor : node.getNeighbors()) {
			if (!graph.Bounds(neighbor.getPosition())) continue;

			int gCost = gCosts[node] + 1;
			const auto it = gCosts.find(neighbor);
			int neighborGCost = it == gCosts.end()
				? std::numeric_limits<int>::max()
				: it->second;
			
			bool addedTierCost = false;

			if (gCost == neighborGCost) {
				gCost += neighbor.getAspect()->getTier();
				neighborGCost += it->first.getAspect()->getTier();
				addedTierCost = true;
			}

			if (gCost < neighborGCost) {
				if (addedTierCost) gCost -= neighbor.getAspect()->getTier();
				const auto [it, _] = gCosts.insert_or_assign(std::move(neighbor), gCost);
				parents[&(it->first)] = &gCosts.find(node)->first;
				openSet.push({
					it->first,
					it->first.Distance(end),
					gCost,
					it->first.getAspect()->getTier()
				});
			}
		}
	}

	throw Error::SolverError("No path found");
}