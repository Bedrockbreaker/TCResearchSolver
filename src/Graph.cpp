#include <iostream>
#include <sstream>

#include "Graph.hpp"

Graph::Graph(int sideLength, std::vector<Node>& constantNodes)
	: sideLength(sideLength)
{
	std::vector<Hex> nullNodes;
	const Hex origin(0, 0);

	for (auto& node : constantNodes) {
		Hex pos = node.getPosition();

		if (origin.Distance(pos) >= sideLength) {
			throw std::runtime_error(
				"Node with position " + pos.toString() +
				" is out of bounds"
			);
		}

		if (nodes.contains(pos)) {
			throw std::runtime_error(
				"Node with position " + pos.toString() +
				" already exists"
			);
		}

		auto [it, inserted] = nodes.try_emplace(pos, std::move(node));

		if (it->second.getAspect() != nullptr) {
			terminals.push_back(&(it->second));
		} else {
			nullNodes.push_back(pos);
		}
	}

	if (terminals.size() < 2) {
		throw std::runtime_error("There must be at least 2 static nodes with aspects");
	}

	for (int j = 1 - sideLength; j < sideLength; j++) {
		for (
			int i = std::max(1 - sideLength - j, 1 - sideLength);
			i <= std::min(sideLength - 1, sideLength - 1 - j);
			i++
		) {
			Hex pos(i, j);

			if (nodes.contains(pos)) continue;

			nodes.try_emplace(pos, Node(pos, nullptr));
		}
	}

	for (const auto& pos : nullNodes) {
		nodes.erase(pos);
	}
}

void Graph::Print() const {
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

			const Node* node = nodes.contains(pos)
				? &nodes.at(pos)
				: nullptr;

			std::string_view namePart1 = node == nullptr ? "*" : "";
			std::string_view namePart2 = "";

			/* namePart1 = std::to_string(i) + "," + std::to_string(j);
			namePart2 = std::to_string(0 - i - j); */

			if (node != nullptr && node->getAspect() != nullptr) {
				namePart1 = node->getAspect()->getName();
				if (namePart1.length() > 7) {
					namePart2 = namePart1.substr(namePart1.length() / 2);
					namePart1 = namePart1.substr(0, (namePart1.length() + 1) / 2);
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