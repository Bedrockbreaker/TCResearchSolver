#pragma once

#include <string>

class Aspect {

private:
	static short count;

	std::string name;
	short id;
	const Aspect* parent1;
	const Aspect* parent2;
	int tier;
	int amount;

public:
	Aspect(const std::string& name, int amount = -1);
	Aspect(
		const std::string& name,
		const Aspect* parent1,
		const Aspect* parent2,
		int amount = -1
	);
	Aspect(Aspect&& other) noexcept;
	Aspect(const Aspect&) = delete;

	std::string_view getName() const noexcept;
	short getId() const;
	const Aspect* getParent1() const;
	const Aspect* getParent2() const;
	int getTier() const;
	int getAmount() const;

	bool isRelated(const Aspect& other) const;

	Aspect& operator=(Aspect&& other) noexcept;
	Aspect& operator=(const Aspect&) = delete;

	bool operator==(const Aspect& other) const;
};