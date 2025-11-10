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

	related.emplace(parent1);
	related.emplace(parent2);
}

void TCSolver::Aspect::AddRelated(int32_t other) {
	if (other <= -1) throw std::invalid_argument("Related aspect id must be non-negative");

	// Intentionally throw if related already exists
	if (related.contains(other)) throw std::invalid_argument("Related aspect already exists");

	related.emplace(other);
}
