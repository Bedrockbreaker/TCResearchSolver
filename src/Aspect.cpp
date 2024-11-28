#include <iostream>

#include "Aspect.hpp"

short Aspect::count = 0;

Aspect::Aspect(const std::string& name, int amount)
	: name(name), id(count++), parent1(nullptr), parent2(nullptr), tier(1), amount(amount) {}

Aspect::Aspect(
	const std::string& name,
	const Aspect* parent1,
	const Aspect* parent2,
	int amount
) : name(name), id(count++), parent1(parent1), parent2(parent2), amount(amount)
{
	tier = std::max(parent1->getTier(), parent2->getTier()) + 1;
}

Aspect::Aspect(Aspect&& other) noexcept
	: name(std::move(other.name)),
	id(other.id),
	parent1(other.parent1),
	parent2(other.parent2),
	tier(other.tier),
	amount(other.amount) {}

std::string_view Aspect::getName() const noexcept {
	return name;
}

short Aspect::getId() const {
	return id;
}

const Aspect* Aspect::getParent1() const {
	return parent1;
}

const Aspect* Aspect::getParent2() const {
	return parent2;
}

int Aspect::getTier() const {
	return tier;
}

int Aspect::getAmount() const {
	return amount;
}

bool Aspect::isRelated(const Aspect& other) const {
	return
		parent1 == &other || parent2 == &other
		|| other.parent1 == this || other.parent2 == this;
}

Aspect& Aspect::operator=(Aspect&& other) noexcept {
	name = std::move(other.name);
	id = other.id;
	parent1 = other.parent1;
	parent2 = other.parent2;
	tier = other.tier;
	amount = other.amount;
	return *this;
}

bool Aspect::operator==(const Aspect& other) const {
	return id == other.id;
}