#include <fstream>
#include <iostream>
#include <ryml_std.hpp>
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
	PrintYaml(root);
}

void Config::PrintYaml(ryml::ConstNodeRef node, int indentation, bool inSequence) {
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