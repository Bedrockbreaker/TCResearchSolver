#pragma once

#include <memory>

#include "Aspect.hpp"
#include "Hex.hpp"

class Node {

private:
	Hex position;
	const Aspect& aspect;

public:
	Node(Hex position, const Aspect& aspect);
	Node(int i, int j, int k, const Aspect& aspect);

	const Hex& getPosition() const;
	const Aspect& getAspect() const;
};