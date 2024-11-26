#include <fstream>
#include <iostream>
#include <sstream>

#include "Config.hpp"

Config::Config(std::string filename) {
	std::ifstream configFile(filename);

	if (!configFile.is_open()) {
		std::cerr << "Could not open config file " << filename << std::endl;
		return;
	}

	std::stringstream buffer;
	buffer << configFile.rdbuf();
	std::string content = buffer.str();

	ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(content));

	ryml::ConstNodeRef root = tree.rootref();
	ryml::ConstNodeRef aspectNodes = AssertGetNode(root, "aspects", NodeType::Map);

	for (const auto& aspectNode : aspectNodes.children()) {
		Aspect aspect = AssertGetAspect(aspectNode);
		aspects.try_emplace(GetNodeName(aspectNode), aspect);
	}

	for (const auto& aspect : aspects) {
		std::cout
			<< aspect.first << " (" << aspect.second.getId() << ") = "
			<< GetAspectName(aspect.second.getParent1()) << " + "
			<< GetAspectName(aspect.second.getParent2()) << std::endl;
	}
}

Config::AspectMap::const_iterator Config::GetAspectIterator(short id) const {
	return std::find_if(
		aspects.begin(),
		aspects.end(),
		[id](const auto& aspect) {
			return aspect.second.getId() == id;
		}
	);
}

std::optional<Aspect> Config::GetAspect(short id) const {
	const auto it = GetAspectIterator(id);

	return it == aspects.end()
		? std::nullopt
		: std::optional<Aspect>(it->second);
}

std::string Config::GetAspectName(short id) const {
	const auto it = GetAspectIterator(id);

	return it == aspects.end() ? "" : it->first;
}

ryml::ConstNodeRef Config::AssertGetNode(
	const ryml::ConstNodeRef& parent,
	c4::csubstr key,
	NodeType type
) const {
	if (!parent.is_map()) {
		std::cerr
			<< "Expected \"" << GetNodeName(parent)
			<< "\" to be a map" << std::endl;
		exit(1);
	}

	if (!parent.has_child(key)) {
		std::cerr
			<< "Could not find \"" << key
			<< "\" in " << GetNodeName(parent)
			<< std::endl;
		exit(1);
	}

	ryml::ConstNodeRef child = parent[key];

	bool isMap = child.is_map();
	bool isSeq = child.is_seq();

	switch(type) {
		case NodeType::Map:
			if (isMap) break;
			std::cerr
				<< "Expected \"" << GetNodeName(child)
				<< "\" to be a map" << std::endl;
			exit(1);
		case NodeType::Sequence:
			if (isSeq) break;
			std::cerr
				<< "Expected \"" << GetNodeName(child)
				<< "\" to be a sequence" << std::endl;
			exit(1);
		case NodeType::Value:
			if (!isMap && !isSeq) break;
			std::cerr
				<< "Expected \"" << GetNodeName(child)
				<< "\" to be a value" << std::endl;
			exit(1);
	}

	return parent[key];
}

Aspect Config::AssertGetAspect(const ryml::ConstNodeRef& node) const {
	ryml::ConstNodeRef parent1Node = AssertGetNode(node, "parent1", NodeType::Value);
	ryml::ConstNodeRef parent2Node = AssertGetNode(node, "parent2", NodeType::Value);

	if (parent1Node.val_is_null() != parent2Node.val_is_null()) {
		std::cerr
			<< "Expected both parents of \"" << GetNodeName(node)
			<< "\" to be null or non-null" << std::endl;
		exit(1);
	}

	if (parent1Node.val_is_null()) {
		return Aspect();
	}

	std::string parent1Name;
	std::string parent2Name;
	parent1Node >> parent1Name;
	parent2Node >> parent2Name;

	if (!aspects.contains(parent1Name)) {
		std::cerr
			<< "Could not find parent1 aspect \"" << parent1Name
			<< "\" for " << GetNodeName(node)
			<< std::endl;
		exit(1);
	}

	if (!aspects.contains(parent2Name)) {
		std::cerr
			<< "Could not find parent2 aspect \"" << parent2Name
			<< "\" for " << GetNodeName(node)
			<< std::endl;
		exit(1);
	}

	return Aspect(aspects.at(parent1Name).getId(), aspects.at(parent2Name).getId());
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