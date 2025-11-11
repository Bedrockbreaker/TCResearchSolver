#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Config.hpp"
#include "Hex.hpp"
#include "Node.hpp"

namespace TCSolver {

class Graph {
public:
	static std::unordered_map<Hex, uint64_t> HEX_ENCODINGS;
	static std::unordered_map<uint64_t, Hex> HEX_DECODINGS;

	using Graph_t = std::unordered_map<Hex, Node>;

	Graph(const Config& config) noexcept;
	~Graph() noexcept = default;

	Graph(Graph&& other) = delete;
	Graph& operator=(Graph&& other) = delete;
	Graph(const Graph& other) = delete;
	Graph& operator=(const Graph& other) = delete;

	Graph_t::iterator begin() noexcept { return nodes.begin(); };
	Graph_t::iterator end() noexcept { return nodes.end(); };
	Graph_t::const_iterator cbegin() const noexcept { return nodes.cbegin(); }
	Graph_t::const_iterator cend() const noexcept { return nodes.cend(); }

	const Node& Add(Hex position, int32_t aspectId);
	const Node& At(Hex position) const;

	int32_t GetSideLength() const { return sideLength; };
	void AddTerminals(const std::vector<Hex>& newTerminals);
	const std::unordered_set<Hex>& GetTerminals() const { return terminals; }
	bool IsTerminal(Hex position) const { return terminals.contains(position); };
	const Config& GetConfig() const { return config; }

	// std::vector<NodePtr> GetNeighbors(Hex position) const;

	uint64_t GetPlacementMask() const;

	bool Contains(Hex position) const
		{ return Hex::Distance(Hex::ZERO, position) < sideLength && nodes.contains(position); }

	void Print() const;

private:
	int sideLength;

	const Config& config;
	Graph_t nodes;
	std::unordered_set<Hex> terminals;
};

inline std::unordered_map<Hex, uint64_t> Graph::HEX_ENCODINGS = {
	{ { 0,  0}, 1ULL << 0  },

	{ {-1,  0}, 1ULL << 1  },
	{ {-1,  1}, 1ULL << 2  },
	{ { 0, -1}, 1ULL << 3  },
	{ { 0,  1}, 1ULL << 4  },
	{ { 1, -1}, 1ULL << 5  },
	{ { 1,  0}, 1ULL << 6  },

	{ {-2,  0}, 1ULL << 7  },
	{ {-2,  1}, 1ULL << 8  },
	{ {-2,  2}, 1ULL << 9  },
	{ {-1, -1}, 1ULL << 10 },
	{ {-1,  2}, 1ULL << 11 },
	{ { 0, -2}, 1ULL << 12 },
	{ { 0,  2}, 1ULL << 13 },
	{ { 1, -2}, 1ULL << 14 },
	{ { 1,  1}, 1ULL << 15 },
	{ { 2, -2}, 1ULL << 16 },
	{ { 2, -1}, 1ULL << 17 },
	{ { 2,  0}, 1ULL << 18 },

	{ {-3,  0}, 1ULL << 19 },
	{ {-3,  1}, 1ULL << 20 },
	{ {-3,  2}, 1ULL << 21 },
	{ {-3,  3}, 1ULL << 22 },
	{ {-2, -1}, 1ULL << 23 },
	{ {-2,  3}, 1ULL << 24 },
	{ {-1, -2}, 1ULL << 25 },
	{ {-1,  3}, 1ULL << 26 },
	{ { 0, -3}, 1ULL << 27 },
	{ { 0,  3}, 1ULL << 28 },
	{ { 1, -3}, 1ULL << 29 },
	{ { 1,  2}, 1ULL << 30 },
	{ { 2, -3}, 1ULL << 31 },
	{ { 2,  1}, 1ULL << 32 },
	{ { 3, -3}, 1ULL << 33 },
	{ { 3, -2}, 1ULL << 34 },
	{ { 3, -1}, 1ULL << 35 },
	{ { 3,  0}, 1ULL << 36 },

	{ {-4,  0}, 1ULL << 37 },
	{ {-4,  1}, 1ULL << 38 },
	{ {-4,  2}, 1ULL << 39 },
	{ {-4,  3}, 1ULL << 40 },
	{ {-4,  4}, 1ULL << 41 },
	{ {-3, -1}, 1ULL << 42 },
	{ {-3,  4}, 1ULL << 43 },
	{ {-2, -2}, 1ULL << 44 },
	{ {-2,  4}, 1ULL << 45 },
	{ {-1, -3}, 1ULL << 46 },
	{ {-1,  4}, 1ULL << 47 },
	{ { 0, -4}, 1ULL << 48 },
	{ { 0,  4}, 1ULL << 49 },
	{ { 1, -4}, 1ULL << 50 },
	{ { 1,  3}, 1ULL << 51 },
	{ { 2, -4}, 1ULL << 52 },
	{ { 2,  2}, 1ULL << 53 },
	{ { 3, -4}, 1ULL << 54 },
	{ { 3,  1}, 1ULL << 55 },
	{ { 4, -4}, 1ULL << 56 },
	{ { 4, -3}, 1ULL << 57 },
	{ { 4, -2}, 1ULL << 58 },
	{ { 4, -1}, 1ULL << 59 },
	{ { 4,  0}, 1ULL << 60 }
	// 3 unused bits
};

inline std::unordered_map<uint64_t, Hex> Graph::HEX_DECODINGS = {
	{ 1ULL << 0,  { 0,  0} },

	{ 1ULL << 1,  {-1,  0} },
	{ 1ULL << 2,  {-1,  1} },
	{ 1ULL << 3,  { 0, -1} },
	{ 1ULL << 4,  { 0,  1} },
	{ 1ULL << 5,  { 1, -1} },
	{ 1ULL << 6,  { 1,  0} },

	{ 1ULL << 7,  {-2,  0} },
	{ 1ULL << 8,  {-2,  1} },
	{ 1ULL << 9,  {-2,  2} },
	{ 1ULL << 10, {-1, -1} },
	{ 1ULL << 11, {-1,  2} },
	{ 1ULL << 12, { 0, -2} },
	{ 1ULL << 13, { 0,  2} },
	{ 1ULL << 14, { 1, -2} },
	{ 1ULL << 15, { 1,  1} },
	{ 1ULL << 16, { 2, -2} },
	{ 1ULL << 17, { 2, -1} },
	{ 1ULL << 18, { 2,  0} },

	{ 1ULL << 19, {-3,  0} },
	{ 1ULL << 20, {-3,  1} },
	{ 1ULL << 21, {-3,  2} },
	{ 1ULL << 22, {-3,  3} },
	{ 1ULL << 23, {-2, -1} },
	{ 1ULL << 24, {-2,  3} },
	{ 1ULL << 25, {-1, -2} },
	{ 1ULL << 26, {-1,  3} },
	{ 1ULL << 27, { 0, -3} },
	{ 1ULL << 28, { 0,  3} },
	{ 1ULL << 29, { 1, -3} },
	{ 1ULL << 30, { 1,  2} },
	{ 1ULL << 31, { 2, -3} },
	{ 1ULL << 32, { 2,  1} },
	{ 1ULL << 33, { 3, -3} },
	{ 1ULL << 34, { 3, -2} },
	{ 1ULL << 35, { 3, -1} },
	{ 1ULL << 36, { 3,  0} },

	{ 1ULL << 37, {-4,  0} },
	{ 1ULL << 38, {-4,  1} },
	{ 1ULL << 39, {-4,  2} },
	{ 1ULL << 40, {-4,  3} },
	{ 1ULL << 41, {-4,  4} },
	{ 1ULL << 42, {-3, -1} },
	{ 1ULL << 43, {-3,  4} },
	{ 1ULL << 44, {-2, -2} },
	{ 1ULL << 45, {-2,  4} },
	{ 1ULL << 46, {-1, -3} },
	{ 1ULL << 47, {-1,  4} },
	{ 1ULL << 48, { 0, -4} },
	{ 1ULL << 49, { 0,  4} },
	{ 1ULL << 50, { 1, -4} },
	{ 1ULL << 51, { 1,  3} },
	{ 1ULL << 52, { 2, -4} },
	{ 1ULL << 53, { 2,  2} },
	{ 1ULL << 54, { 3, -4} },
	{ 1ULL << 55, { 3,  1} },
	{ 1ULL << 56, { 4, -4} },
	{ 1ULL << 57, { 4, -3} },
	{ 1ULL << 58, { 4, -2} },
	{ 1ULL << 59, { 4, -1} },
	{ 1ULL << 60, { 4,  0} }
	// 3 unused bits
};

}
