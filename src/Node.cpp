#include "Node.hpp"

Node::Node(Hex position, const Aspect& aspect)
	: position(position), aspect(aspect) {}

Node::Node(int i, int j, int k, const Aspect& aspect)
	: Node(Hex(i, j, k), aspect) {}

const Hex& Node::getPosition() const {
	return position;
}

const Aspect& Node::getAspect() const {
	return aspect;
}