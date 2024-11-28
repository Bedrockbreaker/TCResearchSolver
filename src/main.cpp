#include <iostream>

#include "Config.hpp"
#include "Node.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];

	Config config;
	config.Parse(configFile);
}