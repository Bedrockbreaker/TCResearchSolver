#include <iostream>
#include <queue>
#include <unordered_map>

#include "AStar.hpp"
#include "Solver.hpp"

bool TCSolver::AStar::Solve(const Graph& graph, Hex start, Hex end, std::vector<State>& path) {
	std::priority_queue<State> openSet;
	std::unordered_map<uint128_t, int32_t> gCosts;
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
			while (currentState.position != start) {
				path.push_back(currentState);
				currentState = parents.at(currentState);
			}
			std::reverse(path.begin(), path.end());
			return true;
		}

		for (const Hex& neighbor : currentState.position.GetNeighboringPositions()) {
			if (Hex::Distance(neighbor, Hex::ZERO) >= gridSize) continue;

			if (currentState.placementMask & Graph::HEX_ENCODINGS.at(neighbor)) {
				if (!terminals.contains(neighbor)) continue; // If we're not looking at a terminal (aka backtracking)

				int32_t existingAspect = graph.At(neighbor).GetAspectId();
				if (!aspects[currentState.aspectId].GetLinks().contains(existingAspect)) continue;

				uint128_t neighborMask = Solver::GetMask(neighbor, existingAspect);
				const auto it = gCosts.find(neighborMask);
				int32_t neighborGCost = it == gCosts.end() ? MAX_INT : it->second;

				int32_t gCost = currentState.gCost; // Don't add anything -- Using an existing aspect not placed by us
				if (gCost >= neighborGCost) continue;

				State newState = {
					neighbor,
					existingAspect,
					Hex::Distance(neighbor, end),
					gCost,
					aspects[existingAspect].GetTier(),
					currentState.placementMask | Graph::HEX_ENCODINGS.at(neighbor)
				};

				openSet.push(newState);
				gCosts.insert_or_assign(neighborMask, gCost);
				parents.insert_or_assign(newState, currentState);
			} else {
				for (int32_t aspectId : aspects[currentState.aspectId].GetLinks()) {
					uint128_t neighborMask = Solver::GetMask(neighbor, aspectId);
					const auto it = gCosts.find(neighborMask);
					int32_t neighborGCost = it == gCosts.end() ? MAX_INT : it->second;

					int32_t gCost = currentState.gCost + 1;
					if (gCost >= neighborGCost) continue;

					State newState = {
						neighbor,
						aspectId,
						Hex::Distance(neighbor, end),
						gCost,
						aspects[aspectId].GetTier(),
						currentState.placementMask | Graph::HEX_ENCODINGS.at(neighbor)
					};

					openSet.push(newState);
					gCosts.insert_or_assign(neighborMask, gCost);
					parents.insert_or_assign(newState, currentState);
				}
			}
		}
	}

	return false;
}
