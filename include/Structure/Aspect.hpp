#pragma once

#include <string>
#include <unordered_set>

namespace TCSolver {

class Aspect {
public:
	Aspect(int32_t id, const std::string& name) noexcept;
	Aspect(int32_t id, const std::string& name, int32_t parent1, int32_t parent2, int32_t tier) noexcept;

	~Aspect() = default;

	Aspect(Aspect&& other) noexcept = default;
	Aspect& operator=(Aspect&& other) noexcept = default;
	Aspect(const Aspect&) = delete;
	Aspect& operator=(const Aspect&) = delete;

	int32_t GetParent1() const noexcept { return parent1; }
	int32_t GetParent2() const noexcept { return parent2; }

	int32_t GetTier() const noexcept { return tier; }
	
	void AddRelated(int32_t other);
	const std::unordered_set<int32_t>& GetLinks() const noexcept { return related; };

	const std::string& GetName() const noexcept { return name; }

	friend bool operator==(const Aspect& lhs, const Aspect& rhs) { return lhs.id == rhs.id; }
	friend bool operator!=(const Aspect& lhs, const Aspect& rhs) { return !(lhs == rhs); }

private:
	int32_t id = -1;
	int32_t parent1 = -1;
	int32_t parent2 = -1;
	int32_t tier = 1;
	std::unordered_set<int32_t> related;
	std::string name;
};

}
