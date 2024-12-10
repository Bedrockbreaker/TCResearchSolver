#pragma once

#include <stdexcept>

namespace TCSolver::Error {

class GraphError : public std::runtime_error {

	using std::runtime_error::runtime_error;
};

}