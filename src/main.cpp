#include <iostream>

#include "AStar.hpp"
#include "Config.hpp"
#include "Graph.hpp"
#include "Hex.hpp"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <config file>" << std::endl;
		return 1;
	}

	std::string configFile = argv[1];

	TCSolver::Config config;
	config.Parse(configFile);
	config.Print();

	TCSolver::Graph graph(config);
	for (const TCSolver::Node& terminal : config.GetTerminals()) {
		graph.Add(terminal.GetPosition(), terminal.GetAspectId());
		if (terminal.GetAspectId() == -1) continue;
		graph.AddTerminals({terminal.GetPosition()});
	}
	graph.Print();

	std::vector<TCSolver::AStar::State> solution = TCSolver::AStar::Solve(
		graph,
		*graph.GetTerminals().cbegin(),
		*(++graph.GetTerminals().cbegin())
	);

	std::cout << "Solution: " << std::endl;
	for (const TCSolver::AStar::State& state : solution) {
		if (!graph.IsTerminal(state.position)) graph.Add(state.position, state.aspectId);
	}
	graph.Print();
}
