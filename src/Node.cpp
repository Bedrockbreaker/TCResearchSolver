#include "Node.hpp"

Node::Node(HexPosition position, const Aspect& aspect)
	: position(position), aspect(aspect) {}

Node::Node(int i, int j, int k, const Aspect& aspect)
	: Node(HexPosition(i, j, k), aspect) {}

const HexPosition& Node::getPosition() const {
	return position;
}

const Aspect& Node::getAspect() const {
	return aspect;
}