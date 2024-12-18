#include "NodeManager.hpp"

using namespace TCSolver;

std::shared_ptr<Node> NodeManager::ComputeIfAbsent(Hex position, const Aspect* aspect) {
	if (Contains(position, aspect)) {
		return nodes.at({position, aspect}).lock();
	}

	return Move(Node(position, aspect));
}

std::shared_ptr<Node> NodeManager::Move(Node&& node) {
	if (Contains(node.getPosition(), node.getAspect())) {
		return nodes.at({node.getPosition(), node.getAspect()}).lock();
	}

	const std::shared_ptr<Node> newNode = std::make_shared<Node>(std::move(node));
	nodes.insert_or_assign({newNode->getPosition(), newNode->getAspect()}, newNode);
	return newNode;
}

bool NodeManager::Contains(Hex position, const Aspect* aspect) const {
	const auto it = nodes.find({position, aspect});
	return it != nodes.end() && !it->second.expired();
}

void NodeManager::Clear() {
	nodes.clear();
}