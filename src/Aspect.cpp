#include "Aspect.hpp"

#include <iostream>

short Aspect::count = 0;

Aspect::Aspect() : id(count++), parent1(-1), parent2(-1) {}

Aspect::Aspect(short parent1, short parent2)
	: id(count++), parent1(parent1), parent2(parent2) {}

Aspect::Aspect(const Aspect& other)
	: id(other.id), parent1(other.parent1), parent2(other.parent2) {}

Aspect::Aspect(Aspect&& other) noexcept
	: id(other.id), parent1(other.parent1), parent2(other.parent2) {}

short Aspect::getId() const {
	return id;
}

short Aspect::getParent1() const {
	return parent1;
}

short Aspect::getParent2() const {
	return parent2;
}

bool Aspect::isRelated(const Aspect& other) const {
	return
		parent1 == other.id || parent2 == other.id
		|| other.parent1 == id || other.parent2 == id;
}

Aspect& Aspect::operator=(const Aspect& other) {
	id = other.id;
	parent1 = other.parent1;
	parent2 = other.parent2;
	return *this;
}

Aspect& Aspect::operator=(Aspect&& other) noexcept {
	id = other.id;
	parent1 = other.parent1;
	parent2 = other.parent2;
	return *this;
}