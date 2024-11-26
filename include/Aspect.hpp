#pragma once

class Aspect {

private:
	static short count;

	short id;
	short parent1;
	short parent2;

public:
	Aspect();
	Aspect(short parent1, short parent2);
	Aspect(const Aspect& other);
	Aspect(Aspect&& other) noexcept;

	short getId() const;
	short getParent1() const;
	short getParent2() const;

	bool isRelated(const Aspect& other) const;

	Aspect& operator=(const Aspect& other);
	Aspect& operator=(Aspect&& other) noexcept;
};