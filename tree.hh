#pragma once

#include "iserializable.hh"
#include "nodeallocator.hh"
#include "node.hh"
#include "intermediatenode.hh"
#include "leaf.hh"

#include <optional>
#include <filesystem>

template <typename K, typename V>
class Tree {
	using Index = std::size_t;

	NodeAllocator<K, V> allocator;

protected:
	void erase_node(Index);

public:
	// creates a new file
	Tree(std::filesystem::path const&, std::size_t);

	// loads an existing database file
	Tree(std::filesystem::path const&);

	auto search(K const&) -> std::optional<V>;
	void insert(K const&, V const&);

	inline auto get_root() -> Node<K, V>& {
		return allocator.get_root();
	}

	inline auto get_height() -> std::size_t {
		return allocator.get_height();
	}
};

#include "tree.tcc"
