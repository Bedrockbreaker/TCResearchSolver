#include <algorithm>
#include <iostream>
#include <sstream>

#include "Graph.hpp"
#include "GraphError.hpp"

using namespace TCSolver;

Graph::Graph(int sideLength, std::shared_ptr<NodeManager> manager)
	: sideLength(sideLength), manager(manager)
{
	for (int j = 1 - sideLength; j < sideLength; j++) {
		for (
			int i = std::max(1 - sideLength - j, 1 - sideLength);
			i <= std::min(sideLength - 1, sideLength - 1 - j);
			i++
		) {
			Upsert(Hex(i, j), nullptr);
		}
	}
}

Graph::Graph(const Graph& other)
	: sideLength(other.sideLength),
	nodes(other.nodes),
	terminals(other.terminals),
	manager(other.manager) {}

Graph::Graph(Graph&& other) noexcept
	: sideLength(other.sideLength),
	nodes(std::move(other.nodes)),
	terminals(std::move(other.terminals)),
	manager(other.manager) {}

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

Graph::NodePtr Graph::Upsert(Hex position, const Aspect* aspect) {
	return Upsert(Node(position, aspect));
}

Graph::NodePtr Graph::Upsert(Node&& nodeRef) {
	Hex position = nodeRef.getPosition();

	if (position.Distance(Hex::ZERO) >= sideLength) {
		throw Error::GraphError(
			"Node with position " + position.toString() +
			" is out of bounds"
		);
	}

	NodePtr node = manager->Move(std::move(nodeRef));
	
	if (IsTerminal(node)) {
		throw Error::GraphError(
			"Node with position " + position.toString() +
			" is a terminal"
		);
	}

	auto nodeIt = nodes.find(position);

	if (nodeIt != nodes.end() && node->getAspect() == nullptr) {
		nodes.erase(nodeIt);
	} else {
		nodes.insert_or_assign(position, node);
	}

	return node;
}

Graph::NodePtr Graph::at(Hex position) const {
	return nodes.at(position);
}

int Graph::GetSideLength() const {
	return sideLength;
}

const void Graph::AddTerminals(std::vector<Node>&& newTerminals) {
	for (auto& nodeRef : newTerminals) {
		Hex position = nodeRef.getPosition();
		NodePtr node = Upsert(std::move(nodeRef));

		auto terminalIt = std::find(terminals.begin(), terminals.end(), node);

		if (node->getAspect() == nullptr) {
			if (terminalIt != terminals.end()) {
				terminals.erase(terminalIt);
			}
		} else {
			if (terminalIt == terminals.end()) {
				terminals.push_back(node);
			} else {
				terminals.erase(terminalIt);
				terminals.push_back(node);
			}
		}
	}
}

const std::vector<Graph::NodePtr>& Graph::GetTerminals() const {
	return terminals;
}

bool Graph::IsTerminal(NodePtr node) const {
	return std::find(terminals.begin(), terminals.end(), node) != terminals.end();
}

std::vector<Graph::NodePtr> Graph::GetNeighbors(Hex position) const {
	std::vector<NodePtr> neighbors;

	for (const auto* aspect : at(position)->getAspect()->getRelated()) {
		for (const auto& hex : position.Neighbours()) {
			if (hex.Distance(Hex::ZERO) >= sideLength) continue;
			const auto it = nodes.find(hex);
			if (it == nodes.end()) continue;
			NodePtr node = manager->ComputeIfAbsent(hex, aspect);
			if (
				it->second->getAspect() != nullptr
				&& !IsTerminal(node)
			) continue;

			neighbors.push_back(node);
		}
	}

	return neighbors;
}

bool Graph::Contains(Hex position) const {
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

			NodePtr node = Contains(pos)
				? nodes.at(pos)
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

Graph& Graph::operator=(const Graph& other) {
	if (this != &other) {
		sideLength = other.sideLength;
		nodes = other.nodes;
		terminals = other.terminals;
		manager = other.manager;
	}
	return *this;
}

Graph& Graph::operator=(Graph&& other) noexcept {
	if (this != &other) {
		sideLength = other.sideLength;
		nodes = std::move(other.nodes);
		terminals = std::move(other.terminals);
		manager = other.manager;
	}
	return *this;
}