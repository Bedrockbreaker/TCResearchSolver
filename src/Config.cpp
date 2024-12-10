#include <fstream>
#include <iostream>
#include <sstream>

#include "Config.hpp"

Config::Config() {}

int Config::GetGridSize() const {
	return size;
}

std::vector<Node>& Config::GetTerminals() {
	return terminals;
}

void Config::Parse(const std::string& filename) {
	std::ifstream configFile(filename);

	if (!configFile.is_open()) {
		throw std::runtime_error("Could not open config file " + filename);
	}

	std::stringstream buffer;
	buffer << configFile.rdbuf();
	std::string content = buffer.str();

	ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(content));

	ryml::ConstNodeRef root = tree.rootref();
	ryml::ConstNodeRef aspectNodes = AssertGetNode(root, "aspects", NodeType::Map);

	for (const auto& aspectNode : aspectNodes.children()) {
		AssertCreateAspect(aspectNode);
	}

	ryml::ConstNodeRef gridSize = AssertGetNode(root, "grid-size", NodeType::Value);
	gridSize >> size;

	ryml::ConstNodeRef staticNodes = AssertGetNode(root, "static-nodes", NodeType::Sequence);
	for (const auto& staticNode : staticNodes.children()) {
		AssertCreateNode(staticNode);
	}
}

const Aspect* Config::GetAspect(std::string name) const {
	auto nameIt = aspectNames.find(name);
	if (nameIt == aspectNames.end()) return nullptr;

	auto aspectIt = aspects.find(nameIt->second);
	if (aspectIt == aspects.end()) return nullptr;

	return &aspectIt->second;
}

ryml::ConstNodeRef Config::AssertGetNode(
	const ryml::ConstNodeRef& parent,
	c4::csubstr key,
	NodeType type
) const {
	if (!parent.is_map()) {
		throw std::runtime_error(
			"Expected \"" + GetNodeName(parent) + "\" to be a map"
		);
	}

	if (!parent.has_child(key)) {
		throw std::runtime_error(
			"Could not find \""
			+ std::string(std::string_view(key))
			+ "\" in " + GetNodeName(parent)
		);
	}

	ryml::ConstNodeRef child = parent[key];

	bool isMap = child.is_map();
	bool isSeq = child.is_seq();

	switch(type) {
		case NodeType::Map:
			if (isMap) break;
			throw std::runtime_error("Expected \"" + GetNodeName(child) + "\" to be a map");
		case NodeType::Sequence:
			if (isSeq) break;
			throw std::runtime_error("Expected \"" + GetNodeName(child) + "\" to be a sequence");
		case NodeType::Value:
			if (!isMap && !isSeq) break;
			throw std::runtime_error("Expected \"" + GetNodeName(child) + "\" to be a value");
	}

	return parent[key];
}

Aspect& Config::AssertCreateAspect(const ryml::ConstNodeRef& node) {
	ryml::ConstNodeRef parent1Node = AssertGetNode(node, "parent1", NodeType::Value);
	ryml::ConstNodeRef parent2Node = AssertGetNode(node, "parent2", NodeType::Value);

	std::string aspectName = GetNodeName(node);

	if (parent1Node.val_is_null() != parent2Node.val_is_null()) {
		throw std::runtime_error(
			"Expected both parents of \""
			+ aspectName
			+ "\" to be null or non-null"
		);
	}

	int amount = -1;
	if (node.has_child("amount")) {
		ryml::ConstNodeRef amountNode = AssertGetNode(node, "amount", NodeType::Value);
		amountNode >> amount;
	}

	if (amount < -1) {
		throw std::runtime_error(
			"Expected amount of \"" + aspectName + "\" to be >= -1"
		);
	}

	if (parent1Node.val_is_null()) {
		Aspect aspect(aspectName, amount);
		auto [aspectIt, inserted] = aspects.try_emplace(aspect.getId(), std::move(aspect));
		if (!inserted) {
			throw std::runtime_error("Aspect \"" + aspectName + "\" already exists");
		}
		aspectNames.try_emplace(aspectName, aspectIt->first);
		return aspectIt->second;
	}

	std::string parent1Name;
	std::string parent2Name;
	parent1Node >> parent1Name;
	parent2Node >> parent2Name;

	auto parent1It = aspectNames.find(parent1Name);
	auto parent2It = aspectNames.find(parent2Name);

	if (parent1It == aspectNames.end()) {
		throw std::runtime_error(
			"Could not find parent1 aspect \""
			+ parent1Name + "\" for " + aspectName
		);
	}

	if (parent2It == aspectNames.end()) {
		throw std::runtime_error(
			"Could not find parent2 aspect \""
			+ parent2Name + "\" for " + aspectName
		);
	}

	Aspect aspect(
		aspectName,
		&aspects.at(parent1It->second),
		&aspects.at(parent2It->second),
		amount
	);
	auto [aspectIt, inserted] = aspects.try_emplace(aspect.getId(), std::move(aspect));
	if (!inserted) {
		throw std::runtime_error("Aspect \"" + aspectName + "\" already exists");
	}
	aspectNames.try_emplace(aspectName, aspectIt->first);

	return aspectIt->second;
}

Node& Config::AssertCreateNode(const ryml::ConstNodeRef& node) {
	ryml::ConstNodeRef aspectNode = AssertGetNode(node, "aspect", NodeType::Value);
	ryml::ConstNodeRef positionNode = AssertGetNode(node, "position", NodeType::Sequence);

	const Aspect* aspect = nullptr;

	if (!aspectNode.val_is_null()) {
		std::string aspectName;
		aspectNode >> aspectName;
		aspect = GetAspect(aspectName);
		if (aspect == nullptr) {
			throw std::runtime_error(
				"Could not find aspect \"" + aspectName + "\" for " + GetNodeName(node)
			);
		}
	}

	int i;
	int j;
	int count = 0;
	for (const auto& coordinateNode : positionNode.children()) {
		switch(count++) {
			case 0:
				coordinateNode >> i;
				break;
			case 1:
				coordinateNode >> j;
				break;
		}
	}
	if (count != 2) {
		throw std::runtime_error(
			"Expected " + GetNodeName(positionNode) + " to have 2 coordinates"
		);
	}

	terminals.push_back(Node(Hex(i, j), aspect));

	return terminals.back();
}

std::string Config::GetNodeName(const ryml::ConstNodeRef& node) const {
	if (node.is_root()) {
		return "config file";
	}

	if (!node.has_key()) {
		std::string nodeType = node.is_map() ? "map" : "sequence";
		return "element of " + nodeType + " in " + GetNodeName(node.parent());
	} else {
		std::string keyName;
		node >> ryml::key(keyName);
		return keyName;
	}
}

void Config::PrintYaml(
	const ryml::ConstNodeRef& node,
	int indentation,
	bool inSequence
) const {
	std::string indent(indentation * 2, ' ');
	if (node.is_root()) {
		indentation--;
	}

	if (node.is_keyval()) {
		std::cout
			<< indent << (inSequence ? "- " : "")
			<< node.key() << ": " << node.val() << "\n";
	} else if (node.is_map()) {
		if (inSequence) {
			std::cout << indent << "- ";
		} else if (node.has_key()) {
			std::cout << indent << node.key() << ":\n";
		}
		bool firstChild = true;
		for (auto const& child : node.children()) {
			PrintYaml(child, (inSequence && firstChild ? 0 : indentation + 1), false);
			firstChild = false;
		}
	} else if (node.is_seq()) {
		if (node.has_key()) {
			std::cout
				<< indent << (inSequence ? "- " : "")
				<< node.key() << ":\n";
		}
		for (auto const& child : node.children()) {
			PrintYaml(child, indentation + 1, true);
		}
	} else if (node.is_val()) {
		std::cout
			<< indent << (inSequence ? "- " : "")
			<< node.val() << "\n";
	}
}

void Config::Print() const {
	std::cout << "Grid size: " << size << "\n\nTerminals:\n";

	for (const auto& terminal : terminals) {
		std::string_view aspectName = terminal.getAspect() == nullptr
			? "NULL"
			: terminal.getAspect()->getName();

		std::cout
			<< "  " << aspectName << " at "
			<< terminal.getPosition().toString() << "\n";
	}

	std::cout << "\nAspects:\n";

	for (const auto& aspect : aspects) {
		std::cout
			<< "  " << aspect.first << " " << aspect.second.getName()
			<< " (" << aspect.second.getTier() << ") x"
			<< aspect.second.getAmount();
		
		if (aspect.second.getParent1() == nullptr) {
			std::cout << "\n";
			continue;
		}
		
		std::cout
			<< " = " << aspect.second.getParent1()->getName() << " + "
			<< aspect.second.getParent2()->getName() << "\n";
	}
}