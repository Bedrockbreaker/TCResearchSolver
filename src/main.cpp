#include <iostream>

#include "Node.hpp"

int main(int argc, char* argv[]) {

	Node node(0, 0, 0);

	HexPosition position = node.getPosition();

	std::cout << position.i << " " << position.j << " " << position.k << std::endl;
}