#pragma once

#include <memory>

#include "Aspect.hpp"
#include "HexPosition.hpp"

class Node {

private:
	HexPosition position;
	std::unique_ptr<Aspect> aspect;

public:
	Node(HexPosition position);
	Node(int i, int j, int k);

	const HexPosition& getPosition() const;
	const Aspect& getAspect() const;
};