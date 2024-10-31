#include "Node.h"

Node::Node(HexPosition position) : position(position) {}

Node::Node(int i, int j, int k) : Node(HexPosition(i, j, k)) {}

const HexPosition& Node::getPosition() const {
	return position;
}