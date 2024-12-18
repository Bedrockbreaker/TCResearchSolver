#pragma once

#include <memory>
#include <unordered_map>

#include "Hex.hpp"
#include "Node.hpp"
#include "NodeManager.hpp"

namespace TCSolver {

class Graph {

public:
	using NodePtr = std::shared_ptr<Node>;
	using Graph_t = std::unordered_map<Hex, NodePtr>;

private:
	int sideLength;

	Graph_t nodes;
	std::vector<NodePtr> terminals;

	std::shared_ptr<NodeManager> manager;

public:
	Graph(int sideLength, std::shared_ptr<NodeManager> manager);
	Graph(const Graph& other);
	Graph(Graph&& other) noexcept;

	Graph_t::iterator begin();
	Graph_t::iterator end();
	Graph_t::const_iterator begin() const noexcept;
	Graph_t::const_iterator end() const noexcept;

	NodePtr Upsert(Hex position, const Aspect* aspect);
	NodePtr Upsert(Node&& node);
	NodePtr at(Hex position) const;

	int GetSideLength() const;
	const void AddTerminals(std::vector<Node>&& newTerminals);
	const std::vector<NodePtr>& GetTerminals() const;
	bool IsTerminal(NodePtr node) const;
	std::vector<NodePtr> GetNeighbors(Hex position) const;

	bool Contains(Hex position) const;

	void Print() const;

	Graph& operator=(const Graph& other);
	Graph& operator=(Graph&& other) noexcept;
};

}