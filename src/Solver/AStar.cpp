#include <iostream>
#include <queue>
#include <unordered_map>

#include "AStar.hpp"
#include "Graph.hpp"

std::vector<TCSolver::AStar::State> TCSolver::AStar::Solve(const Graph& graph, Hex start, Hex end) {
	std::priority_queue<State> openSet;
	std::unordered_map<Hex, int32_t> gCosts;
	std::unordered_map<State, State> parents;

	const Config& config = graph.GetConfig();
	const std::unordered_set<Hex>& terminals = graph.GetTerminals();
	const std::vector<Aspect>& aspects = config.GetAspects();
	int32_t gridSize = config.GetGridSize();
	static constexpr int32_t MAX_INT = std::numeric_limits<int32_t>::max();

	openSet.push({
		start,
		graph.At(start).GetAspectId(),
		Hex::Distance(start, end), // hCost
		0, // gCost
		aspects[graph.At(start).GetAspectId()].GetTier(),
		graph.GetPlacementMask()
	});

	while (!openSet.empty()) {
		State currentState = openSet.top();
		openSet.pop();

		if (currentState.position == end) {
			std::vector<State> path;
			while (currentState.position != start) {
				path.push_back(currentState);
				currentState = parents.at(currentState);
			}
			std::reverse(path.begin(), path.end());
			return path;
		}

		for (const Hex& neighbor : currentState.position.GetNeighboringPositions()) {
			if (Hex::Distance(neighbor, Hex::ZERO) >= gridSize) continue;

			int32_t gCost = currentState.gCost + 1;
			const auto it = gCosts.find(neighbor);
			int32_t neighborGCost = it == gCosts.end() ? MAX_INT : it->second;

			if (gCost >= neighborGCost) continue;

			if (currentState.placementMask & Graph::HEX_ENCODINGS.at(neighbor)) {
				if (!terminals.contains(neighbor)) continue; // If we're not looking at a terminal (aka backtracking)

				int32_t existingAspect = graph.At(neighbor).GetAspectId();
				const std::vector<int32_t>& links = aspects[currentState.aspectId].GetLinks();
				if (std::find(links.begin(), links.end(), existingAspect) == links.end()) continue;

				State newState = {
					neighbor,
					existingAspect,
					Hex::Distance(neighbor, end),
					gCost,
					aspects[existingAspect].GetTier(),
					currentState.placementMask | Graph::HEX_ENCODINGS.at(neighbor)
				};

				openSet.push(newState);
				gCosts.insert_or_assign(neighbor, gCost);
				parents.insert_or_assign(newState, currentState);
			} else {
				gCosts.insert_or_assign(neighbor, gCost);

				for (int32_t aspectId : aspects[currentState.aspectId].GetLinks()) {
					State newState = {
						neighbor,
						aspectId,
						Hex::Distance(neighbor, end),
						gCost,
						aspects[aspectId].GetTier(),
						currentState.placementMask | Graph::HEX_ENCODINGS.at(neighbor)
					};

					openSet.push(newState);
					parents.insert_or_assign(newState, currentState);
				}
			}
		}
	}

	return std::vector<State>();
}
