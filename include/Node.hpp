#pragma once

#include "HexPosition.hpp"

class Node {

private:
	HexPosition position;

public:
	const HexPosition& getPosition() const;

	Node(HexPosition position);
	Node(int i, int j, int k);
};