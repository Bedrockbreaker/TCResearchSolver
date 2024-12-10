#include "Node.hpp"

using namespace TCSolver;

Node::Node(Hex position, const Aspect* aspect)
	: position(position), aspect(aspect) {}

Node::Node(int i, int j, int k, const Aspect* aspect)
	: Node(Hex(i, j, k), aspect) {}

Node::Node(const Node& other)
	: position(other.position), aspect(other.aspect) {}

Node::Node(Node&& other) noexcept
	: position(std::move(other.position)),
	aspect(other.aspect) {}

const Hex& Node::getPosition() const {
	return position;
}

const Aspect* Node::getAspect() const {
	return aspect;
}

std::vector<Node> Node::getNeighbors() const {
	std::vector<Node> neighbors;
	
	for (auto aspectPointer : aspect->getRelated()) {
		for (const auto& hex : position.Neighbours()) {
			neighbors.push_back(Node(hex, aspectPointer));
		}
	}

	return neighbors;
}

int Node::Distance(const Node& other) const {
	return position.Distance(other.position);
}

bool Node::operator==(const Node& other) const {
	return position == other.position && aspect == other.aspect;
}

Node& Node::operator=(const Node& other) {
	position = other.position;
	aspect = other.aspect;
	return *this;
}

Node& Node::operator=(Node&& other) noexcept {
	position = std::move(other.position);
	aspect = other.aspect;
	return *this;
}