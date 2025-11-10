#include <fstream>
#include <iostream>
#include <sstream>

#include "Aspect.hpp"
#include "Config.hpp"

void TCSolver::Config::Parse(const std::string& filename) {
	std::ifstream configFile(filename);

	if (!configFile.is_open()) throw std::runtime_error(std::format("Could not open config file: {}", filename));

	std::stringstream buffer;
	buffer << configFile.rdbuf();
	std::string content = buffer.str();

	ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(content));

	ryml::ConstNodeRef root = tree.rootref();
	ryml::ConstNodeRef aspectsYamlNode = GetYamlNode(root, "aspects", ryml::NodeType::Map);

	for (const ryml::ConstNodeRef& aspectYamlNode : aspectsYamlNode.children())
		CreateAspectFromYamlNode(aspectYamlNode);

	ryml::ConstNodeRef gridSizeIn = GetYamlNode(root, "grid-size", ryml::NodeType::Value);
	gridSizeIn >> gridSize;

	ryml::ConstNodeRef terminalsYamlNode = GetYamlNode(root, "terminals", ryml::NodeType::Sequence);
	for (const ryml::ConstNodeRef& terminalYamlNode : terminalsYamlNode.children())
		CreateGraphNodeFromYamlNode(terminalYamlNode);
}

int32_t TCSolver::Config::GetAspectIdByName(const std::string& name) const {
	if (!aspectNames.contains(name)) return -1;
	int32_t aspectId = aspectNames.at(name);

	assert(aspectId >= 0 && aspectId < aspects.size() && "aspectId out of range");

	return aspectId;
}

ryml::ConstNodeRef TCSolver::Config::GetYamlNode(
	const ryml::ConstNodeRef& parent,
	c4::csubstr key,
	ryml::NodeType type
) const {
	if (!parent.is_map())
		throw std::runtime_error(std::format("Expected \"{}\" to be a map", GetYamlNodeKeyName(parent)));

	if (!parent.has_child(key))
		throw std::runtime_error(std::format("Could not find \"{}\" in {}", std::string(std::string_view(key)), GetYamlNodeKeyName(parent)));

	ryml::ConstNodeRef child = parent[key];

	bool isMap = child.is_map();
	bool isSeq = child.is_seq();

	switch(type) {
		case ryml::NodeType::Map:
			if (isMap) break;
			throw std::runtime_error(std::format("Expected \"{}\" to be a map", GetYamlNodeKeyName(child)));
		case ryml::NodeType::Sequence:
			if (isSeq) break;
			throw std::runtime_error(std::format("Expected \"{}\" to be a sequence", GetYamlNodeKeyName(child)));
		case ryml::NodeType::Value:
			if (!isMap && !isSeq) break;
			throw std::runtime_error(std::format("Expected \"{}\" to be a value", GetYamlNodeKeyName(child)));
	}

	return parent[key];
}

void TCSolver::Config::CreateAspectFromYamlNode(const ryml::ConstNodeRef& node) {
	ryml::ConstNodeRef parent1Node = GetYamlNode(node, "parent1", ryml::NodeType::Value);
	ryml::ConstNodeRef parent2Node = GetYamlNode(node, "parent2", ryml::NodeType::Value);

	std::string aspectName = GetYamlNodeKeyName(node);
	if (aspectNames.contains(aspectName))
		throw std::runtime_error(std::format("Aspect \"{}\" already exists", aspectName));

	if (parent1Node.val_is_null() != parent2Node.val_is_null())
		throw std::runtime_error(std::format("Expected both parents of \"{}\" to be null or non-null", aspectName));

	// int32_t amount = -1;
	// if (node.has_child("amount")) {
	// 	ryml::ConstNodeRef amountNode = GetYamlNode(node, "amount", ryml::NodeType::Value);
	// 	amountNode >> amount;
	// }
	// if (amount < -1) throw std::runtime_error(std::format("Expected amount of \"{}\" to be >= -1", aspectName));

	int32_t aspectId = aspects.size();

	// If primal aspect (both parents are null, guaranteed if parent1 is null)
	if (parent1Node.val_is_null()) {
		aspects.push_back(Aspect(aspectId, aspectName));
		aspectNames.try_emplace(aspectName, aspectId);
		return;
	}
	// else compound aspect ...

	std::string parent1Name;
	std::string parent2Name;
	parent1Node >> parent1Name;
	parent2Node >> parent2Name;

	auto parent1It = aspectNames.find(parent1Name);
	auto parent2It = aspectNames.find(parent2Name);

	if (parent1It == aspectNames.end())
		throw std::runtime_error(std::format("Could not find parent1 aspect \"{}\" for {}", parent1Name, aspectName));

	if (parent2It == aspectNames.end())
		throw std::runtime_error(std::format("Could not find parent2 aspect \"{}\" for {}", parent2Name, aspectName));

	int32_t tier = 1 + std::max(aspects[parent1It->second].GetTier(), aspects[parent2It->second].GetTier());

	aspects.emplace_back(aspectId, aspectName, parent1It->second, parent2It->second, tier);
	aspectNames.emplace(aspectName, aspectId);
	aspects[parent1It->second].AddRelated(aspectId);
	aspects[parent2It->second].AddRelated(aspectId);
}

void TCSolver::Config::CreateGraphNodeFromYamlNode(const ryml::ConstNodeRef& node) {
	ryml::ConstNodeRef aspectYamlNode = GetYamlNode(node, "aspect", ryml::NodeType::Value);
	ryml::ConstNodeRef positionYamlNode = GetYamlNode(node, "position", ryml::NodeType::Sequence);

	int32_t aspectId = -1;

	// If terminal is specified (i.e. not a hole)
	if (!aspectYamlNode.val_is_null()) {
		std::string aspectName;
		aspectYamlNode >> aspectName;
		aspectId = GetAspectIdByName(aspectName);
		if (aspectId == -1) {
			throw std::runtime_error(
				std::format("Could not find aspect \"{}\" for {}", aspectName, GetYamlNodeKeyName(node))
			);
		}
	}

	int32_t i;
	int32_t j;
	int32_t count = 0;
	for (const ryml::ConstNodeRef& coordinateYamlNode : positionYamlNode.children()) {
		switch(count++) {
			case 0:
				coordinateYamlNode >> i;
				break;
			case 1:
				coordinateYamlNode >> j;
				break;
		}
	}
	if (count != 2) {
		throw std::runtime_error(
			std::format("Expected {} to have exactly 2 coordinates", GetYamlNodeKeyName(positionYamlNode))
		);
	}

	terminals.emplace_back(Hex(i, j), aspectId);
}

std::string TCSolver::Config::GetYamlNodeKeyName(const ryml::ConstNodeRef& node) const {
	if (node.is_root()) {
		return "config file";
	}

	if (!node.has_key()) {
		std::string nodeType = node.is_map() ? "map" : "sequence";
		return "element of " + nodeType + " in " + GetYamlNodeKeyName(node.parent());
	} else {
		std::string keyName;
		node >> ryml::key(keyName);
		return keyName;
	}
}

void TCSolver::Config::PrintYaml(const ryml::ConstNodeRef& yamlNode, int32_t indentation, bool bSequence) const {
	std::string indent(indentation * 2, ' ');
	if (yamlNode.is_root()) indentation--;

	if (yamlNode.is_keyval()) {
		std::cout << indent << (bSequence ? "- " : "") << yamlNode.key() << ": " << yamlNode.val() << "\n";
	} else if (yamlNode.is_map()) {
		if (bSequence) {
			std::cout << indent << "- ";
		} else if (yamlNode.has_key()) {
			std::cout << indent << yamlNode.key() << ":\n";
		}
		bool firstChild = true;
		for (auto const& child : yamlNode.children()) {
			PrintYaml(child, (bSequence && firstChild ? 0 : indentation + 1), false);
			firstChild = false;
		}
	} else if (yamlNode.is_seq()) {
		if (yamlNode.has_key()) std::cout << indent << (bSequence ? "- " : "") << yamlNode.key() << ":\n";
		for (auto const& child : yamlNode.children()) PrintYaml(child, indentation + 1, true);
	} else if (yamlNode.is_val()) {
		std::cout << indent << (bSequence ? "- " : "") << yamlNode.val() << "\n";
	}
}

void TCSolver::Config::Print() const {
	std::cout << "Grid size: " << gridSize << "\n\nTerminals:\n";

	for (const TCSolver::Node& terminal : terminals) {
		std::string_view aspectName = terminal.GetAspectId() == -1
			? "NULL"
			: aspects[terminal.GetAspectId()].GetName();

		std::cout << "  " << aspectName << " at " << terminal.GetPosition().to_string() << "\n";
	}

	std::cout << "\nAspects:\n";

	for (int32_t i = 0; i < aspects.size(); ++i) {
		const TCSolver::Aspect& aspect = aspects[i];
		std::cout
			<< "  " << i << " " << aspect.GetName()
			<< " (" << aspect.GetTier() << ")";

		if (aspect.GetParent1() == -1) {
			std::cout << "\n";
			continue;
		}

		std::cout
			<< " = " << aspects[aspect.GetParent1()].GetName() << " + "
			<< aspects[aspect.GetParent2()].GetName() << "\n";
	}
}
