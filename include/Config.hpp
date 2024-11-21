#pragma once

#include <ryml.hpp>
#include <unordered_map>

#include "Aspect.hpp"

class Config {

private:
	std::unordered_map<std::string, Aspect> aspects;
	
	void PrintYaml(ryml::ConstNodeRef node, int indentation = 0, bool inSequence = false);

public:
	Config(std::string filename);

};