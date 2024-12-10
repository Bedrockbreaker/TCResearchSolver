#pragma once

#include <memory>
#include <vector>

#include "Aspect.hpp"
#include "Hex.hpp"

class Node {

private:
	Hex position;
	const Aspect* aspect;
	std::vector<Node*> neighbors;

public:
	Node(Hex position, const Aspect* aspect);
	Node(int i, int j, int k, const Aspect* aspect);
	Node(Node&& other) noexcept;

	const Hex& getPosition() const;
	const Aspect* getAspect() const;
	const std::vector<Node*>& Neighbors() const;
};