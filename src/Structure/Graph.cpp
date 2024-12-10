#include <algorithm>
#include <iostream>
#include <sstream>

#include "Graph.hpp"
#include "GraphError.hpp"

using namespace TCSolver;

Graph::Graph(int sideLength) : sideLength(sideLength) {
	for (int j = 1 - sideLength; j < sideLength; j++) {
		for (
			int i = std::max(1 - sideLength - j, 1 - sideLength);
			i <= std::min(sideLength - 1, sideLength - 1 - j);
			i++
		) {
			Hex pos(i, j);
			nodes.try_emplace(Hex(i, j), Node(pos, nullptr));
		}
	}
}

Graph::Graph(const Graph& other)
	: sideLength(other.sideLength), nodes(other.nodes), terminals(other.terminals) {}

Graph::Graph_t::iterator Graph::begin() {
	return nodes.begin();
}

Graph::Graph_t::iterator Graph::end() {
	return nodes.end();
}

Graph::Graph_t::const_iterator Graph::begin() const noexcept {
	return nodes.cbegin();
}

Graph::Graph_t::const_iterator Graph::end() const noexcept {
	return nodes.cend();
}

const Node& Graph::at(Hex position) const {
	return nodes.at(position);
}

int Graph::GetSideLength() const {
	return sideLength;
}

const void Graph::MergeTerminals(const std::vector<Node>& newTerminals) {
	static const Hex origin(0, 0);

	for (auto& node : newTerminals) {
		Hex pos = node.getPosition();

		if (origin.Distance(pos) >= sideLength) {
			throw Error::GraphError(
				"Node with position " + pos.toString() +
				" is out of bounds"
			);
		}

		auto nodeIt = nodes.find(pos);
		auto terminalIt = std::find(terminals.begin(), terminals.end(), &(nodeIt->second));
		
		if (node.getAspect() == nullptr) {
			if (nodeIt != nodes.end()) {			
				nodes.erase(nodeIt);
			}

			if (terminalIt != terminals.end()) {
				terminals.erase(terminalIt);
			}
		} else {
			const auto [it, _] = nodes.insert_or_assign(pos, std::move(node));
			if (terminalIt == terminals.end()) {
				terminals.push_back(&(it->second));
			} else {
				*terminalIt = &(it->second);
			}
		}
	}
}

const std::vector<Node*>& Graph::GetTerminals() const {
	return terminals;
}

bool Graph::Bounds(Hex position) const {
	return
		position.Distance(Hex(0, 0)) < sideLength
		&& nodes.find(position) != nodes.end();
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