#include <iostream>

#include "DreyfusWagner.hpp"
#include "Solver.hpp"

bool TCSolver::DreyfusWagner::Solve(const Graph& graph) {
	static constexpr int32_t MAX_INT = std::numeric_limits<int32_t>::max();

	// Remove the first terminal to later use as the root for the final part of the algorithm
	std::unordered_set<Hex> terminals = graph.GetTerminals();
	Hex rootTerminal = *terminals.begin();
	terminals.erase(terminals.begin());

	// map[fromTerminalSetOrNodeMask, toNode] = distance
	std::unordered_map<uint128_t, std::unordered_map<uint128_t, int32_t>> dp;
	dp.reserve(std::max<size_t>(4, terminals.size() * 2));

	std::unordered_set<uint128_t> allNodesSet;
	allNodesSet.reserve(200000); // Heuristic from testing

	// parents map used by Dijkstra as before
	std::unordered_map<uint128_t, uint128_t> parents;
	parents.reserve(262144); // Heuristic from testing

	// 1. (Base case) Find the distance from each terminal to every other reachable node

	Dijkstra(graph, graph.GetTerminals(), dp, parents, allNodesSet);

	// Convert allNodesSet to a vector for much faster iteration
	std::vector<uint128_t> allNodes(allNodesSet.begin(), allNodesSet.end());

	// 2. Iterate over all combinatorial subsets of the terminals that are not empty and not equal to the full set

	uint64_t fullTerminalMask = 0;
	for (const Hex& terminal : terminals) {
		fullTerminalMask |= Graph::HEX_ENCODINGS[terminal];
	}

	TerminalSubsets_t terminalSubsets = GetTerminalSubsets(fullTerminalMask);

	// 3. For each subset...
	while (!terminalSubsets.empty()) {
		uint64_t subsetD = terminalSubsets.top();
		terminalSubsets.pop();

		// Break down the mask into an iterable list of terminal positions
		std::vector<uint64_t> terminalPositions;
		for (int32_t i = 0; i < 64; ++i) {
			if (subsetD & (1ULL << i)) terminalPositions.push_back(1ULL << i);
		}

		int32_t count1 = 0;
		int32_t count2 = 0;

		// 4. For each node (J)...
		for (uint128_t nodeMaskJ : allNodes) {
			int32_t minDistance = MAX_INT;

			// 5. Remove a single terminal (E) from the subset (D)
			// and find the minimum of distance(E,J) + distance(D-E,J)
			for (uint64_t terminalE : terminalPositions) {
				uint128_t subsetDMinusEMask = {subsetD ^ terminalE, 0};
				uint128_t terminalEMask = {terminalE, 0};

				auto itDMinusE = dp.find(subsetDMinusEMask);
				if (itDMinusE == dp.end()) continue;
				auto itDMinusEtoJ = itDMinusE->second.find(nodeMaskJ);
				if (itDMinusEtoJ == itDMinusE->second.end()) continue;
				int32_t distanceDMinusEToJ = itDMinusEtoJ->second;

				auto itE = dp.find(terminalEMask);
				if (itE == dp.end()) continue;
				auto itEtoJ = itE->second.find(nodeMaskJ);
				if (itEtoJ == itE->second.end()) continue;
				int32_t distanceEToJ = itEtoJ->second;

				if (distanceDMinusEToJ != MAX_INT && distanceEToJ != MAX_INT)
					minDistance = std::min(minDistance, distanceDMinusEToJ + distanceEToJ);
			}

			++count1;
			if (minDistance == MAX_INT) continue;
			++count2;

			// 6. For each node (I), dp[D][I] = min(dp[D][I], dp[J][I] + minDistance)
			uint128_t subsetDMask = {subsetD, 0};
			// Ensure dp[subsetDMask] exists (needs to be written to). Use reference to reduce lookups.
			std::unordered_map<uint128_t, int32_t>& dpSubsetDMap = dp[subsetDMask];

			// Instead of iterating over allNodes, iterate only over dp[nodeMaskJ]'s keys
			auto itDpJ = dp.find(nodeMaskJ);
			if (itDpJ == dp.end()) continue;
			std::unordered_map<uint128_t, int32_t>& dpJMap = itDpJ->second;

			for (const auto& [nodeMaskI, dpJtoI] : dpJMap) {
				
				// find current dp[D][I] if present
				auto itDpDtoI = dpSubsetDMap.find(nodeMaskI);
				int32_t dpDtoI = itDpDtoI == dpSubsetDMap.end() ? MAX_INT : itDpDtoI->second;

				// Candidate cost
				// if dpJtoI is MAX_INT, it's not useful; but dpJMap values are actual entries, so this is safe.
				int32_t candidate = dpJtoI == MAX_INT ? MAX_INT : dpJtoI + minDistance;
				if (candidate < dpDtoI) dpSubsetDMap[nodeMaskI] = candidate;
			}
		}
	}

	// Break down the mask into an iterable list of terminal positions
	std::vector<uint64_t> fullTerminalPositions;
	for (int32_t i = 0; i < 64; ++i) {
		if (fullTerminalMask & (1ULL << i)) fullTerminalPositions.push_back(1ULL << i);
	}

	// 7. For each node (J)...
	uint128_t rootTerminalMask = Solver::GetMask(rootTerminal, 0);
	int32_t steinerDistance = MAX_INT;
	auto itRoot = dp.find(rootTerminalMask);
	if (itRoot == dp.end()) throw std::runtime_error("Root terminal not found");

	for (uint128_t nodeMaskJ : allNodes) {
		int32_t minDistance = MAX_INT;

		// 8. Remove a single terminal (E) from the subset (D)
		// and find the minimum of distance(E,J) + distance(D-E,J)
		for (uint64_t terminalE : fullTerminalPositions) {
			uint128_t subsetDMinusEMask = {fullTerminalMask ^ terminalE, 0};
			uint128_t terminalEMask = {terminalE, 0};

			auto itDMinusE = dp.find(subsetDMinusEMask);
			auto itE = dp.find(terminalEMask);
			if (itDMinusE == dp.end() || itE == dp.end()) continue;
			
			auto itDMinusEtoJ = itDMinusE->second.find(nodeMaskJ);
			auto itEtoJ = itE->second.find(nodeMaskJ);
			if (itDMinusEtoJ == itDMinusE->second.end() || itEtoJ == itE->second.end()) continue;
			
			int32_t distanceDMinusEtoJ = itDMinusEtoJ->second;
			int32_t distanceEtoJ = itEtoJ->second;

			if (distanceDMinusEtoJ != MAX_INT && distanceEtoJ != MAX_INT)
				minDistance = std::min(minDistance, distanceDMinusEtoJ + distanceEtoJ);
		}

		// 9. Find the minimum of dp[root][J] + min(dp[D-E][J] + dp[E][J])
		auto itRootToJ = itRoot->second.find(nodeMaskJ);
		if (itRootToJ == itRoot->second.end()) continue;
		int32_t distanceRootToJ = itRootToJ->second;

		if (distanceRootToJ != MAX_INT && minDistance != MAX_INT)
			steinerDistance = std::min(steinerDistance, distanceRootToJ + minDistance);
	}

	std::cout << "Steiner distance: " << steinerDistance << std::endl;

	return steinerDistance < MAX_INT;
}

void TCSolver::DreyfusWagner::Dijkstra(
	const Graph& graph,
	const std::unordered_set<Hex>& initialPositions,
	std::unordered_map<uint128_t, std::unordered_map<uint128_t, int32_t>>& dp,
	std::unordered_map<uint128_t, uint128_t>& parents,
	std::unordered_set<uint128_t>& allNodes
) {
	static constexpr int32_t MAX_INT = std::numeric_limits<int32_t>::max();

	std::priority_queue<State> openSet;

	const std::unordered_set<Hex>& terminals = graph.GetTerminals();
	const std::vector<Aspect>& aspects = graph.GetConfig().GetAspects();
	uint64_t placementMask = graph.GetPlacementMask();

	// TODO: parallelize
	for (Hex terminalPosition : initialPositions) {
		int32_t terminalAspectId = graph.At(terminalPosition).GetAspectId();
		openSet.push({
			0,
			terminalAspectId,
			terminalPosition,
			placementMask
		});

		uint128_t pureTerminalMask = Solver::GetMask(terminalPosition, 0);
		uint128_t terminalNodeMask = Solver::GetMask(placementMask, terminalAspectId);

		// Ensure inner maps exist and reserve a bit. Needs more testing to figure out heuristic reserve amount
		std::unordered_map<uint128_t, int32_t>& dpPure = dp[pureTerminalMask];
		dpPure.reserve(1024);
		std::unordered_map<uint128_t, int32_t>& dpTerminal = dp[terminalNodeMask];
		dpTerminal.reserve(1024);

		dp[pureTerminalMask][terminalNodeMask] = 0;
		dp[terminalNodeMask][terminalNodeMask] = 0;

		while (!openSet.empty()) {
			State currentState = openSet.top();
			openSet.pop();

			for (const Hex& neighbor : currentState.position.GetNeighboringPositions()) {
				if (Hex::Distance(neighbor, Hex::ZERO) >= graph.GetSideLength()) continue;

				uint64_t neighborPositionMask = Graph::HEX_ENCODINGS.at(neighbor);
				uint64_t combinedMask = currentState.placementMask | neighborPositionMask;

				if (currentState.placementMask & neighborPositionMask) {
					// If we're not looking at a terminal (aka backtracking)
					if (!terminals.contains(neighbor)) continue;

					int32_t existingAspect = graph.At(neighbor).GetAspectId();
					if (!aspects[currentState.aspectId].GetLinks().contains(existingAspect)) continue;

					uint128_t neighborNodeMask = Solver::GetMask(combinedMask, existingAspect);
					auto itPureToNeighbor = dpPure.find(neighborNodeMask);
					int32_t dpCost = itPureToNeighbor == dpPure.end() ? MAX_INT : itPureToNeighbor->second;

					int32_t cost = currentState.cost; // Don't add anything -- Using an existing aspect not placed by us
					if (dpCost <= cost) continue;

					dpPure[neighborNodeMask] = cost;
					dp[neighborNodeMask][terminalNodeMask] = cost;
					auto dpNeighbor = dp.find(neighborNodeMask);
					if (dpNeighbor == dp.end()) throw std::runtime_error("dp neighbor not found");

					uint128_t parentMask = Solver::GetMask(currentState.placementMask, currentState.aspectId);
					parents[neighborNodeMask] = parentMask;

					// Fill reverse cumulative costs along parent chain
					while (parentMask != terminalNodeMask) {
						dpNeighbor->second[parentMask] = cost - dpPure[parentMask];
						parentMask = parents[parentMask];
					}

					openSet.push({
						cost,
						existingAspect,
						neighbor,
						combinedMask
					});
					allNodes.insert(neighborNodeMask);
				} else {
					for (int32_t aspectId : aspects[currentState.aspectId].GetLinks()) {
						uint128_t neighborNodeMask = Solver::GetMask(combinedMask, aspectId);
						
						auto itPureToNeighbor = dpPure.find(neighborNodeMask);
						int32_t dpCost = itPureToNeighbor == dpPure.end() ? MAX_INT : itPureToNeighbor->second;

						int32_t cost = currentState.cost + 1;
						if (dpCost <= cost) continue;

						dpPure[neighborNodeMask] = cost;
						dp[neighborNodeMask][terminalNodeMask] = cost;
						auto dpNeighbor = dp.find(neighborNodeMask);
						if (dpNeighbor == dp.end()) throw std::runtime_error("dp neighbor not found");

						uint128_t parentMask = Solver::GetMask(currentState.placementMask, currentState.aspectId);
						parents[neighborNodeMask] = parentMask;

						// Fill reverse cumulative costs along parent chain
						while (parentMask != terminalNodeMask) {
							dpNeighbor->second[parentMask] = cost - dpPure[parentMask];
							parentMask = parents[parentMask];
						}

						openSet.push({
							cost,
							aspectId,
							neighbor,
							combinedMask
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
	for (int32_t i = 1; i <= (1 << numTerminals) - 1; ++i) {
		uint64_t subsetMask = 0;
		for (int32_t j = 0; j < numTerminals; ++j) {
			if (i & (1 << j)) subsetMask |= (1 << terminalPositions[j]);
		}

		if (subsetMask != 0 && std::popcount(subsetMask) > 1) subsets.emplace(subsetMask);
	}

	return subsets;
}
