#pragma once

#include "iserializable.hh"
#include "node.hh"
#include "intermediatenode.hh"
#include "leaf.hh"

#include <deque>
#include <vector>
#include <memory>
#include <filesystem>
#include <fstream>

using Index = std::size_t;

template <typename K, typename V>
class NodeAllocator : ISerializable {
	std::filesystem::path path;
	std::fstream file;

	std::deque<std::unique_ptr<Node<K, V>>> nodes;
	std::vector<Index> empty_blocks;
	std::size_t node_count = 0;
	std::size_t tree_height = 0;
	Index root_idx;

	std::size_t node_degree;
	std::size_t leaf_degree;
	std::size_t block_size;

public:
	// creates a new file
	NodeAllocator(std::filesystem::path const&, std::size_t block_size);

	// loads an existing database file
	NodeAllocator(std::filesystem::path const&);

	
	// from ISerializable
	void serialize(std::ostream&) const override;
	void deserialize(std::istream&) override;
	auto size_on_disk() const -> std::size_t override;
	
	inline auto get_root() -> Node<K, V>& {
		return *nodes[0];
	}

	void replace_root();
	auto allocate(Node<K, V> const&) -> Index;
	void defragment();

	auto cache_leaf(IntermediateNode<K, V>&, Index)
		-> Leaf<K, V>&;

	auto cache_intermediate(IntermediateNode<K, V>&, Index)
		-> IntermediateNode<K, V>&;

	inline auto get_path() const -> std::filesystem::path {
		return path;
	}

	inline auto get_block_size() const -> std::size_t {
		return block_size;
	}

	inline auto get_height() const -> std::size_t {
		return tree_height;
	}
};

#include "nodeallocator.tcc"
