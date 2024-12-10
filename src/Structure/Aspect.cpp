#include <iostream>

#include "Aspect.hpp"

using namespace TCSolver;

short Aspect::count = 0;

Aspect::Aspect(const std::string& name, int amount)
	: name(name),
	id(count++),
	parent1(nullptr),
	parent2(nullptr),
	tier(1),
	amount(amount) {}

Aspect::Aspect(
	const std::string& name,
	Aspect* parent1,
	Aspect* parent2,
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
	children(std::move(other.children)),
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

const std::vector<const Aspect*>& Aspect::getChildren() const {
	return children;
}

std::vector<const Aspect*> Aspect::getRelated() const {
	std::vector<const Aspect*> related;
	related.insert(related.end(), children.begin(), children.end());
	if (parent1 != nullptr) {
		related.push_back(parent1);
		related.push_back(parent2);
	}
	return related;
}

void Aspect::UpdateParentRelations() const {
	if (parent1 != nullptr) {
		parent1->children.push_back(this);
		parent2->children.push_back(this);
	}
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