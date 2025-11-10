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
	// static std::unordered_map<uint64_t, Hex> HEX_DECODINGS;
	static std::unordered_map<Hex, uint8_t> HEX_ENCODINGS_PACKED;
	static std::vector<Hex> HEX_DECODINGS_PACKED;

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

inline std::unordered_map<Hex, uint8_t> Graph::HEX_ENCODINGS_PACKED = {
	{ { 0,  0}, 1  },

	{ {-1,  0}, 2  },
	{ {-1,  1}, 3  },
	{ { 0, -1}, 4  },
	{ { 0,  1}, 5  },
	{ { 1, -1}, 6  },
	{ { 1,  0}, 7  },

	{ {-2,  0}, 8  },
	{ {-2,  1}, 9  },
	{ {-2,  2}, 10 },
	{ {-1, -1}, 11 },
	{ {-1,  2}, 12 },
	{ { 0, -2}, 13 },
	{ { 0,  2}, 14 },
	{ { 1, -2}, 15 },
	{ { 1,  1}, 16 },
	{ { 2, -2}, 17 },
	{ { 2, -1}, 18 },
	{ { 2,  0}, 19 },

	{ {-3,  0}, 20 },
	{ {-3,  1}, 21 },
	{ {-3,  2}, 22 },
	{ {-3,  3}, 23 },
	{ {-2, -1}, 24 },
	{ {-2,  3}, 25 },
	{ {-1, -2}, 26 },
	{ {-1,  3}, 27 },
	{ { 0, -3}, 28 },
	{ { 0,  3}, 29 },
	{ { 1, -3}, 30 },
	{ { 1,  2}, 31 },
	{ { 2, -3}, 32 },
	{ { 2,  1}, 33 },
	{ { 3, -3}, 34 },
	{ { 3, -2}, 35 },
	{ { 3, -1}, 36 },
	{ { 3,  0}, 37 },

	{ {-4,  0}, 38 },
	{ {-4,  1}, 39 },
	{ {-4,  2}, 40 },
	{ {-4,  3}, 41 },
	{ {-4,  4}, 42 },
	{ {-3, -1}, 43 },
	{ {-3,  4}, 44 },
	{ {-2, -2}, 45 },
	{ {-2,  4}, 46 },
	{ {-1, -3}, 47 },
	{ {-1,  4}, 48 },
	{ { 0, -4}, 49 },
	{ { 0,  4}, 50 },
	{ { 1, -4}, 51 },
	{ { 1,  3}, 52 },
	{ { 2, -4}, 53 },
	{ { 2,  2}, 54 },
	{ { 3, -4}, 55 },
	{ { 3,  1}, 56 },
	{ { 4, -4}, 57 },
	{ { 4, -3}, 58 },
	{ { 4, -2}, 59 },
	{ { 4, -1}, 60 },
	{ { 4,  0}, 61 }
};

inline std::vector<Hex> Graph::HEX_DECODINGS_PACKED = {
	{ 0,  0},

	{-1,  0},
	{-1,  1},
	{ 0, -1},
	{ 0,  1},
	{ 1, -1},
	{ 1,  0},

	{-2,  0},
	{-2,  1},
	{-2,  2},
	{-1, -1},
	{-1,  2},
	{ 0, -2},
	{ 0,  2},
	{ 1, -2},
	{ 1,  1},
	{ 2, -2},
	{ 2, -1},
	{ 2,  0},

	{-3,  0},
	{-3,  1},
	{-3,  2},
	{-3,  3},
	{-2, -1},
	{-2,  3},
	{-1, -2},
	{-1,  3},
	{ 0, -3},
	{ 0,  3},
	{ 1, -3},
	{ 1,  2},
	{ 2, -3},
	{ 2,  1},
	{ 3, -3},
	{ 3, -2},
	{ 3, -1},
	{ 3,  0},

	{-4,  0},
	{-4,  1},
	{-4,  2},
	{-4,  3},
	{-4,  4},
	{-3, -1},
	{-3,  4},
	{-2, -2},
	{-2,  4},
	{-1, -3},
	{-1,  4},
	{ 0, -4},
	{ 0,  4},
	{ 1, -4},
	{ 1,  3},
	{ 2, -4},
	{ 2,  2},
	{ 3, -4},
	{ 3,  1},
	{ 4, -4},
	{ 4, -3},
	{ 4, -2},
	{ 4, -1},
	{ 4,  0}
};

// inline std::unordered_map<uint64_t, Hex> Graph::HEX_DECODINGS = {
// 	{ 0x0000'0000'0000'0001ULL, {-4,  0} },
// 	{ 0x0000'0000'0000'0002ULL, {-4,  1} },
// 	{ 0x0000'0000'0000'0004ULL, {-4,  2} },
// 	{ 0x0000'0000'0000'0008ULL, {-4,  3} },
// 	{ 0x0000'0000'0000'0010ULL, {-4,  4} },
// 	{ 0x0000'0000'0000'0020ULL, {-3, -1} },
// 	{ 0x0000'0000'0000'0040ULL, {-3,  0} },
// 	{ 0x0000'0000'0000'0080ULL, {-3,  1} },
// 	{ 0x0000'0000'0000'0100ULL, {-3,  2} },
// 	{ 0x0000'0000'0000'0200ULL, {-3,  3} },
// 	{ 0x0000'0000'0000'0400ULL, {-3,  4} },
// 	{ 0x0000'0000'0000'0800ULL, {-2, -2} },
// 	{ 0x0000'0000'0000'1000ULL, {-2, -1} },
// 	{ 0x0000'0000'0000'2000ULL, {-2,  0} },
// 	{ 0x0000'0000'0000'4000ULL, {-2,  1} },
// 	{ 0x0000'0000'0000'8000ULL, {-2,  2} },
// 	{ 0x0000'0000'0001'0000ULL, {-2,  3} },
// 	{ 0x0000'0000'0002'0000ULL, {-2,  4} },
// 	{ 0x0000'0000'0004'0000ULL, {-1, -3} },
// 	{ 0x0000'0000'0008'0000ULL, {-1, -2} },
// 	{ 0x0000'0000'0010'0000ULL, {-1, -1} },
// 	{ 0x0000'0000'0020'0000ULL, {-1,  0} },
// 	{ 0x0000'0000'0040'0000ULL, {-1,  1} },
// 	{ 0x0000'0000'0080'0000ULL, {-1,  2} },
// 	{ 0x0000'0000'0100'0000ULL, {-1,  3} },
// 	{ 0x0000'0000'0200'0000ULL, {-1,  4} },
// 	{ 0x0000'0000'0400'0000ULL, { 0, -4} },
// 	{ 0x0000'0000'0800'0000ULL, { 0, -3} },
// 	{ 0x0000'0000'1000'0000ULL, { 0, -2} },
// 	{ 0x0000'0000'2000'0000ULL, { 0, -1} },
// 	{ 0x0000'0000'4000'0000ULL, { 0,  0} },
// 	{ 0x0000'0000'8000'0000ULL, { 0,  1} },
// 	{ 0x0000'0001'0000'0000ULL, { 0,  2} },
// 	{ 0x0000'0002'0000'0000ULL, { 0,  3} },
// 	{ 0x0000'0004'0000'0000ULL, { 0,  4} },
// 	{ 0x0000'0008'0000'0000ULL, { 1, -4} },
// 	{ 0x0000'0010'0000'0000ULL, { 1, -3} },
// 	{ 0x0000'0020'0000'0000ULL, { 1, -2} },
// 	{ 0x0000'0040'0000'0000ULL, { 1, -1} },
// 	{ 0x0000'0080'0000'0000ULL, { 1,  0} },
// 	{ 0x0000'0100'0000'0000ULL, { 1,  1} },
// 	{ 0x0000'0200'0000'0000ULL, { 1,  2} },
// 	{ 0x0000'0400'0000'0000ULL, { 1,  3} },
// 	{ 0x0000'0800'0000'0000ULL, { 2, -4} },
// 	{ 0x0000'1000'0000'0000ULL, { 2, -3} },
// 	{ 0x0000'2000'0000'0000ULL, { 2, -2} },
// 	{ 0x0000'4000'0000'0000ULL, { 2, -1} },
// 	{ 0x0000'8000'0000'0000ULL, { 2,  0} },
// 	{ 0x0001'0000'0000'0000ULL, { 2,  1} },
// 	{ 0x0002'0000'0000'0000ULL, { 2,  2} },
// 	{ 0x0004'0000'0000'0000ULL, { 3, -4} },
// 	{ 0x0008'0000'0000'0000ULL, { 3, -3} },
// 	{ 0x0010'0000'0000'0000ULL, { 3, -2} },
// 	{ 0x0020'0000'0000'0000ULL, { 3, -1} },
// 	{ 0x0040'0000'0000'0000ULL, { 3,  0} },
// 	{ 0x0080'0000'0000'0000ULL, { 3,  1} },
// 	{ 0x0100'0000'0000'0000ULL, { 4, -4} },
// 	{ 0x0200'0000'0000'0000ULL, { 4, -3} },
// 	{ 0x0400'0000'0000'0000ULL, { 4, -2} },
// 	{ 0x0800'0000'0000'0000ULL, { 4, -1} },
// 	{ 0x1000'0000'0000'0000ULL, { 4,  0} }
// 	// 3 unused bits
// };

}
