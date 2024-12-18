#include <iostream>

#include "AStar.hpp"
#include "Config.hpp"
#include "Graph.hpp"
#include "GraphError.hpp"
#include "NodeManager.hpp"
#include "ParseError.hpp"
#include "SolverError.hpp"

using namespace TCSolver;

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

		std::shared_ptr<NodeManager> manager = std::make_shared<NodeManager>();

		Graph graph(config.GetGridSize(), manager);
		graph.AddTerminals(std::move(config.GetTerminals()));
		// graph.Print();

		Solvers::AStar aStar;
		Graph solution = aStar.Solve(graph);
		solution.Print();
	} catch (const Error::ParseError& error) {
		std::cerr << error.what() << std::endl;
		return 2;
	} catch (const Error::GraphError& error) {
		std::cerr << error.what() << std::endl;
		return 3;
	} catch (const Error::SolverError& error) {
		std::cerr << error.what() << std::endl;
		return 4;
	}
}