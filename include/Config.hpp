#pragma once

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <unordered_map>

#include "Aspect.hpp"
#include "NodeType.hpp"

class Config {

private:
	std::unordered_map<short, Aspect> aspects;
	std::unordered_map<std::string, short> aspectNames;

	const Aspect* GetAspect(std::string name) const;

	ryml::ConstNodeRef AssertGetNode(
		const ryml::ConstNodeRef& parent,
		c4::csubstr key,
		NodeType type
	) const;

	Aspect& AssertCreateAspect(const ryml::ConstNodeRef& node);

	std::string GetNodeName(const ryml::ConstNodeRef& node) const;

	void PrintYaml(
		const ryml::ConstNodeRef& node,
		int indentation = 0,
		bool inSequence = false
	) const;

public:
	Config();

	void Parse(const std::string& filename);

};