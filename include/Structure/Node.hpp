#pragma once

#include <memory>
#include <vector>

#include "Aspect.hpp"
#include "Hex.hpp"

namespace TCSolver {

class Node {
public:
	static constexpr int32_t Distance(const Node& a, const Node& b) { return Hex::Distance(a.position, b.position); }

	constexpr Node(Hex position, int32_t aspectId) noexcept : position(position), aspectId(aspectId) {};
	constexpr ~Node() = default;

	constexpr Node(const Node& other) noexcept = default;
	constexpr Node& operator=(const Node& other) noexcept = default;
	Node(Node&& other) = delete;
	Node& operator=(Node&& other) = delete;

	constexpr const Hex& GetPosition() const noexcept { return position; }
	constexpr int32_t GetAspectId() const noexcept { return aspectId; }

	friend constexpr bool operator==(const Node& lhs, const Node& rhs) noexcept
		{ return lhs.position == rhs.position && lhs.aspectId == rhs.aspectId; }
	friend constexpr bool operator!=(const Node& lhs, const Node& rhs) noexcept { return !(lhs == rhs); }

private:
	Hex position;
	int32_t aspectId;
};

}

namespace std {
	template<>
	struct hash<TCSolver::Node> {
		size_t operator()(const TCSolver::Node& node) const noexcept {
			uint64_t a = hash<TCSolver::Hex>()(node.GetPosition());
			uint64_t b = static_cast<uint64_t>(node.GetAspectId());
			return a ^ (b + 0x9E3779B97F4A7C15ULL + (a << 6) + (a >> 2));
		}
	};
}
