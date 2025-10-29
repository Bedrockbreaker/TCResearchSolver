#pragma once

#include <string>
#include <vector>

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

	const std::string& GetName() const noexcept { return name; }

	int32_t GetParent1() const noexcept { return parent1; }
	int32_t GetParent2() const noexcept { return parent2; }

	const std::vector<int32_t>& GetChildren() const noexcept { return children; }
	const std::vector<int32_t>& GetLinks() const noexcept { return related; };

	void AddChild(int32_t child);

	int32_t GetTier() const { return tier; }

	bool IsParentOrChildOf(const Aspect& other) const noexcept
		{ return parent1 == other.id  || parent2 == other.id || other.parent1 == id || other.parent2 == id; }

	friend bool operator==(const Aspect& lhs, const Aspect& rhs) { return lhs.id == rhs.id; }
	friend bool operator!=(const Aspect& lhs, const Aspect& rhs) { return !(lhs == rhs); }

private:
	int32_t id = -1;
	int32_t parent1 = -1;
	int32_t parent2 = -1;
	int32_t tier = 1;
	std::vector<int32_t> children;
	std::vector<int32_t> related;
	std::string name;
};

}
