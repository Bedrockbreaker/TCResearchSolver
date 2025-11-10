#include <iostream>

#include "DreyfusWagner.hpp"
#include "Solver.hpp"

std::vector<TCSolver::DreyfusWagner::State> TCSolver::DreyfusWagner::Solve(const Graph& graph) {
	static constexpr int32_t MAX_INT = std::numeric_limits<int32_t>::max();
	
	// Remove the first terminal to later use as the root for the final part of the algorithm
	std::unordered_set<Hex> terminals = graph.GetTerminals();
	Hex rootTerminal = *terminals.begin();
	terminals.erase(terminals.begin());

	// map[fromTerminalSet, toNode] = distance
	std::unordered_map<uint64_t, std::unordered_map<uint16_t, int32_t>> dp(terminals.size());
	std::unordered_set<uint16_t> allNodes;

	// 1. (Base case) Find the distance from each terminal to every other reachable node

	std::unordered_set<uint16_t> initialNodes;
	for (const Hex& terminal : terminals) {
		initialNodes.insert(Solver::GetMask(terminal, graph.At(terminal).GetAspectId()));
	}

	Dijkstra(graph, initialNodes, dp, allNodes);

	// 1.5. Find the distance between every pair reachable nodes

	std::unordered_set<uint16_t> _; // Do not care about the added nodes
	Dijkstra(graph, allNodes, dp, _);

	// 2. Iterate over all combinatorial subsets of the terminals that are not empty and not equal to the full set

	uint64_t fullTerminalMask = 0;
	for (const Hex& terminal : terminals) {
		fullTerminalMask |= Graph::HEX_ENCODINGS[terminal];
	}

	TerminalSubsets_t terminalSubsets = GetTerminalSubsets(fullTerminalMask);

	// 3. For each subset...
	while (!terminalSubsets.empty()) {
		uint64_t subset = terminalSubsets.top();
		terminalSubsets.pop();

		// Break down the mask into an iterable list of terminal positions
		std::vector<uint64_t> terminalPositions;
		for (int32_t i = 0; i < 64; ++i) {
			if (subset & (1ULL << i)) terminalPositions.push_back(1ULL << i);
		}

		// 4. For each node (J)...
		for (uint16_t nodeMaskJ : allNodes) {
			int32_t minDistance = MAX_INT;

			// 5. Remove a single terminal (E) from the subset (D)
			// and find the minimum of distance(E,J) + distance(D-E,J)
			for (uint64_t terminalE : terminalPositions) {
				uint64_t subsetDMinusE = subset ^ terminalE;
				int32_t distanceDMinusEToJ = dp[subsetDMinusE][nodeMaskJ];
				int32_t distanceEToJ = dp[terminalE][nodeMaskJ];
				minDistance = std::min(minDistance, distanceDMinusEToJ + distanceEToJ);
			}

			// 6. For each node (I), dp[D][I] = min(dp[D][I], dp[J][I] + minDistance)
			for (uint16_t nodeMaskI : allNodes) {
				int32_t dpDI = dp[subset].find(nodeMaskI) == dp[subset].end() ? MAX_INT : dp[subset][nodeMaskI];
				dp[subset][nodeMaskI] = std::min(dpDI, dp[subset][nodeMaskJ] + minDistance);
			}
		}
	}

	// Break down the mask into an iterable list of terminal positions
	std::vector<uint64_t> fullTerminalPositions;
	for (int32_t i = 0; i < 64; ++i) {
		if (fullTerminalMask & (1ULL << i)) fullTerminalPositions.push_back(1ULL << i);
	}

	// 7. For each node (J)...
	uint64_t rootTerminalMask = Graph::HEX_ENCODINGS[rootTerminal];
	int32_t steinerDistance = MAX_INT;
	for (uint16_t nodeMaskJ : allNodes) {
		int32_t minDistance = MAX_INT;

		// 8. Remove a single terminal (E) from the subset (D)
		// and find the minimum of distance(E,J) + distance(D-E,J)
		for (uint64_t terminalE : fullTerminalPositions) {
			uint64_t subsetDMinusE = fullTerminalMask ^ terminalE;
			int32_t distanceDMinusEToJ = dp[subsetDMinusE][nodeMaskJ];
			int32_t distanceEToJ = dp[terminalE][nodeMaskJ];
			minDistance = std::min(minDistance, distanceDMinusEToJ + distanceEToJ);
		}

		// 9. Find the minimum of dp[root][J] + min(dp[D-E][J] + dp[E][J])
		steinerDistance = std::min(steinerDistance, dp[rootTerminalMask][nodeMaskJ] + minDistance);
	}

	std::cout << "Steiner distance: " << steinerDistance << std::endl;

	return {};
}

void TCSolver::DreyfusWagner::Dijkstra(
	const Graph& graph,
	const std::unordered_set<uint16_t>& initialNodes,
	std::unordered_map<uint64_t, std::unordered_map<uint16_t, int32_t>>& dp,
	std::unordered_set<uint16_t>& allNodes
) {
	static constexpr int32_t MAX_INT = std::numeric_limits<int32_t>::max();

	std::priority_queue<State> openSet;

	const std::unordered_set<Hex>& terminals = graph.GetTerminals();
	const std::vector<Aspect>& aspects = graph.GetConfig().GetAspects();
	uint64_t placementMask = graph.GetPlacementMask();

	// TODO: parallelize
	for (uint16_t terminalNodeMask : initialNodes) {
		int32_t terminalAspectId = static_cast<int32_t>(terminalNodeMask >> 8);
		Hex terminalPosition = Graph::HEX_DECODINGS_PACKED[(terminalNodeMask & 0xFF) - 1];
		openSet.push({
			0,
			terminalAspectId,
			terminalPosition,
			placementMask
		});

		uint64_t terminalPositionMask = Graph::HEX_ENCODINGS[terminalPosition];
		dp[terminalPositionMask][terminalNodeMask] = 0;

		while (!openSet.empty()) {
			State currentState = openSet.top();
			openSet.pop();

			for (const Hex& neighbor : currentState.position.GetNeighboringPositions()) {
				if (Hex::Distance(neighbor, Hex::ZERO) >= graph.GetSideLength()) continue;

				uint64_t neighborPositionMask = Graph::HEX_ENCODINGS.at(neighbor);

				if (currentState.placementMask & neighborPositionMask) {
					// If we're not looking at a terminal (aka backtracking)
					if (!terminals.contains(neighbor)) continue;

					int32_t existingAspect = graph.At(neighbor).GetAspectId();
					const std::vector<int32_t>& links = aspects[currentState.aspectId].GetLinks();
					if (std::find(links.begin(), links.end(), existingAspect) == links.end()) continue;

					uint16_t neighborNodeMask = Solver::GetMask(neighbor, existingAspect);
					int32_t cost = currentState.cost + 1;

					const auto it = dp[terminalPositionMask].find(neighborNodeMask);
					int32_t dpCost = it == dp[terminalPositionMask].end() ?  MAX_INT : it->second;

					if (dpCost <= cost) continue;

					dp[terminalPositionMask][neighborNodeMask] = cost;
					openSet.push({
						cost,
						existingAspect,
						neighbor,
						currentState.placementMask | neighborPositionMask
					});
					allNodes.insert(neighborNodeMask);
				} else {
					for (int32_t aspectId : aspects[currentState.aspectId].GetLinks()) {
						uint16_t neighborNodeMask = Solver::GetMask(neighbor, aspectId);
						int32_t cost = currentState.cost + 1;

						const auto it = dp[terminalPositionMask].find(neighborNodeMask);
						int32_t dpCost = it == dp[terminalPositionMask].end() ?  MAX_INT : it->second;

						if (dpCost <= cost) continue;

						dp[terminalPositionMask][neighborNodeMask] = cost;
						openSet.push({
							cost,
							aspectId,
							neighbor,
							currentState.placementMask | neighborPositionMask
						});
						allNodes.insert(neighborNodeMask);
					}
				}
			}
		}
	}
}

constexpr TCSolver::DreyfusWagner::TerminalSubsets_t TCSolver::DreyfusWagner::GetTerminalSubsets(
	uint64_t terminalMask
) noexcept {
	int32_t numTerminals = 0;
	std::vector<uint64_t> terminalPositions;
	for (int32_t i = 0; i < 64; ++i) {
		if (terminalMask & (1ULL << i)) {
			terminalPositions.push_back(i);
			++numTerminals;
		}
	}

	TerminalSubsets_t subsets;

	for (int32_t i = 1; i < (1 << numTerminals) - 1; ++i) {
		uint64_t subsetMask = 0;
		for (int32_t j = 0; j < numTerminals; ++j) {
			if (i & (1 << j)) subsetMask |= (1 << terminalPositions[j]);
		}

		if (subsetMask != 0 && std::popcount(subsetMask) > 1) subsets.emplace(subsetMask);
	}

	return subsets;
}
