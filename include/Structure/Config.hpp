#pragma once

#include <ryml.hpp>
#include <ryml_std.hpp>
#include <unordered_map>

#include "Aspect.hpp"
#include "Node.hpp"
#include "NodeType.hpp"

namespace TCSolver {

class Config {
public:
	Config() = default;
	~Config() = default;

	Config(const Config&) = delete;
	Config(Config&&) = delete;
	Config& operator=(const Config&) = delete;
	Config& operator=(Config&&) = delete;

	const std::vector<Aspect>& GetAspects() const noexcept { return aspects; }
	int32_t GetGridSize() const noexcept { return gridSize; }
	const std::vector<Node>& GetTerminals() { return terminals; }

	void Parse(const std::string& filename);

	void Print() const;

private:
	int32_t gridSize = 0;
	std::vector<Aspect> aspects;
	std::unordered_map<std::string, int32_t> aspectNames;
	std::vector<Node> terminals;

	int32_t GetAspectIdByName(const std::string& name) const;

	ryml::ConstNodeRef GetYamlNode(const ryml::ConstNodeRef& parent, c4::csubstr key, ryml::NodeType type) const;

	void CreateAspectFromYamlNode(const ryml::ConstNodeRef& node);
	void CreateGraphNodeFromYamlNode(const ryml::ConstNodeRef& node);

	std::string GetYamlNodeKeyName(const ryml::ConstNodeRef& node) const;

	void PrintYaml(const ryml::ConstNodeRef& node, int32_t indentation = 0, bool bInSequence = false) const;
};

}
