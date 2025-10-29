#include <algorithm>
#include <assert.h>
#include <stdexcept>

#include "Aspect.hpp"

TCSolver::Aspect::Aspect(int32_t id, const std::string& name) noexcept : id(id), name(name) {
	assert(id > -1 && "id must be non-negative");
}

TCSolver::Aspect::Aspect(int32_t id, const std::string& name, int32_t parent1, int32_t parent2, int32_t tier) noexcept :
	id(id), name(name), parent1(parent1), parent2(parent2), tier(tier)
{
	assert(id > -1 && "id must be non-negative");
	assert(parent1 > -1 && parent2 > -1 && "parent1 and parent2 must be non-negative");

	related.reserve(2);
	related.push_back(parent1);
	related.push_back(parent2);
}

void TCSolver::Aspect::AddChild(int32_t child) {
	if (child <= -1) throw std::invalid_argument("child must be non-negative");

	// Intentionally throw if child already exists
	if (std::find(children.begin(), children.end(), child) != children.end())
		throw std::invalid_argument("child already exists");

	children.push_back(child);
	related.push_back(child);
}
