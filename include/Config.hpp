#pragma once

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <unordered_map>

#include "Aspect.hpp"
#include "NodeType.hpp"

class Config {

	using AspectMap = std::unordered_map<std::string, Aspect>;

private:
	AspectMap aspects;
	
	AspectMap::const_iterator GetAspectIterator(short id) const;
	std::optional<Aspect> GetAspect(short id) const;
	std::string GetAspectName(short id) const;

	ryml::ConstNodeRef AssertGetNode(
		const ryml::ConstNodeRef& parent,
		c4::csubstr key,
		NodeType type
	) const;

	Aspect AssertGetAspect(const ryml::ConstNodeRef& node) const;

	std::string GetNodeName(const ryml::ConstNodeRef& node) const;

	void PrintYaml(
		const ryml::ConstNodeRef& node,
		int indentation = 0,
		bool inSequence = false
	) const;

public:
	Config(std::string filename);

};