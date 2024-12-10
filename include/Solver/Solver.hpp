#pragma once

#include <unordered_map>

#include "Graph.hpp"

namespace TCSolver::Solvers {
	class AbstractSolver {

	public:
		virtual Graph Solve(const Graph& graph) = 0;

	};
}