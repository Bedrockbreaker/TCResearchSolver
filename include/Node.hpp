#pragma once

#include <memory>

#include "Aspect.hpp"
#include "HexPosition.hpp"

class Node {

private:
	HexPosition position;
	const Aspect& aspect;

public:
	Node(HexPosition position, const Aspect& aspect);
	Node(int i, int j, int k, const Aspect& aspect);

	const HexPosition& getPosition() const;
	const Aspect& getAspect() const;
};