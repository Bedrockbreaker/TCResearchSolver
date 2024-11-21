#pragma once

class Aspect {

public:
	const short id;

	const short parent1;
	const short parent2;

	bool isRelated(const Aspect& other) const;
};