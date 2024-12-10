#pragma once

#include <unordered_map>

#include "Hex.hpp"
#include "Node.hpp"

class Graph {

	using Graph_t = std::unordered_map<Hex, Node>;

private:
	int sideLength;

	Graph_t nodes;
	std::vector<Node*> terminals;

public:
	Graph(int sideLength, std::vector<Node>& constantNodes);

	void Print() const;
};