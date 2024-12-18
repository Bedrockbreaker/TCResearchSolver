#pragma once

#include <memory>
#include <unordered_map>

#include "Hex.hpp"
#include "Node.hpp"

namespace std {
	template<>
	struct hash<std::pair<TCSolver::Hex, const TCSolver::Aspect*>> {
		size_t operator()(
			const std::pair<TCSolver::Hex, const TCSolver::Aspect*>& pair
		) const {
			size_t h1 = std::hash<TCSolver::Hex>()(pair.first);
			size_t h2 = std::hash<const TCSolver::Aspect*>()(pair.second);
			return h1 ^ (h2 << 1);
		}
	};
}

namespace TCSolver {

class NodeManager {

private:
	std::unordered_map<std::pair<Hex, const Aspect*>, std::weak_ptr<Node>> nodes;

public:
	NodeManager() = default;

	std::shared_ptr<Node> ComputeIfAbsent(Hex position, const Aspect* aspect);
	std::shared_ptr<Node> Move(Node&& node);

	bool Contains(Hex position, const Aspect* aspect) const;

	void Clear();
};

}