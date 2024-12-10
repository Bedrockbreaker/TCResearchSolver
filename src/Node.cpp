#include "Node.hpp"

Node::Node(Hex position, const Aspect* aspect)
	: position(position), aspect(aspect) {}

Node::Node(int i, int j, int k, const Aspect* aspect)
	: Node(Hex(i, j, k), aspect) {}

Node::Node(Node&& other) noexcept
	: position(std::move(other.position)),
	aspect(other.aspect),
	neighbors(std::move(other.neighbors)) {}

const Hex& Node::getPosition() const {
	return position;
}

const Aspect* Node::getAspect() const {
	return aspect;
}

const std::vector<Node*>& Node::Neighbors() const {
	return neighbors;
}