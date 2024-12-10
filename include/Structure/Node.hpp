#pragma once

#include <memory>
#include <vector>

#include "Aspect.hpp"
#include "Hex.hpp"

namespace TCSolver {

class Node {

private:
	Hex position;
	const Aspect* aspect;

public:
	Node(Hex position, const Aspect* aspect);
	Node(int i, int j, int k, const Aspect* aspect);
	Node(const Node& other);
	Node(Node&& other) noexcept;

	const Hex& getPosition() const;
	const Aspect* getAspect() const;
	std::vector<Node> getNeighbors() const;

	int Distance(const Node& other) const;

	bool operator==(const Node& other) const;

	Node& operator=(const Node& other);
	Node& operator=(Node&& other) noexcept;
};

}

namespace std {
	template<>
	struct hash<TCSolver::Node> {
		size_t operator()(const TCSolver::Node& node) const {
			return hash<TCSolver::Hex>()(node.getPosition())
				^ (hash<const TCSolver::Aspect*>()(node.getAspect()) << 1);
		}
	};
}