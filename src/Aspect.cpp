#include "Aspect.hpp"

bool Aspect::isRelated(const Aspect& other) const {
	return
		parent1 == other.id || parent2 == other.id
		|| other.parent1 == id || other.parent2 == id;
}