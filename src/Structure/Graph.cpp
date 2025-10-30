#include <algorithm>
#include <iostream>
#include <sstream>

#include "Graph.hpp"

TCSolver::Graph::Graph(const Config& config) noexcept : config(config), sideLength(config.GetGridSize()) {
	assert(config.GetGridSize() > 0 && config.GetGridSize() <= 5 && "Grid size must be between 1 and 5");
}

const TCSolver::Node& TCSolver::Graph::Add(Hex position, int32_t aspectId) {
#ifndef NDEBUG
	if (Hex::Distance(position, Hex::ZERO) >= sideLength)
		throw std::runtime_error(std::format("Node with position {} is out of bounds", position.to_string()));

	if (nodes.contains(position))
		throw std::runtime_error(std::format("Node with position {} already exists", position.to_string()));

	if (IsTerminal(position))
		throw std::runtime_error(std::format("Node with position {} is a terminal", position.to_string()));
#endif

	auto [it, bInserted] = nodes.try_emplace(position, position, aspectId);
	return it->second;
}

const TCSolver::Node& TCSolver::Graph::At(Hex position) const {
#ifndef NDEBUG
	if (!nodes.contains(position))
		throw std::runtime_error(std::format("Node with position {} does not exist", position.to_string()));
#endif

	return nodes.at(position);
}

void TCSolver::Graph::AddTerminals(const std::vector<Hex>& newTerminals) {
	std::copy(newTerminals.begin(), newTerminals.end(), std::inserter(terminals, terminals.begin()));
}

uint64_t TCSolver::Graph::GetPlacementMask() const {
	uint64_t mask = 0;
	for (const std::pair<Hex, Node>& pair : nodes) {
		mask |= Graph::HEX_ENCODINGS.at(pair.first);
	}
	return mask;
}

void TCSolver::Graph::Print() const {
	int radius = sideLength - 1;
	
	std::string blank(radius * 7, ' ');
	std::cout
		<< blank << "  _____\n"
		<< blank << " /     \\\n";

	for (int y = -radius * 2; y <= radius * 2; y++) {
		std::stringstream row1;
		std::stringstream row2;

		if (std::abs(y) > radius) {
			std::string blank2((std::abs(y) - sideLength) * 7, ' ');
			if (y < 0) {
				row1 << blank2 << "  _____";
				row2 << blank2 << " /     ";
			} else {
				row1 << blank2 << " \\_____";
				row2 << blank2 << "       ";
			}
		}

		int width = std::abs(y) < radius
			? sideLength - std::abs((y - (radius % 2)) % 2)
			: 2 * radius + 1 - std::abs(y);
		
		for (int x = 0; x < width; x++) {
			int i = y <= -radius
				? y + radius - x
				: std::min((y + radius) / 2, radius) - x;
			int j = 1 - width + 2 * x;

			Hex pos(i, j);

			bool bGraphContainsNode = Contains(pos);

			std::string_view namePart1 = bGraphContainsNode ? "*" : "";
			std::string_view namePart2 = "";

			/* namePart1 = std::to_string(i) + "," + std::to_string(j);
			namePart2 = std::to_string(0 - i - j); */

			if (bGraphContainsNode) {
				const Node& node = nodes.at(pos);
				if (node.GetAspectId() > -1) {
					namePart1 = config.GetAspects()[node.GetAspectId()].GetName();
					if (namePart1.length() > 7) {
						namePart2 = namePart1.substr(namePart1.length() / 2);
						namePart1 = namePart1.substr(0, (namePart1.length() + 1) / 2);
					}
				}
			}

			int padding1 = 7 - namePart1.length();
			int padding2 = 7 - namePart2.length();

			if (x == 0 && std::abs(y) < radius && std::abs((y - (radius % 2)) % 2) == 1) {
				row1 << " \\_____";
				row2 << " /     ";
			}

			row1
				<< "/"
				<< std::string(padding1 / 2, ' ')
				<< namePart1
				<< std::string((padding1 + 1) / 2, ' ');
			row2
				<< "\\"
				<< std::string(padding2 / 2, ' ')
				<< namePart2
				<< std::string((padding2 + 1) / 2, ' ');
			
			if (x < width - 1) {
				row1 << "\\_____";
				row2 << "/     ";
			} else if (std::abs(y) <= radius) {
				row1 << "\\";
				row2 << "/";
				if (std::abs((y - (radius % 2)) % 2) == 1) {
					row1 << "_____/";
					row2 << "     \\";
				}
			}
		}

		if (std::abs(y) > radius) {
			if (y < 0) {
				row1 << "\\_____";
				row2 << "/     \\";
			} else {
				row1 << "\\_____/";
				row2 << "/";
			}
		}

		std::cout << row1.str() << "\n";
		std::cout << row2.str() << "\n";
	}

	std::cout << blank << " \\_____/\n";
}
