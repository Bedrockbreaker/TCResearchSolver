#pragma once

#include <unordered_map>

#include "Hex.hpp"
#include "Node.hpp"

namespace TCSolver {

class Graph {

	using Graph_t = std::unordered_map<Hex, Node>;

private:
	int sideLength;

	Graph_t nodes;
	std::vector<Node*> terminals;

public:
	Graph(int sideLength);
	Graph(const Graph& other);

	Graph_t::iterator begin();
	Graph_t::iterator end();
	Graph_t::const_iterator begin() const noexcept;
	Graph_t::const_iterator end() const noexcept;

	const Node& at(Hex position) const;

	int GetSideLength() const;
	const void MergeTerminals(const std::vector<Node>& newTerminals);
	const std::vector<Node*>& GetTerminals() const;

	bool Bounds(Hex position) const;

	void Print() const;
};

}