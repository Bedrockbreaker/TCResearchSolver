#include <chrono>
#include <iostream>

#include "AStar.hpp"
#include "Config.hpp"
#include "DreyfusWagner.hpp"
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

	int32_t terminals = graph.GetTerminals().size();
	if (terminals <= 0) {
		std::cerr << "Not enough terminals" << std::endl;
		return 1;
	} else if (terminals == 1) {
		// TODO: explore just the neighbors and choose the cheapest
	} else if (terminals == 2) {
		std::vector<TCSolver::AStar::State> solution;

		auto start = std::chrono::high_resolution_clock::now();

		bool bSuccess = TCSolver::AStar::Solve(graph,
			*graph.GetTerminals().cbegin(),
			*(++graph.GetTerminals().cbegin()),
			solution
		);

		auto end = std::chrono::high_resolution_clock::now();

		if (!bSuccess) {
			std::cerr
				<< "No solution found (took "
				<< std::chrono::duration_cast<std::chrono::microseconds>(end - start)
				<< ")"
				<< std::endl;
			return 0;
		}

		std::cout
			<< "Solution found in "
			<< std::chrono::duration_cast<std::chrono::microseconds>(end - start)
			<< ": "
			<< std::endl;

		for (const TCSolver::AStar::State& state : solution) {
			if (!graph.IsTerminal(state.position)) graph.Add(state.position, state.aspectId);
		}

		graph.Print();
	} else if (terminals <= 15) {
		auto start = std::chrono::high_resolution_clock::now();

		bool bSuccess = TCSolver::DreyfusWagner::Solve(graph);

		auto end = std::chrono::high_resolution_clock::now();

		std::cout
			<< "Solution found in "
			<< std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
			<< ": "
			<< std::endl;
		// for (const TCSolver::DreyfusWagner::State& state : solution) {
		// 	if (!graph.IsTerminal(state.position)) graph.Add(state.position, state.aspectId);
		// }
		// graph.Print();
	} else {
		std::cerr << "Too many terminals" << std::endl;
		return 1;
	}
}
