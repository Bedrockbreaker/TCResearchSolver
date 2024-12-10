#include <iostream>

#include "Config.hpp"
#include "Graph.hpp"
#include "Node.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];

	try {
		Config config;
		config.Parse(configFile);
		// config.Print();

		Graph graph(config.GetGridSize(), config.GetTerminals());
		graph.Print();
	} catch (const std::runtime_error& error) {
		std::cerr << error.what() << std::endl;
		return 1;
	}
}